// search_engine.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include <vector>
#include "..\include\search_engine.h"
#include "..\include\JSON.h"
#include "..\include\InvertIndex.h"
#include "..\include\Server.h"

int main()
{
	//  JSON part:
	ConverterJSON newJSON;

	//  list of requests
	for (auto& el : newJSON.GetRequests()) 
	{
		std::cout << el << std::endl;
	}

	//  response limit
	std::cout << "Response limit is: " <<
		newJSON.GetResponsesLimit() << std::endl;
//________________________________________________________________

	//  Inverted index part:
	//  (pointer for SearchServer constructor)
	InvertedIndex newInvInd;

	//  update documents
	newInvInd.UpdateDocumentBase(newJSON.GetTextDocuments());
//________________________________________________________________

	//  Search server part:
	SearchServer server(newInvInd);

	auto requests = newJSON.GetRequests();

	//search
	auto answers = ConvertAnswer(server.search(requests));

	newJSON.PutAnswers(answers);
}
