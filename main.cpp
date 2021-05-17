
// Lexical Analyzer
// main.cpp
// 323-02

/* Written By: Kevin Espinoza
               Andres Jaramillo
               Jamie Lambrecht
               Steven Tran
               Janelle Estabillo
*/
#include "Parser.h"
#include "Lexer.h"

int main(int argc, char *argv[]) 
{
    // Declare parser as a pointer so that we can choose which constructor to call based on command-line arguments
    Lexer lexer;
    Parser parser;

    if (argc == 1) 
    {
        // Use default file name from default constructor
        lexer = Lexer();
    } 
    else if (argc == 2) 
    {
        // Pass filename from command-line argument to constructor
        std::string filename = argv[1];
        lexer = Lexer(filename);
    } else {
        // Too many arguments from command-line, give user error and exit.
        std::cerr << "Too many arguments!";
        exit(1);
    }
    parser = Parser();

    // Begin lexical analysis

    // Begin syntax analysis
    parser.parse(lexer.lexer());

    return 0;
}
