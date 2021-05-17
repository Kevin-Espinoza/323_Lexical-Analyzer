//
// Created by mjlam on 5/16/2021.
//

#ifndef COMPILERPROJECT_LEXER_H
#define COMPILERPROJECT_LEXER_H
#include "Tokens.h"
#include "States.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

class Lexer {
private:
    std::string filename;
    std::ifstream lexer_input;
    std::ofstream lexer_output;
    int lexer_state;
    Tokens tokens;
    std::vector<std::pair<std::string, std::pair<int, int>>> lexemes;
public:
    Lexer();
    Lexer(std::string filename);
    std::pair<int, int> match_pattern(std::string);
    std::pair<int, int> match_pattern(char);
    bool check_if_digit(char current);
    bool check_if_digits(std::string pattern);
    void writeToFile();
    const std::vector<std::pair<std::string, std::pair<int, int>>> &lexer();
};


#endif //COMPILERPROJECT_LEXER_H
