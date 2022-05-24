# Search Engine

The simple offline search engine for text documents.

## Features

Search Engine - console application,
searchable and configurable via JSON files.

## Tech

Search Engine uses a number of open source projects to work properly:

- [nlohmann JSON](https://github.com/nlohmann/json) - JSON for Modern C++
- [Thread Pool](https://github.com/skprpi/Habr/blob/main/thread_pool/best_version.cpp) - simple thread pool for C++

## Installation

After cloning the repository, Search Engine will additionally need:
- nlohmann library in root directory,
- config.json, requests.json & answers.json in ../out/build directory
- text files to be searched in ../out/build/resources.

## Example

An example of usage is given in search_engine.cpp

