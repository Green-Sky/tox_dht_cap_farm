cmake_minimum_required(VERSION 3.14...3.24 FATAL_ERROR)

project(dht_tool)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(./external) # before increasing warn levels, sad :(

########################################

add_executable(dht_tool
	./main.cpp
)

target_link_libraries(dht_tool PUBLIC
	toxcore
)

