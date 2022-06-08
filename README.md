# Search Engine

The simple offline search engine for text documents.

## Features

Search Engine - console application,
searchable and configurable via JSON files.

## Tech

Search Engine uses a number of open source projects to work properly:

- [nlohmann JSON](https://github.com/nlohmann/json) - JSON for Modern C++
- [Thread Pool](https://github.com/skprpi/Habr/blob/main/thread_pool/best_version.cpp) - simple thread pool for C++

## Requirements

- C++ 17 compatitable compiler such as Clang or GCC
- CMake

## Installation

- git clone https://github.com/AndrewAbramow/search_engine.git
- cd search_engine
- mkdir build
- cd build
- cmake ..

## Usage

- In config.json, set the names of the files to be searched
- In requests.json, set the searched words
- cd ../search_engine/build/Mingw64-Release
-  ./search_engine.exe
- The answer will be waiting for you in the answers.json
