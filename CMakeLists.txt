cmake_minimum_required(VERSION 3.10)

project(MidiParser LANGUAGES CXX)

add_library(MidiParser INTERFACE)

target_include_directories(MidiParser INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)