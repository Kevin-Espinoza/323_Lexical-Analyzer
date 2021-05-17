#ifndef COMPILERPROJECT_PARSER_H
#define COMPILERPROJECT_PARSER_H

#include "Tokens.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <stack>

enum Instructions {
    POPM = 0,
    PUSHM,
    PUSHI,
    POPI,
    ADD,
    SUB,
    MULT,
    DIV,
    LES,
    GRT,
    EQU,
    LEQ,
    GEQ,
    JUMPZ,
    LABEL,
    JUMP,
    STDOUT,
    STDIN
};


enum Types {
    INTEGER = 0, FLOAT, BOOL
};

struct Instruction {
    int address = 0;
    std::string operation = "";
    std::string operand = "";
};

class Parser {

private:
    std::string instruction_strings[18] = {
            "POPM\t", "PUSHM\t", "PUSHI\t", "POPI\t",
            "ADD\t\t", "SUB\t\t", "MULT\t", "DIV\t\t",
            "LES\t\t", "GRT\t\t", "EQU\t\t",
            "LEQ\t\t", "GEQ\t\t",
            "JUMPZ\t", "LABEL\t", "JUMP\t",
            "STDOUT\t", "STDIN\t"};
    std::string type_names[3] = {"integer", "float", "bool"};
    std::vector<std::string> syntax;    // Vector will be filled in parse() - contains syntactical info for cout
    const std::vector<std::pair<std::string, std::pair<int, int>>> *lexemes;
    std::vector<std::pair<std::string, std::pair<int, int>>>::const_iterator current;
    bool printFlag;
    std::ofstream parser_output;
    std::map<std::string, std::pair<int, int>> symbol_table;
    int next_avail_memloc = 5000;
    std::vector<std::string> intermediate_code;
    std::vector<Instruction> instr_table;
    int instr_address = 1;
    std::stack<int> jump_stack;
    bool if_flag = false;
    bool while_flag = false;
    bool increment_flag = false;
    bool else_flag = false;
    bool orequal_flag = false;
    Tokens tokens;

public:
    Parser();
    void genInstr(int, int);
    void genInstr(int);
    int getAddress(std::string);
    int getType(std::string);
    void back_patch(int);
    void parse(const std::vector<std::pair<std::string, std::pair<int, int>>> &);
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
    std::vector<std::pair<std::string, std::pair<int, int>>>::const_iterator getNext();
    void increment(int n);
    void printLexeme();
    void printRule(int i);
    void printInstructions();
    void printSymbolTable();
};
#endif //COMPILERPROJECT_PARSER_H
