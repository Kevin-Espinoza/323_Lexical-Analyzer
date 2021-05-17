//
// Created by mjlam on 5/16/2021.
//

#include "Lexer.h"

Lexer::Lexer()
{
    this->filename = "sample_input.txt";
}

Lexer::Lexer(std::string filename)
{
    this->filename = filename;
}

std::pair<int, int> Lexer::match_pattern(std::string pattern) {
    std::pair<int, int> category_and_id;

    auto it = tokens.keywords.find(pattern);
    if (it != tokens.keywords.end())
    {
        category_and_id.first = Category::Keywords;
        category_and_id.second = it->second;
    }
    else
    {
        // This has no fail value for undefined identifiers, because we are not there yet.
        auto it = tokens.identifiers.find(pattern);
        category_and_id.first = Category::Identifiers;
        if (it != tokens.identifiers.end())
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

std::pair<int, int> Lexer::match_pattern(char symbol) {
    std::pair<int, int> category_and_id;

    auto it = tokens.separators.find(symbol);
    if (it != tokens.separators.end())
    {
        category_and_id.first = Category::Separators;
        category_and_id.second = it->second;
        // We know its an separator
    }
    else {
        auto it = tokens.operators.find(symbol);
        if (it != tokens.operators.end())
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

bool Lexer::check_if_digit(char current) {
    if (current < 48 || current > 57) {
        return false;
    }
    return true;
}

bool Lexer::check_if_digits(std::string pattern) {
    for (int i = 0; i < pattern.length(); i++) {
        if (pattern[i] < 48 || pattern[i] > 57) {
            return false;
        }
    }
    return true;
}

void Lexer::writeToFile()
{
    // This file is where the tokens and lexemes will be printed
    this->lexer_output.open("lexer_output.txt");

    if (this->lexer_output.is_open()) {
        // Tokens/ Lexemes
        this->lexer_output << "TOKENS" << std::setw(21) << "LEXEMES\n\n";

        std::pair<int, int> category_and_id;
        for (auto lexeme : this->lexemes) {
            category_and_id = lexeme.second;
            if (category_and_id.first >= 0 && category_and_id.first <= 3) {
                this->lexer_output << tokens.categories[category_and_id.first] << "\t=\t";
                // this->lexer_output << "KEYWORD      =    ";
                // this->lexer_output << "EXPRESSION   =    ";
                this->lexer_output << lexeme.first << std::endl;
            } else if (category_and_id.first == Literals) {
                this->lexer_output << tokens.literals[category_and_id.second] << "\t=\t";
                this->lexer_output << lexeme.first << std::endl;
            }
        }
    } else {
        std::cerr << "LEXER::Unable to write to output file";
        exit(1);
    }
    this->lexer_output.close();
}

// This function will contain all the Lexer functions to properly separate all Tokens/Lexemes
std::vector<std::pair<std::string, std::pair<int, int>>> const &Lexer::lexer()
{

    // This is the string that we will be analyzing, it is basically an array of the
    // entire current line in the text
    std::string input;
    std::string pattern;
    bool is_sep_or_opr = false;
    bool is_real = false;

    // This is the file we are reading from
    this->lexer_input.open(filename);

    // Make sure the files successfully open
    if (this->lexer_input.is_open()) {
        std::cout << "LEXER:: Input file opened succesfully. Beginning lexical analysis...\n";

        this->lexer_state = States::PARSER_DEFAULT;
        std::pair<std::string, std::pair<int, int>> token;
        bool backup = false;

        while (std::getline(this->lexer_input, input))
        {
            input.push_back(' ');
            if (this->lexer_input.eof()) {
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
                switch (this->lexer_state) {
                    case States::PARSER_DEFAULT:
                        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n' && input[i] != '\r' && input[i] != '\0')
                        {
                            if (input[i] == '!')
                            {
                                this->lexer_state = States::PARSER_COMMENT;
                            }
                            else
                            {
                                this->lexer_state = States::PARSER_INIT_TOKEN;
                                backup = true;
                            }
                        }
                        break;
                    case States::PARSER_INIT_TOKEN:
                        if (match_pattern(input[i]).first != -1)
                        {
                            // Found a separator or operator before anything else
                            pattern.clear();
                            pattern = input.substr(i, 1);
                            this->lexer_state = States::PARSER_RECORD_TOKEN;
                            is_sep_or_opr = true;
                        }
                        else
                        {
                            // Not a separator or operator. Begin building other token string
                            pattern.clear();
                            this->lexer_state = States::PARSER_BUILD_TOKEN;
                            backup = true;
                        }
                        break;
                    case States::PARSER_BUILD_TOKEN:
                        if (match_pattern(input[i]).first != -1 || input[i] == ' ' || input[i] == '\n') {
                            // current char is a separator, operator, or whitespace (delimiter)
                            // finalize pattern and move to recording state
                            // Make sure to back up (decrement the index) so that we don't skip reading the delimiter
                            if (match_pattern(input[i]).first == Category::Separators && match_pattern(input[i]).second == 7) {
                                if (check_if_digits(pattern)) {
                                    pattern.push_back(input[i]);
                                    this->lexer_state = States::PARSER_BUILD_REAL;
                                } else {
                                    this->lexer_state = States::PARSER_RECORD_TOKEN;
                                    backup = true;
                                }
                            } else {
                                this->lexer_state = States::PARSER_RECORD_TOKEN;
                                backup = true;
                            }
                        }
                        else {
                            pattern.push_back(input[i]);
                        }
                        break;
                    case States::PARSER_BUILD_REAL:
                        is_real = true;
                        if (!check_if_digit(input[i])) {
                            this->lexer_state = States::PARSER_RECORD_TOKEN;
                            backup = true;
                        } else {
                            pattern.push_back(input[i]);
                        }
                        break;
                    case States::PARSER_RECORD_TOKEN:
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

                        this->lexer_state = States::PARSER_DEFAULT;
                        backup = true;
                        break;

                    case States::PARSER_COMMENT:
                        if (input[i] == '!')
                            this->lexer_state = States::PARSER_DEFAULT;
                        break;
                    default:
                        std::cerr << "ERROR: Undefined state in Parser::lexer()";
                        exit(1);
                }
            }
        }

        writeToFile();

        this->lexer_input.close();
    } else {
        std::cerr << "LEXER::Unable to read from input file";
        exit(1);
    }

    return this->lexemes;

}
