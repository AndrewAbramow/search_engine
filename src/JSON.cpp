#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "JSON.h"
#include "nlohmann/json.hpp"

std::vector <std::string> ConverterJSON::GetTextDocuments() {

	// open config file
	std::ifstream config("..\\resources\\config.json");  

	nlohmann::json jObject;  // create new json object

	std::vector<std::string> textDocuments;

	//  read docs paths from config
	if (config.is_open())
	{
		config >> jObject;  // parse configs to json object

		if (!jObject.is_null())
		{

			// check engine name
			std::cout << "Engine name: " << jObject["config"]["name"] << std::endl

				// check engine version
				<< "Version: " << jObject["config"]["version"] << std::endl

				// check max responses
				<< "Max responses: " << jObject["config"]["max_responses"] << std::endl;

			for (auto& el : jObject["files"])
			{
				// collect texts from request in vector
				std::string contents;

				std::ifstream request(el);

				contents.assign((std::istreambuf_iterator<char>(request)),
					(std::istreambuf_iterator<char>()));

				// all letters are lowercase
				std::transform(contents.begin(),
							   contents.end(), 
							   contents.begin(), 
							   ::tolower);

				textDocuments.push_back(contents);

				request.close();
			}
			config.close();
		}
		else 
		{
			std::cerr << " config file is empty" << std::endl;
		}
	}
	else 
	{
		std::cerr << " config file is missing" << std::endl;
	}

	return textDocuments;
}

int ConverterJSON::GetResponsesLimit() 
{
	// open config file
	std::ifstream config("..\\resources\\config.json");  

	nlohmann::json jObject;  // create new json object

	config >> jObject;  // parse configs to json object

	int maxResponses{ 0 };

	if (config.is_open())
	{
		if (!jObject.is_null()) 
		{
			maxResponses = jObject["config"]["max_responses"];
		}
		else 
		{
			std::cerr << " config file is empty" << std::endl;
		}
		config.close();
	}
	else 
	{
		std::cerr << " config file is missing" << std::endl;
	}

	return maxResponses;
}

std::vector<std::string> ConverterJSON::GetRequests() 
{
	// open requests file
	std::ifstream request("..\\resources\\requests.json");  

	nlohmann::json jObject;  // create new json object

	std::vector<std::string> content;

	if (request.is_open())
	{
		request >> jObject;  // parse configs to json object

		if (!jObject.is_null()) {

			for (auto& el : jObject["requests"])
			{
				// collect requests in vector
				content.push_back(el);
			}
		}
		else 
		{
			std::cerr << " config file is empty" << std::endl;
		}
		request.close();
	}
	else 
	{
		std::cerr << " config file is missing" << std::endl;
	}
	

	return content;
}

std::string RequestNumber(int n) 
{
	std::string numb = std::to_string(n);

	while (numb.size() != 3)
	{
		numb = '0' + numb;
	}

	return numb;
}

double ToTwoDecimalPlaces(float f) 
{
	int i;

	if (f >= 0)
	{
		i = static_cast<int>(f * 100 + 0.5);
	}
	else
	{
		i = static_cast<int>(f * 100 - 0.5);
	}

	return (i / 100.0);
}

void ConverterJSON::PutAnswers(std::vector<std::vector<std::pair<int, float>>>
	results) 
{
	std::ofstream answer("..\\resources\\answers.json");

	nlohmann::json jObject;

	int counter = 0;

	// parse input vector
	for (auto& El : results)
	{
		/*std::cout << counter + 1 << " answer write\n";*/

		nlohmann::json jRequest;

		if (El.size() > 0) 
		{
			jRequest["result"] = true;

			nlohmann::json docidRank;

			int responsesLimit{ 0 };

			if (El.size() > GetResponsesLimit())
			{
				responsesLimit = GetResponsesLimit();
			}
			else
			{
				responsesLimit = El.size();
			}

			for (int i = 0; i < responsesLimit; i++)
			{
				nlohmann::json pair;

				pair["docid"] = El[i].first;

				pair["rank"] = ToTwoDecimalPlaces(El[i].second);

				docidRank.push_back(pair);
			}
			jRequest["relevance"] = docidRank;

			counter++;

			jObject["request" + RequestNumber(counter)] = jRequest;
		}
		else 
		{
			jRequest["result"] = false;

			counter++;

			jObject["request" + RequestNumber(counter)] = jRequest;
		}
	}
	// collect all intermediate json objects
	nlohmann::json output;

	output["answers"] = jObject;

	if (answer.is_open())
	{
		answer << std::setw(4) << output;
	}
	answer.close();
}