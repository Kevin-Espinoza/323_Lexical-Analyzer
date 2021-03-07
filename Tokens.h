
#include <string>
#include <unordered_map>

// Possible states: Idle
enum Category {
    Separators = 0,
    Operators,
    Keywords,
    Identifers,
    Literals 
} category;

enum Numbers {
    Integer = 0,
    Float,
    String
} numbers;

struct Tokens 
{
    std::string categories[6] = {"SEPARATOR", "OPERATOR", "KEYWORD\t", "IDENTIFIER"};
    std::string literals[2] = {"INTEGER\t", "FLOAT\t"};

    std::unordered_map<std::string, int> keywords = { 
        { "int" , 0}, { "float" , 1}, { "bool" , 2}, { "True"    , 3},
        { "False", 4}, { "if", 5}, { "else", 6}, { "then", 7}, { "endif", 8},
        { "endelse", 9}, { "while", 10}, { "whileend", 11}, { "do", 12},
        { "enddo", 13}, { "for", 14}, { "endfor", 15}, { "STDinput", 16},
        { "STDoutput", 17}, { "and", 18}, { "or", 19}, { "not", 20}
    };

    // This is intentionally empty at first. When a new valid identifier is found
    // add it to this container. 
    // NOTE: This may be of no use, if so delete this function from the code.
    std::unordered_map<std::string, int> identifiers;

    std::unordered_map<char, int> separators = {
        { '(', 0}, { ')', 1}, { '{', 2}, { '}', 3}, { '[', 4}, { ']', 5}, 
        { ',', 6}, { '.', 7}, { ':', 8}, { ';', 9}
    };

    std::unordered_map<char, int> operators = {
        { '*', 0}, { '+', 1}, { '-', 2}, { '=', 3}, { '/', 4}, { '>', 5},
        { '<', 6}, { '%', 7}
    };



};