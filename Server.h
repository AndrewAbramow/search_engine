#pragma once

#include <iostream>
#include "InvertIndex.h"
#include "JSON.h"

struct WordOccurrence 
{
	std::string word;

	size_t count;
};

struct RelativeIndex 
{
	size_t docId;

	float rank;

	bool operator ==(const RelativeIndex& other) const 
	{
		return (docId == other.docId && rank == other.rank);
	}
};

size_t MaxCount(std::vector<Entry> listOfOccurrences);

std::vector <size_t> Union(std::vector <size_t>& vector1,
	std::vector <size_t>& vector2);

std::vector<RelativeIndex> SingleSearch(std::string request,
	std::map<std::string,
	std::vector<Entry>>&
	freqDictionary);

class SearchServer 
{
public:
	
	SearchServer(InvertedIndex& idx) : _index(idx){};
	
	std::vector<std::vector<RelativeIndex>> search(const
		std::vector<std::string>& queriesInput);

	InvertedIndex GetInvertedIndex();
	
private:
	InvertedIndex _index;
};

std::vector<std::vector<std::pair<int, float>>> ConvertAnswer(
	const std::vector < std::vector <RelativeIndex>> answer);

void WordOccurrenceSort(std::vector<WordOccurrence>& wordOccurrence);

void SortByRelev(std::vector<RelativeIndex>& relevance);

void SortLess(std::vector<RelativeIndex>& relevance, int& begin, int& end);

void SortById(std::vector<RelativeIndex>& relevance);
