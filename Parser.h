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
    bool check_if_digit(char current);
    bool check_if_digits(std::string pattern);
    void writeToFile(std::ofstream &token_separation);
    void lexer();
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
            if (check_if_digits(pattern)) {
                category_and_id.first = Category::Literals;
                category_and_id.second = Numbers::Integer;
            } else {
                category_and_id.second = -1;
            }
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
    
bool Parser::check_if_digit(char current) {
    if (current < 48 || current > 57) {
        return false;
    }
    return true;
}

bool Parser::check_if_digits(std::string pattern) {
    for (int i = 0; i < pattern.length(); i++) {
        if (pattern[i] < 48 || pattern[i] > 57) {
            return false;
        }
    }
    return true;
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
        } else if (category_and_id.first == Literals) {
            token_separation << this->tokens.literals[category_and_id.second] << "\t=\t";
            token_separation << lexeme.first << std::endl;
        }
    }
}

void Parser::lexer() 
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
    bool is_sep_or_opr = false;
    bool is_real = false;
    bool done = false;

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
            if (myReader.eof()) {
                input.push_back('\n');
            }
            for (int i = 0; i < input.size(); ++i) 
            {
                if (backup) {
                    if (i > 0) {
                        i--;
                    }
                    backup = false;
                }
                switch (parser_state) {
                    case PARSER_DEFAULT:
                        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n') 
                        {
                            if (input[i] == '!') 
                            {
                                parser_state = PARSER_COMMENT;
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
                            pattern.clear(); 
                            pattern = input.substr(i, 1);
                            parser_state = PARSER_RECORD_TOKEN;
                            is_sep_or_opr = true;
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
                        if (match_pattern(input[i]).first != -1 || input[i] == ' ' || input[i] == '\n') {
                            // current char is a separator, operator, or whitespace (delimiter)
                            // finalize pattern and move to recording state
                            // Make sure to back up (decrement the index) so that we don't skip reading the delimiter 
                            if (match_pattern(input[i]).first == Category::Separators && match_pattern(input[i]).second == 7) {
                                if (check_if_digits(pattern)) {
                                    pattern.push_back(input[i]);
                                    parser_state = PARSER_BUILD_REAL;
                                } else {
                                    parser_state = PARSER_RECORD_TOKEN;
                                    backup = true;
                                }
                            } else {
                                parser_state = PARSER_RECORD_TOKEN;
                                backup = true;
                            }
                        }
                        else { 
                            pattern.push_back(input[i]);
                        }
                        break;
                    case PARSER_BUILD_REAL:
                        is_real = true;
                        if (!check_if_digit(input[i])) {
                            parser_state = PARSER_RECORD_TOKEN;
                            backup = true;
                        } else {
                            pattern.push_back(input[i]);
                        }
                        break;
                    case PARSER_RECORD_TOKEN:
                        // This is a bad, band-aid solution[?]. Remove this if and fix match_pattern???
                        token.first = pattern;
                        if (is_sep_or_opr) {
                            token.second = match_pattern(pattern[0]);
                            is_sep_or_opr = false;
                        }
                        else if (is_real) {
                            std::pair<int, int> category_and_id;
                            category_and_id.first = Literals;
                            category_and_id.second = Float;
                            token.second = category_and_id;
                            is_real = false;
                        }
                        else
                        {
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
                        std::cerr << "ERROR: Undefined state in Parser::lexer()";
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
