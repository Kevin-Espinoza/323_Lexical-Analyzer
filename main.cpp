
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
    Parser *parser; 
    if (argc == 1) 
    {
        parser = new Parser();
        parser->readFromFile();
    } 
    else if (argc == 2) 
    {
        std::string filename = argv[1];
        parser = new Parser(filename);
        parser->readFromFile();
    } else {
        std::cerr << "Too many arguments!";
    }

    return 0;
}
