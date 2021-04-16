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
    std::vector<std::string> syntax;    // Vector will be filled in parse() - contains syntactical info for cout 
    std::vector<std::pair<std::string, std::pair<int, int>>> lexemes;
    std::vector<std::pair<std::string, std::pair<int, int>>>::iterator current;
    bool printFlag;
    std::ofstream parser_output;

public:
    Parser();
    Parser(std::string filename);
    std::pair<int, int> match_pattern(std::string);
    std::pair<int, int> match_pattern(char);
    bool check_if_digit(char current);
    bool check_if_digits(std::string pattern);
    void writeToFile(std::ofstream &token_separation);
    void lexer();
    void parse();
    void StatementList();
    void Statement();
    void Conditional();
    void Expression();
    void Assignment();
    void ExpressionPrime();
    void Term();
    void TermPrime();
    void Factor();
    void Declarative();
    void Num();
    std::vector<std::pair<std::string, std::pair<int, int>>>::iterator getNext();
    void increment(int n);
    void printLexeme();
    void printRule(int i);
};

Parser::Parser() 
{
    this->filename = "sample_input.txt";
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
        category_and_id.first = Category::Identifiers;
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

// This function will contain all the Lexer functions to properly separate all Tokens/Lexemes
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
                        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n' && input[i] != '\r' && input[i] != '\0') 
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

void Parser::parse()
{
    this->parser_output.open("parser_output.txt");

    // Fill the syntax vector with proper output terms
    // NOTE: syntax was declared empty to keep code clean
    // Reference for proper output for tokens
    /*syntax[0]*/ this->syntax.push_back("\t<Expression> -> <Expression> + <Term> | <Expression> - <Term> | <Term>");
    /*syntax[1]*/ this->syntax.push_back("\t<Term> -> <Term> * <Factor> | <Term> / <Factor> | <Factor>");
    /*syntax[2]*/ this->syntax.push_back("\t<Factor> -> ( <Expression> ) | <ID> | <Num>");
    /*syntax[3]*/ this->syntax.push_back("\t<ID> -> id");
    /*syntax[4]*/ this->syntax.push_back("\t<Statement> -> <Assign>");
    /*syntax[5]*/ this->syntax.push_back("\t<Assign> -> <ID> = <Expression>;");
    /*syntax[6]*/ this->syntax.push_back("\t<Statement> -> <Declarative>");
    /*syntax[7]*/ this->syntax.push_back("\t<Declarative> -> <Type> <ID>");
    /*syntax[8]*/ this->syntax.push_back("\t<Type> -> bool | float | int");
    /*syntax[9]*/ this->syntax.push_back("\t<Conditional> -> <Expression> <Relop> <Expression> | <Expression>");
    /*syntax[10]*/this->syntax.push_back("Began code block\n");
    /*syntax[11]*/this->syntax.push_back("<RELOP>\n");


    // Begin parse()
    this->current = lexemes.begin();
    //current->second is category_and_id (second.first is category, second.second is id)
    this->StatementList();

}

void Parser::StatementList() {
    while (this->current != this->lexemes.end()) {
        this->Statement();
        this->increment(1);
    }

}

void Parser::Statement() {
    while (this->current->first != "}" && this->current->first != ";") {
        if (this->getNext()->first == "=") {
            this->printLexeme();
            this->printRule(4);
            this->Assignment();
        } else if (this->current->second.first == Category::Keywords && this->current->second.second <= 2){
            this->printLexeme();
            this->printRule(6);
            this->Declarative();
        } else if (this->current->first == "(")
        {
            this->Expression();
        } else if (this->current->first == ")") {
            this->increment(1);
        } else if (this->current->first == "if" || this->current->first == "while") {
            // print the if/while
            this->printLexeme();
            this->increment(1);
            this->Conditional();
            if (this->current->first == "{") {
                this->printLexeme();
                this->printRule(10);
                this->increment(1);
            } else {
                std::cerr << "Expected {\n";
                exit(1);
            }
        }
    }
//    this->increment(1);
    this->printLexeme();
}

void Parser::Conditional() {
    this->printRule(9);
    // check for opening parenthesis
    if (this->current->first == "(") {
        this->printLexeme();
        this->increment(1);
    }
    this->Expression();
    this->increment(1);
    // check for relational operator
    if (this->current->first == "<" || this->current->first == ">") {
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        if (this->current->first == "=") {
            this->printLexeme();
            this->printRule(11);
            this->increment(1);
        }
        this->Expression();
        this->increment(1);
    } else if (this->current->first == "=" && this->getNext()->first == "=") {
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        this->Expression();
        this->increment(1);
    }
    // check for closing parenthesis
    if (this->current->first == ")") {
        this->printLexeme();
        this->increment(1);
    } else {
        std::cerr << "Expected closing parenthesis\n";
        exit(1);
    }
}

    void Parser::Declarative() {
        this->printRule(7);
        this->printRule(8);
    if (this->getNext()->second.first == Category::Identifiers) {
        this->increment(1);
        this->printLexeme();
        if (this->getNext()->first != "=") {
            this->increment(1);
        }
    } else {
        std::cerr << "Expected valid identifier\n";
    }

}

void Parser::Assignment() {
    this->printRule(5);
    if (this->current->second.first == Category::Identifiers) {
        this->increment(1);
        this->printLexeme();
        this->increment(1);
        if (this->getNext()->first == ";") {
            this->Expression();
            this->increment(1);
        } else {
            this->Expression();
        }

    } else {
        std::cerr << "Error:: Assignment must begin with identifier.\n";
        exit(1);
    }

}

void Parser::Expression() {
    this->printLexeme();
    this->printRule(0);
    this->Term();
    if (this->getNext()->first == "+" || this->getNext()->first == "-") {
        this->increment(1);
        this->ExpressionPrime();
    }
}

void Parser::ExpressionPrime() {

    if (this->current->first == "+" || this->current->first == "-") {
        this->printLexeme();
        this->increment(1);
        this->printLexeme();
        this->Term();
        this->increment(1);
        this->ExpressionPrime();
    }
}

void Parser::Term() {
//    this->printLexeme();
    this->printRule(1);
    this->Factor();
    if (this->getNext()->first == "*" || this->getNext()->first == "/") {
        this->increment(1);
        this->TermPrime();
    }
}

void Parser::TermPrime() {

    if (this->current->first == "*" || this->current->first == "/") {
        this->printLexeme();
        this->increment(1);
        this->printLexeme();
        this->Factor();
        this->increment(1);
        this->TermPrime();
    }
}


//    /*syntax[2]*/ this->syntax.push_back("\t<Factor> -> ( <Expression> ) | <ID> | <Num>");
void Parser::Factor() {
        // this->printLexeme();
        this->printRule(2);
        if (this->current->second.first == Category::Identifiers)
        {
            this->printRule(3);
        } else if (this->current->second.first == Category::Literals) {
            if (this->current->second.second == Numbers::Integer || this->current->second.second == Numbers::Float) {

            } else {
                std::cerr << "Invalid numerical factor\n";
            }
        }
        else if (this->current->first == "(") {
            this->increment(1);
            this->Expression();
            if (this->current->first != ")") {
                std::cerr << "Expected \")\"\n";
                exit(1);
            } else {
                this->printLexeme();
            }
        } else {
            std::cerr << "Expected Identifier, number or parenthetical expression.\n";
            exit(1);
        }
}

void Parser::Num() {
    if (this->current->second.first == Category::Literals &&
    (this->current->second.second == Numbers::Integer || this->current->second.second == Numbers::Float)) {

    } else {
        std::cerr << "Invalid number.\n";
    }
}

std::vector<std::pair<std::string, std::pair<int, int>>>::iterator Parser::getNext()
{
    if (this->current != this->lexemes.end()) {
        return std::next(this->current);
    }
    else {
        return this->lexemes.end();
    }

}

void Parser::increment(int n) {
    this->printFlag = false;
    while (n > 0) {
        this->current = std::next(this->current);
        n--;
    }
    while (n < 0) {
        this->current = std::next(this->current);
        n++;
    }
}

void Parser::printRule(int i) {
    std::cout << syntax[i] << '\n';
    this->parser_output << syntax[i] << '\n';
}

void Parser::printLexeme() {
    if (!this->printFlag) {
        std::cout << "\nToken:\t" << tokens.categories[this->current->second.first] << "\tLexeme:\t" << this->current->first << '\n';
        this->parser_output << "\nToken:\t" << tokens.categories[this->current->second.first] << "\tLexeme:\t" << this->current->first << '\n';
        this->printFlag = true;
    }
}