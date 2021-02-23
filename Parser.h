    // TODO: Have the program read in line for line from the input file

    // Save the read line into a String and keep track of the position you are at.

    // Check every element from the string. First check if the current element is a space, 
    // if not, check if the element is either a separator or operator. If not, check if 
    // it's a keyword. If not that, then it must be an identifier.

    // We need to make sure we aren't trying to check for an element that's out of bounds
    // to avoid Seg Faults. To do this we must have a check for what our next elements are and if they
    // are within our string's size.

#include "Tokens.h"
#include "State.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

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

Parser::Parser() 
{
    this->filename = "Lex_test1.txt";
}

Parser::Parser(std::string filename) 
{
    this->filename = filename;
}

/*
std::pair<int, int> Parser::match_pattern(std::string pattern) {
    std::pair<int, int> category_and_id;
    category_and_id.first = this->tokens.keywords.find(pattern)->first;
    if (category_and_id != this->tokens.keywords.end()) {
    }
    if (this->tokens.identifiers.find(pattern) != this->tokens.identifiers.end()) {
        
    }
    return category_and_id;
}
*/

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

    // Make sure the files successfully open
	if (myReader.is_open() && token_separation.is_open())
	{
		while (std::getline(myReader, input))
		{

            int window_start = 0;
            int window_size;
            std::string pattern = "";
            std::pair<int, int> lexeme;

            for (int i = 0; i < input.size(); ++i) 
            {

                window_size = i - window_start + 1;
                if (input[i] != ' ') 
                {
                    if (this->tokens.separators.find(input[i]) != this->tokens.separators.end())
                    {
                        // We know its an separator
                        token_separation << "SEPARATOR    =    " << input[i] << std::endl;
                        found_pattern = true;
                    }
                    else if (this->tokens.operators.find(input[i]) != this->tokens.operators.end())
                    {
                        // We know its an operator
                        token_separation << "OPERATOR     =    " << input[i] << std::endl;
                        found_pattern = true;
                    }    
                    else {
                        // i.e attempt to enforce syntactical rules for keywords, identifiers, etc.
                        // I believe this might be more appropriate to do after collecting lexemes...
                    }
                }
                else 
                {
                    found_pattern = true;
                }
                // std::cout << pattern;
                // lexeme = match_pattern(pattern);

                if (found_pattern) {
                    if (window_size > 1) 
                        pattern = input.substr(window_start, window_size - 1);
                    
                    if (i+1 < input.size())
                        window_start = i+1;

                    found_pattern = false;

                    if (this->tokens.keywords.find(pattern) != this->tokens.keywords.end()) 
                        token_separation << "KEYWORD      =    " << pattern << std::endl;

                    else
                        token_separation << "EXPRESSION   =    " << pattern << std::endl;
                }
            } 
        }
        myReader.close();		
        token_separation.close();
	}

	else std::cerr << "Unable to open file";

}
