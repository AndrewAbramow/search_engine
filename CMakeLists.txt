# CMakeList.txt : CMake project for search_engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

project ("search_engine")

set(JSON_BuidTests OFF CACHE INTERNAL "")
# Add Google Test library
include(FetchContent)
FetchContent_Declare(
googletest
URL
https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 17)
FetchContent_MakeAvailable(googletest)
enable_testing()
# Add json library folder
add_subdirectory (nlohmann_json)
add_compile_definitions(TESTS)
# Add source to this project's executable.
add_executable (search_engine   "search_engine.cpp"
								"Tests.cpp"				
								"JSON.cpp"
								"InvertIndex.cpp"
								"Server.cpp"
								"ThreadPool.cpp")
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json gtest_main)
include(GoogleTest)
gtest_discover_tests(search_engine)

# TODO: Add tests and install targets if needed.
