
// Lexical Analyzer
// main.cpp
// 323-02

/* Written By: Kevin Espinoza
               Andres Jaramillo
               Jaime Lambrecht
               Steven Tran
               est_jan -- replace w real name
*/
#include "Parser.h"

int main(int argc, char *argv[]) 
{
    // Declare parser as a pointer so that we can choose which constructor to call based on command-line arguments
    Parser *parser; 

    if (argc == 1) 
    {
        // Use default file name from default constructor
        parser = new Parser();
    } 
    else if (argc == 2) 
    {
        // Pass filename from command-line argument to constructor
        std::string filename = argv[1];
        parser = new Parser(filename);
    } else {
        // Too many arguments from command-line, give user error and exit.
        std::cerr << "Too many arguments!";
        exit(1);
    }

    // Begin lexical analysis
    parser->readFromFile();

    return 0;
}
