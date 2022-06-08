#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include "ThreadPool.h"
#include "InvertIndex.h"


std::vector <Entry> InvertedIndex::GetWordCount(const std::string& word) 
{
	return freqDictionary[word];
}

std::vector<std::string> ParseDocs(std::string line) 
{
	std::vector <std::string> words;

	std::string token;

	std::istringstream sStream(line);

	// derive single words from line
	while (sStream >> token)
	{
		words.push_back(token);
	}
	return words;
}

std::vector<std::string> InvertedIndex::GetDocs() 
{
	return docs;
}

void InvertedIndex::SetDocs(std::vector<std::string> _docs)
{
	docs = _docs;
}

size_t Count(std::string word, std::string line) 
{
	size_t count = 0;

	std::string token;

	std::istringstream sStream(line);

	while (sStream >> token)
	{
		//  exclude the occurrence of a token in another word
		if (token == word) ++count;
	}
	return count;
}

std::map<std::string, std::map<size_t, size_t>> ToOrderedDictionary(
	std::map<std::string, std::vector<Entry>>& freqDictionary)
{
	std::map<std::string, std::map<size_t, size_t>> orderedDictionary;

	for (auto& El : freqDictionary)  // words
	{
		for (auto& el : El.second)  //id's & counts
		{
			orderedDictionary[El.first][el.docId] = el.count;
		}
	}

	return orderedDictionary;
}

std::map<std::string, std::vector<Entry>> ToFreqDictionary(
	std::map<std::string, std::map<size_t, size_t>>& orderedDictionary)
{
	std::map<std::string, std::vector<Entry>> freqDictionary;

	for (auto& El : orderedDictionary)  // words
	{
		for (auto& el : El.second)  //id's & counts
		{
			freqDictionary[El.first].push_back({ el.first,el.second });
		}
	}

	return freqDictionary;
}

std::mutex safeAdd;

void Indexing(size_t textId, std::vector<std::string> docs,
	std::map<std::string, std::map<size_t, size_t>>& orderedDictionary)
{
	std::vector<std::string> parsedWords;

	//  1. parse words from documents
	for (auto& el : ParseDocs(docs[textId]))
	{
		parsedWords.push_back(el);
	}

	//  2. make invert index for freq_dictionary
	for (size_t i = 0; i < docs.size(); i++) 
	{
		for (auto& El : parsedWords)
		{  
			size_t counter = Count(El, docs[i]);

			//  search word found in the document
			if (counter > 0)
			{
				std::lock_guard <std::mutex> lock(safeAdd);

				//  if this word has already been searched in the text
				if (orderedDictionary[El].count(i) == 0)
				{
					orderedDictionary[El][i] = counter;
				}
			}
		}
	}
}

std::map<std::string, std::vector<Entry>> InvertedIndex::GetFreqDictionary() 
{
	return freqDictionary;
}

void InvertedIndex::SetFreqDictionary(std::map<std::string, std::vector<Entry>> newDictionary) 
{
	freqDictionary = newDictionary;
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> inputDocs) 
{
	// renew docs
	std::vector<std::string> newDocs;

	if (GetDocs().size() > 0) 
	{
		std::vector<std::string>oldDocs = GetDocs();

		// insert new & old docs into updated_docs
		std::merge(inputDocs.begin(), inputDocs.end(),
			oldDocs.begin(), oldDocs.end(),
			std::back_inserter(newDocs));

		// set updated_docs as docs in InvertedIndex object
		SetDocs(newDocs);
	}
	else {
		/*std::cout << "input_docs is empty" << std::endl;*/
		SetDocs(inputDocs);

		newDocs = inputDocs;
	}

	std::map < std::string , std::vector<Entry >> newFreqDictionary = GetFreqDictionary();
	
	std::map < std::string , std::map<size_t, size_t >> orderedDictionary;

	if (newFreqDictionary.size() > 0) 
	{

		orderedDictionary = ToOrderedDictionary(newFreqDictionary);
	}

	unsigned int numThreads = std::thread::hardware_concurrency()-1;

	thread_pool threadPool(numThreads);

	for (int i = 0; i < newDocs.size(); i++)
	{
		// indexing each (i) text in its own thread
		threadPool.add_task(Indexing, i, newDocs, std::ref(orderedDictionary));

		//Indexing (i, newDocs, newFreqDictionary);
		/*std::cout << " doc indexing id: " << i << " started" << std::endl;*/
	}
	threadPool.wait_all();

	//  update new_freq_dictionary
	SetFreqDictionary(ToFreqDictionary(orderedDictionary));
	/*std::cout<<"Freq_dictionary containing:\n";

	for (auto& El : GetFreqDictionary()) 
	{
		std::cout << El.first << ":\n";

		for (auto& el : El.second) 
		{
			std::cout << "\t" <<
			el.docId << " : " << el.count << std::endl;
		}
	}*/
}