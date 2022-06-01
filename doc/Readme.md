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

git clone https://github.com/AndrewAbramow/search_engine.git
cd search_engine
cmake -S . -B build
cmake --build build
cmake --build build -t test

## Example

An example of usage is given in search_engine.cpp

