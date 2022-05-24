#pragma once

#include <iostream>
#include <vector>
#include <map>

struct Entry 
{
	size_t docId, count;
	
	// ������ �������� ��������� ��� ���������� �������� ���������
	bool operator ==(const Entry& other) const 
	{
		return (docId == other.docId &&
			count == other.count);
	}
};

std::vector<std::string> ParseDocs(std::string line);

size_t Count(std::string word, std::string line);

void Indexing(size_t textId, std::vector<std::string> docs,
	std::map<std::string, std::vector<Entry>>& freqDictionary);

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
	std::vector<std::string> docs; // ������ ����������� ����������

	std::map<std::string, std::vector<Entry>> freqDictionary; // ��������� �������
};