#pragma once

//
// file: Parser.h
// name:
// date: 21 February 2021
//
// This data structure reads from the input file and writes to
// token_separation.txt. It applies the function match_string 
// to identify tokens in the input string
//
#include "Token_buffer.h"
#include "Tokens.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

class Parser {

private:
    Tokens tokens;
    std::string filename;
    std::ifstream myReader;
    std::vector<std::pair<int, int>> lexemes;
public:
    Parser();
    Parser(std::string filname);
    std::pair<int, int> match_pattern(std::string);
    void readFromFile();
};

// default constructor
Parser::Parser()
{
    this->filename = "Lex_test3.txt";
}

// input file specific constructor
Parser::Parser(std::string filename)
{
    this->filename = filename;
}

// this function extracts either an identifier or keyword
// potential pattern is the string parameter
// pattern has been stripped of leading symbols, seperators and whitespace
std::pair<int, int> Parser::match_pattern(std::string pattern) {
    std::pair<int, int> category_and_id;                  // needed adjustment
  /*  category_and_id.first = this->tokens.keywords.find(pattern)->first;
    if (category_and_id != this->tokens.keywords.end()) {
    }
    if (this->tokens.identifiers.find(pattern) != this->tokens.identifiers.end()) {

    } */

    token_buffer tk;

    return category_and_id;
}

// TODO: Have the program read in line for line from the input file

// Save the read line into a String and keep track of the position you are at.

// Check every element from the string. First check if the current element is a space, 
// if not, check if the element is either a separator or operator. If not, check if 
// it's a keyword. If not that, then it must be an identifier.

// We need to make sure we aren't trying to check for an element that's out of bounds
// to avoid Seg Faults. To do this we must have a check for what our next elements are and if they
// are within our string's size.


// this function reads from the file and matches strings in an input line
//
void Parser::readFromFile()
{
    bool found_pattern = false;

    // This file is where the tokens and lexemes will be printed
    std::ofstream token_separation;
    token_separation.open("token_separation.txt");

    // Tokens/ Lexemes
    token_separation << "TOKENS" << std::setw(21) << "LEXEMES\n\n";

    // This is the string that we will be analyzing, it is basically an array of the
    // entire current line in the text
    std::string input;

    // This is the file we are reading from
    myReader.open(filename);

    // make sure input and output files are open...
    if (myReader.is_open() && token_separation.is_open())
    {
        while (std::getline(myReader, input))		//read to end of file, one line at a time
        {
            // std::cout << lineFromFile << '\n';		//print out each line...debug?

            // not sure what these are for yet...
            int window_start = 0;
            int window_size;
            std::string pattern = "";
            std::pair<int, int> lexeme;

            // until the end of the current input line
            for (int i = 0; i < input.size(); ++i)
            {
                // reset found_pattern; suppose not
                found_pattern = false;

                window_size = i - window_start + 1;

                // this is the section that negotiates the identification of 
                    // a token from the list of possible tokens.
                // New tokens are added within the if-else if network else is reserved
                // syntax errors.

                    // TODO: Identify if the first element starts with a '!', if so this is a 
                    //       comment and should be ignored by the program
                if (input[0] == '!') {
                    // go to the next line in the text
            // by advancing the read head?
                    token_separation << "COMMENT    =    " << input << std::endl;
                    i = input.size() - 1;
                    found_pattern = true;
                }
                // the current character under the read head is not a white space
                else if (input[i] != ' ' && input[i] != '\t')
                {

                    // Do we know its an separator
                    if (this->tokens.separators.find(input[i]) != this->tokens.separators.end())
                    {
                        token_separation << "SEPARATOR    =    " << input[i] << std::endl;
                        found_pattern = true;
                    }
                    // Do we know its an operator
                    else if (this->tokens.operators.find(input[i]) != this->tokens.operators.end())
                    {
                        token_separation << "OPERATOR     =    " << input[i] << std::endl;
                        found_pattern = true;
                    }
                    // is this not one of the single character token types that are being extracted
                    else {

                        // build pattern
                        std::pair<int, int> pattern = match_pattern(input.substr(i));
                        token_separation << "debug line 148 of parser.h: this is the integer pair representeing the patter we matched\n";
                        token_separation << std::get<0>(pattern) << " " << std::get<1>(pattern) << std::endl;

                        // do we know it is a keyword or an expression


            //
                        /*
             * if (this->tokens.keywords.find(input[i]) != this->tokens.keywords.end())
                    {
                            // std::cout << pattern;
                            // lexeme = match_pattern(pattern);
                //
                            // this is where I left off....
                            token_separation << "KEYWORD      =    " << pattern << std::endl;
                found_pattern = true;
                        }
            else
                {
                            token_separation << "EXPRESSION   =    " << pattern << std::endl;
                found_pattern = true;
            }
                        // try something else
                        // If its an identifier or keyword, iterate to the next blank space
                        // Ex:
                        // if (input[i] != ' ' && i+1 != input.size())
                        // {
                        //     ++i;
                        // }
                        // i.e attempt to enforce syntactical rules for keywords, identifiers, etc.
                        // I believe this might be more appropriate to do after collecting lexemes...

            // already assumed that there was no pattern
                        //found_pattern = false;
            //
            */
                        ;
                    }
                }
                // it is a white space
                        //else 
                        //{
                //    // already assumed that there is not a pattern...
                //	found_pattern = false;
                        //}
            } // for loop

        } // while loop
        //always close the file
        myReader.close();
        token_separation.close();
    }
    else {
        std::cerr << "Unable to open file";
    }
}

