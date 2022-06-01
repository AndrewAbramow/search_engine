#pragma once

#include <iostream>
#include <vector>
#include "Server.h"


class ConverterJSON 
{
public:
	ConverterJSON() = default;
	
	std::vector<std::string> GetTextDocuments();

	int GetResponsesLimit();
	
	std::vector<std::string> GetRequests();
	
	void PutAnswers(std::vector<std::vector<std::pair<int, float>>>);
};

std::string RequestNumber(int n);

double ToTwoDecimalPlaces(float f);