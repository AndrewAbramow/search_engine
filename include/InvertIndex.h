#pragma once

#include <iostream>
#include <vector>
#include <map>

struct Entry 
{
	size_t docId, count;
	
	// ƒанный оператор необходим дл€ проведени€ тестовых сценариев
	bool operator ==(const Entry& other) const 
	{
		return (docId == other.docId &&
			count == other.count);
	}
};

std::vector<std::string> ParseDocs(std::string line);

size_t Count(std::string word, std::string line);

std::map<std::string, std::map<size_t, size_t>> ToOrderedDictionary(
	std::map<std::string, std::vector<Entry>>& freqDictionary);

std::map<std::string, std::vector<Entry>> ToFreqDictionary(
	std::map<std::string, std::map<size_t, size_t>>& orderedDictionary);

void Indexing(size_t textId, std::vector<std::string> docs,
	std::map<std::string, std::map<size_t, size_t>>& orderedDictionary);

class InvertedIndex 
{
public:
	InvertedIndex() = default;
	
	void UpdateDocumentBase(std::vector<std::string> inputDocs);
	
	std::vector <Entry> GetWordCount(const std::string& word);

	std::vector<std::string> GetDocs();

	void SetDocs(std::vector<std::string> docs);

	std::map<std::string, std::vector<Entry>> GetFreqDictionary();

	void SetFreqDictionary(std::map<std::string, std::vector<Entry>>);

private:
	std::vector<std::string> docs; // список содержимого документов

	std::map<std::string, std::vector<Entry>> freqDictionary; // частотный словарь
};