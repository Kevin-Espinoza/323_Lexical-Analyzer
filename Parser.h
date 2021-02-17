#include "Tokens.h"
#include <fstream>
#include <iostream>
#include <vector>

class Parser {

private:
    Tokens tokens;
    std::string filename;
	std::ifstream myReader;
public:
    Parser();
    Parser(std::string filname);
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

    // TODO: Have the program read in line for line from the input file

    // Save the read line into a String and keep track of the position you are at.

    // Check every element from the string. First check if the current element is a space, 
    // if not, check if the element is either a separator or operator. If not, check if 
    // it's a keyword. If not that, then it must be an identifier.

    // We need to make sure we aren't trying to check for an element that's out of bounds
    // to avoid Seg Faults. To do this we must have a check for what our next elements are and if they
    // are within our string's size.

void Parser::readFromFile() 
{
	std::string input;
    myReader.open(filename);
	if (myReader.is_open())										//make sure file can be opened
	{
		while (std::getline(myReader, input))		//read to end of file
		{
			// std::cout << lineFromFile << '\n';				//print out each line

            for (int i = 0; i < input.size(); ++i) 
            {
                if (input[i] != ' ') 
                {
                    if (tokens.separators.find(input[i]) != tokens.separators.end())
                    {
                        // We know its an separator
                        std::cout << "Found a separator!";
                    }
                    else if (tokens.operators.find(input[i]) != tokens.operators.end())
                    {
                        // We know its an operator
                        std::cout << "Found an operator!";
                    }    
                    else 
                    {
                        // try something else
                        // If its an identifier or keyword, iterate to the next blank space
                        // Ex:  
                        // if (input[i] != ' ' && i+1 != input.size()) 
                        // {
                        //     ++i;  
                        // }
                    }
                }
                else 
                {

                }
            } 
            // This is blank space, just separate by comma
		myReader.close();										//always close the file
        }
	}

	else std::cerr << "Unable to open file";

}