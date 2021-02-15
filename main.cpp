
// Lexical Analyzer
// main.cpp
// 323-02

/* Written By: Kevin Espinoza
               Andres Jaramillo
               Jaime Lambrecht
               Steven Tran
               est_jan -- replace w real name
*/

#include <unordered_map>
#include <iostream>
#include <string>





int main() {

    std::unordered_map<std::string, int> keywords ({ 
        { "int" , 0}, { "float" , 1}, { "bool" , 2}, { "True"    , 3},
        { "False", 4}, { "if", 5}, { "else", 6}, { "then", 7}, { "endif", 8},
        { "endelse", 9}, { "while", 10}, { "whileend", 11}, { "do", 12},
        { "enddo", 13}, { "for", 14}, { "endfor", 15}, { "STDinput", 16},
        { "STDoutput", 17}, { "and", 18}, { "or", 19}, { "not", 20}
    });

    // This is intentionally empty at first. When a new valid identifier is found
    // add it to this container. 
    // NOTE: This may be of no use, if so delete this function from the code.
    std::unordered_map<std::string, int> identifiers;

    std::unordered_map<char, int> separators({
        { '(', 0}, { ')', 1}, { '{', 2}, { '}', 3}, { '[', 4}, { ']', 5}, 
        { ',', 6}, { '.', 7}, { ':', 8}, { ';', 9}
    });

    std::unordered_map<char, int> operators({
        { '*', 0}, { '+', 1}, { '-', 2}, { '=', 3}, { '/', 4}, { '>', 5},
        { '<', 6}, { '%', 7}
    });




    // TODO: Have the program read in line for line from the input file

    // Save the read line into a String and keep track of the position you are at.

    // Check every element from the string. First check if the current element is a space, 
    // if not, check if the element is either a separator or operator. If not, check if 
    // it's a keyword. If not that, then it must be an identifier.

    // We need to make sure we aren't trying to check for an element that's out of bounds
    // to avoid Seg Faults. To do this we must have a check for what our next elements are and if they
    // are within our string's size. 


    /*
        Here is an example of the model we can use. We can change and modify to what we see fit.

        Note: input is our line string which we are iterating. 

        
        for (int i = 0; i < input.size(); ++i) {
            if (input[i] != ' ') {
                if (separators.find(input[i]) != separators.end())
                    // We know its an separator
                else if (operators.find(input[i]) != operators.end())
                    // We know its an operator
                else 
                    // try something else
                    // If its an identifier or keyword, iterate to the next blank space
                    // Ex:  
                    //     if (input[i] != ' ' && i+1 != input.size()) {
                               ++i;  
                           }
                    }
            }
            else 
                // This is blank space, just separate by comma
            
        }
    }

    */





    return 0;
}
