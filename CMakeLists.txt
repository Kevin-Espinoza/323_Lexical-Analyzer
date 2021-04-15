cmake_minimum_required(VERSION 3.16)
project(323_Lexical_Analyzer)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(323_Lexical_Analyzer
        main.cpp
        Parser.h
        State.h
        stdafx.h
        Tokens.h)
