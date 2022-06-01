#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include "..\include\InvertIndex.h"
#include "..\include\Server.h"

InvertedIndex SearchServer::GetInvertedIndex() 
{
	return _index;
}

size_t MaxCount(std::vector<Entry> listOfOccurrences)
{
	size_t maxCount{ 0 };

	for (auto& el : listOfOccurrences)
	{
		if (el.count > maxCount) maxCount = el.count;
	}

	return maxCount;
}

void WordOccurrenceSort(std::vector<WordOccurrence>& wordOccurrence)
{
	std::sort(wordOccurrence.begin(), wordOccurrence.end(),
		[](WordOccurrence a, WordOccurrence b)
		{
			return a.count < b.count;
		});
}

void SortByRelev(std::vector<RelativeIndex>& relevance) 
{
	std::sort(relevance.begin(), relevance.end(),
		[](RelativeIndex a, RelativeIndex b) 
		{
		return a.rank > b.rank;
		});
}

void SortLess(std::vector<RelativeIndex>& relevance, int& begin, int& end) 
{
	struct 
	{
		bool operator()(RelativeIndex a, RelativeIndex b) const
		{
			return a.docId < b.docId;
		}
	} 
	customLess;
	std::sort(relevance.begin() + begin, relevance.begin() + end, customLess);
}

void SortById(std::vector<RelativeIndex>& input) 
{
	int begin{ 0 };

	float initRank = input.front().rank;

	for (int i = 0; i < input.size(); ++i) 
	{
		if (input[i].rank != initRank || i == input.size() - 1) 
		{
			initRank = input[i].rank;

			if (i == input.size() - 1)
			{
				i = input.size();
			}

			//  sorting by id part of the input
			SortLess(input, begin, i);

			//  move to next range
			begin = i;
		}
	}
}

std::vector <size_t> Union(std::vector <size_t>& vector1,
	std::vector <size_t>& vector2) 
{
	std::vector<size_t> resultDocs;

	std::set_union(vector1.begin(),
		vector1.end(),
		vector2.begin(),
		vector2.end(),
		std::back_inserter(resultDocs));

	return resultDocs;
}

std::vector<RelativeIndex> SingleSearch(std::string request,
	std::map<std::string,
	std::vector<Entry>>&
	freqDictionary) 
{
	std::vector<RelativeIndex> relevance;

	// 1. parse request line into words
	std::vector<std::string> parsedRequest = ParseDocs(request);

	// 2. unique words with max count
	std::vector<WordOccurrence> wordOccurrence;

	for (auto& El : parsedRequest) 
	{
		//  freq_dictionary contains this word
		if (freqDictionary.count(El) > 0) 
		{
			WordOccurrence temp{ El, MaxCount(freqDictionary[El]) };
			// no repeats

			bool flag = true;

			for (auto& el : wordOccurrence) 
			{
				if (el.word == temp.word)
				{
					flag = false;
				}
			}
			if (flag) {
				wordOccurrence.push_back(temp);
			}
		}
	}

	//  3. sort word_occurrence

	WordOccurrenceSort(wordOccurrence);

	//  4-5. vector of doc_id's of requested words

	std::vector <size_t> Ids;

	for (int i = 0; i < wordOccurrence.size(); i++) 
	{
		std::vector<size_t> newIds;

		for (auto& el : freqDictionary[wordOccurrence[i].word]) 
		{
			newIds.push_back(el.docId);
			std::cout << wordOccurrence[i].word << " - " << el.docId << std::endl;
		}
		//  renew answer docs (id's) on every step
		Ids = Union(Ids, newIds);
	}

	//  7. calculate relevance
	for (auto& El : Ids) //  answer docs ids
	{  
		size_t count{ 0 };

		for (auto& el : wordOccurrence) //  words & counts
		{  
			for (auto& f : freqDictionary[el.word]) 
			{
				//  search for word count of answer in freq_dict by id
				if (f.docId == El) count += f.count;
			}
		}
		relevance.push_back({ El,(float)count });
	}
	/* For now vector of Relative Index contains
	absolute relevance. We'll get relative relevance
	on next step in search function */

	if (relevance.size() > 1)
	{
		//  Sort by relevance
		SortByRelev(relevance);

		//  Sort by id
		SortById(relevance);
	}

	if (relevance.size() > 5)
	{
		relevance.resize(5);
	}
	
	return relevance;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const
	std::vector<std::string>& queries_input) 
{
	//  get freq_dictionary from InvertedIndex object
	std::map<std::string, std::vector<Entry>>freqDictionary =
		GetInvertedIndex().GetFreqDictionary();

	//  for each of queries_inputs make SingleSearch
	std::vector<std::vector<RelativeIndex>>listOfRelevance;

	for (auto& el : queries_input) 
	{
		listOfRelevance.push_back(SingleSearch(el,
			freqDictionary));
	}

	//  find max absolute relevance for each doc_id
	std::vector<size_t> maxRelevance;

	for (auto& El : listOfRelevance)
	{
		size_t temp{ 0 };

		for (auto& el : El) 
		{
			if (el.rank > temp) 
			{
				temp = el.rank;
			}
		}
		maxRelevance.push_back(temp);
	}
	std::cout << "Max relevance is: \n";
	for (auto& el : maxRelevance) 
	{
		std::cout << el << std::endl;
	}
	//  7. relative relevance
	std::vector < std::vector <RelativeIndex>> listOfRelRelev;

	for (int i = 0; i < listOfRelevance.size(); i++)
	{
		std::vector <RelativeIndex > temp;

		for (auto& el : listOfRelevance[i])
		{
			//  no division by zero
			if (maxRelevance[i] != 0) 
			{
				//  limit floating point precision
				/*float rel_relevance = std::roundf((el.rank / (float)max_relevance[i])
					* 100) / 100.0;*/

				float relativeRelevance = (el.rank / (float)maxRelevance[i]);

				temp.push_back({ el.docId, relativeRelevance });
			}
		}
		listOfRelRelev.push_back(temp);
	}

	std::cout << "Absolute relevance is: \n";

	for (auto& El : listOfRelevance)
	{
		for (auto& el : El) 
		{
			std::cout << "doc.id - " << el.docId
				<< " : rank - " << el.rank << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << "Relative relevance is: \n";
	for (auto& El : listOfRelRelev)
	{
		for (auto& el : El) 
		{
			std::cout << "doc.id - " << el.docId
				<< " : rank - " << el.rank << std::endl;
		}
		std::cout << std::endl;
	}

	return listOfRelRelev;
}

std::vector<std::vector<std::pair<int, float>>> ConvertAnswer(
	const std::vector < std::vector <RelativeIndex>> answer) 
{
	std::vector<std::vector<std::pair<int, float>>> result;

	for (auto& El : answer) 
	{
		std::vector<std::pair<int, float>> temp;

		for (auto& el : El) 
		{
			temp.push_back(std::make_pair(int(el.docId), el.rank));
		}
		result.push_back(temp);
	}

	return result;
}
