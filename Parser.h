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
    std::vector<std::pair<std::string, std::pair<int, int>>> lexemes;
public:
    Parser();
    Parser(std::string filname);
    std::pair<int, int> match_pattern(std::string);
    std::pair<int, int> match_pattern(char);
    void writeToFile(std::ofstream &token_separation);
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

std::pair<int, int> Parser::match_pattern(std::string pattern) {
    std::pair<int, int> category_and_id;

    // This is a bad copy of the overload for single char separators / operators
    // This should definitely be refactored.
    // if (pattern.length() == 1) {
    //     char symbol = pattern[0];
    //     auto it = tokens.separators.find(symbol);
    //     if (it != this->tokens.separators.end())
    //     {
    //         category_and_id.first = Category::Separators; 
    //         category_and_id.second = it->second; 
    //         // We know its an separator
    //     }
    //     else {
    //         auto it = tokens.separators.find(symbol);
    //         if (it != this->tokens.operators.end())
    //         {
    //             category_and_id.first = Category::Operators; 
    //             category_and_id.second = it->second; 
    //             // We know its an operator
    //         } else {
    //             category_and_id.first = -1;
    //             category_and_id.second = -1;
    //         }
    //     }
    //     return category_and_id;
    // }

    auto it = tokens.keywords.find(pattern);
    if (it != this->tokens.keywords.end()) 
    {
        category_and_id.first = Category::Keywords;
        category_and_id.second = it->second;
    } 
    else 
    {
        // This has no fail value for undefined identifiers, because we are not there yet.
        auto it = this->tokens.identifiers.find(pattern);
        category_and_id.first = Category::Identifers;
        if (it != this->tokens.identifiers.end()) 
        {
            category_and_id.second = it->second;
        } 
        else 
        {
            category_and_id.second = -1;
        }
    }

    return category_and_id;
}

std::pair<int, int> Parser::match_pattern(char symbol) {
    std::pair<int, int> category_and_id;

    auto it = tokens.separators.find(symbol);
    if (it != this->tokens.separators.end())
    {
        category_and_id.first = Category::Separators; 
        category_and_id.second = it->second; 
        // We know its an separator
    }
    else {
        auto it = tokens.operators.find(symbol);
        if (it != this->tokens.operators.end())
        {
            category_and_id.first = Category::Operators; 
            category_and_id.second = it->second; 
            // We know its an operator
        } else {
            category_and_id.first = -1;
            category_and_id.second = -1;
        }
    }
    return category_and_id;
}

void Parser::writeToFile(std::ofstream &token_separation)
{
    std::pair<int, int> category_and_id;
    for (auto lexeme : this->lexemes) {
        category_and_id = lexeme.second;
        if (category_and_id.first >= 0 && category_and_id.first <= 3) {
            token_separation << this->tokens.categories[category_and_id.first] << "\t=\t";
            // token_separation << "KEYWORD      =    ";
            // token_separation << "EXPRESSION   =    "; 
            token_separation << lexeme.first << std::endl;
        }
    }
}

void Parser::readFromFile() 
{
    // This file is where the tokens and lexemes will be printed
    std::ofstream token_separation;
    token_separation.open("token_separation.txt");
    
    // Tokens/ Lexemes
    token_separation << "TOKENS" << std::setw(21) << "LEXEMES\n\n";
    
    // This is the string that we will be analyzing, it is basically an array of the
    // entire current line in the text
	std::string input;
	std::string pattern;

    // This is the file we are reading from
    myReader.open(filename);

    // Make sure the files successfully open
	if (myReader.is_open() && token_separation.is_open())
	{
        parser_state = PARSER_DEFAULT;
        std::pair<std::string, std::pair<int, int>> token;
        bool backup = false;

		while (std::getline(myReader, input))
		{
            backup = false;
            for (int i = 0; i < input.size(); ++i) 
            {
                if (backup) {
                    i--;
                    backup = false;
                }
                switch (parser_state) {
                    case PARSER_DEFAULT:
                        if (input[i] != ' ' && input[i] != '\t') 
                        {
                            if (input[i] == '!') 
                            {
                                parser_state = PARSER_COMMENT;
                                std::cout << "DEBUG: Began comment.\n";
                            } 
                            else
                            {
                                parser_state = PARSER_INIT_TOKEN;
                                backup = true;
                            }
                        }
                        break;
                    case PARSER_INIT_TOKEN:
                        if (match_pattern(input[i]).first != -1) 
                        {
                            // Found a separator or operator before anything else
                            std::cout << "Found lone sep/opr\n";
                            pattern = input.substr(i, 1);
                            parser_state = PARSER_RECORD_TOKEN;
                        }
                        else 
                        {
                            // Not a separator or operator. Begin building other token string
                            pattern.clear(); 
                            parser_state = PARSER_BUILD_TOKEN;
                            backup = true;
                        }
                        break;
                    case PARSER_BUILD_TOKEN:
                        if (match_pattern(input[i]).first != -1 || input[i] == ' ') {
                            // current char is a separator, operator, or whitespace (delimiter)
                            // finalize pattern and move to recording state
                            // Make sure to back up (decrement the index) so that we don't skip reading the delimiter 
                            parser_state = PARSER_RECORD_TOKEN;
                            backup = true;
                        } else {
                            pattern.push_back(input[i]);
                        }
                        break;
                    case PARSER_RECORD_TOKEN:
                        // This is a bad, band-aid solution. Remove this if and fix match_pattern
                        if (pattern.length() == 1) {
                            token.first = pattern;
                            token.second = match_pattern(pattern[0]);
                        }
                        else
                        {
                            token.first = pattern;
                            token.second = match_pattern(pattern);
                        }
                        this->lexemes.push_back(token);

                        parser_state = PARSER_DEFAULT;
                        backup = true;
                        break;
                    case PARSER_COMMENT:
                        if (input[i] == '!')
                            parser_state = PARSER_DEFAULT;
                        break;
                    default:
                        std::cerr << "ERROR: Undefined state in Parser::readFromFile()";
                        exit(1);
                }
            } 
        }

        writeToFile(token_separation);

        myReader.close();		
        token_separation.close();
	}

	else std::cerr << "Unable to open file";

}
