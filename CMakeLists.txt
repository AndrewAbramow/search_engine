# CMakeList.txt : CMake project for search_engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

project ("search_engine")

set(CMAKE_CXX_STANDARD 17)

# Add json library folder
add_subdirectory (nlohmann_json)
add_subdirectory(tests)

# Add source to this project's executable.
add_executable (search_engine   "src/search_engine.cpp"
								"src/JSON.cpp"
								"src/InvertIndex.cpp"
								"src/Server.cpp"
								"src/ThreadPool.cpp")

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(search_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
