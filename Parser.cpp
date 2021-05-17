
#include "Parser.h"

Parser::Parser() {

}

int Parser::getAddress(std::string id) {
    int address;
    auto symbol = this->symbol_table.find(id);
    if (symbol != this->symbol_table.end()) {
        address = symbol->second.first;
    } else {
        std::cerr << "ERROR::Undeclared identifier";
        exit(1);
    }
    return address;
}

void Parser::genInstr(int instr_type, int val) {
    Instruction instruction;
    instruction.operation = this->instruction_strings[instr_type];
    std::string operand = "";
    operand = std::to_string(val);
    instruction.operand = operand;
    instruction.address = this->instr_address++;
    this->instr_table.push_back(instruction);

}

void Parser::genInstr(int instr_type) {
    Instruction instruction;
    instruction.operation = this->instruction_strings[instr_type];
    instruction.operand = "nil";
    instruction.address = this->instr_address++;
    this->instr_table.push_back(instruction);

}

int Parser::getType(std::string str) {
    int type;
    if (str == "int") {
        type = Types::INTEGER;
    } else if (str == "float") {
        type = Types::FLOAT;
    } else if (str == "bool") {
        type = Types::BOOL;
    } else {
        std::cerr << "Error::Invalid type.\n";
        exit(1);
    }
    return type;
}

void Parser::back_patch(int jump_addr) {
    int address = this->jump_stack.top();
    this->jump_stack.pop();
    this->instr_table[address - 1].operand = std::to_string(jump_addr);
}



void Parser::parse(const std::vector<std::pair<std::string, std::pair<int, int>>> &lexemes)
{
    this->lexemes = &lexemes;
    this->parser_output.open("parser_output.txt");
    if (!this->parser_output.is_open()) {
        std::cerr << "PARSER:: Could not open output file.\n";
        exit(1);
    }

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
    this->current = this->lexemes->begin();
    //current->second is category_and_id (second.first is category, second.second is id)
    this->StatementList();
    this->printInstructions();
    this->printSymbolTable();
}

void Parser::StatementList() {
    while (this->current != this->lexemes->end()) {
        this->Statement();
        if (this->current != this->lexemes->end()) {
            this->increment(1);
        }
    }

}

void Parser::Statement() {
    while (this->current->first != "}" && this->current->first != ";") {
        if (this->getNext()->first == "=") {
            this->printLexeme();
            this->printRule(4);
            this->Assignment();
        } else if (this->current->second.first == Category::Keywords && this->current->second.second <= 2) {
            this->printLexeme();
            this->printRule(6);
            this->Declarative();
        } else if (this->current->first == "STDoutput") {
            this->printLexeme();
            this->increment(1);
            if (this->current->first == "(") {
                this->increment_flag = true;
                this->Expression();
                this->increment_flag = false;
            } else {
                std::cerr << "Error: Expected (";
                exit(1);
            }
            this->genInstr(Instructions::STDOUT);
        } else if (this->current->first == "STDinput") {
            this->genInstr(Instructions::STDIN);
            this->printLexeme();
            this->increment(1);
            if (this->current->first == "(") {
                this->increment_flag = true;
                this->Expression();
                this->increment_flag = false;
            } else {
                std::cerr << "Error: Expected (";
                exit(1);
            }
        } else if (this->current->first == "(") {
            this->Expression();
        } else if (this->current->first == ")") {
            this->increment(1);
        } else if (this->current->first == "if" || this->current->first == "while") {
            if (this->current->first == "while") {
                this->jump_stack.push(this->instr_address);
                this->genInstr(Instructions::LABEL);
                this->while_flag = true;
            } else {
                this->if_flag = true;
            }
            this->Conditional();
            if (this->current->first == "{") {
                this->printLexeme();
                this->printRule(10);
                this->increment(1);
            } else {
                std::cerr << "Expected {\n";
                exit(1);
            }
        } else if (this->current->first == "else") {
            this->printLexeme();
            this->increment(1);
            if (this->current->first == "if") {
                this->Conditional();
            }
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
    if (this->current->first == "}" ) {
        if (this->if_flag) {
            int save = 0;
            if (this->getNext() != this->lexemes->end()) {
                if (this->getNext()->first == "else") {
                    save = this->instr_address;
                    this->genInstr(Instructions::JUMP);
                    this->else_flag = true;
                }
            }
            this->back_patch(this->instr_address);
            this->jump_stack.push(save);
            this->if_flag = false;
        }
        if (this->while_flag) {
            this->back_patch(this->instr_address + 1);
            this->genInstr(Instructions::JUMP, this->jump_stack.top());
            this->jump_stack.pop();
            this->while_flag = false;
        } else {
            if (this->getNext()->first != "else") {
                if (this->getNext()->first != "endif") {
                    std::cerr << "Error: Expected endif\n";
                    exit(1);
                } else {
                    if (this->else_flag) {
                        this->back_patch(this->instr_address);
                        this->else_flag = false;
                    }
                    this->increment(1);
                }
            }
        }
    }
//    this->increment(1);
    this->printLexeme();
}

void Parser::Conditional() {
    this->printLexeme();
    this->increment(1);
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
        char op = this->current->first[0];
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        if (this->current->first == "=") {
            this->printLexeme();
            this->printRule(11);
            this->increment(1);
            this->orequal_flag = true;
        }
        this->Expression();
        this->increment(1);
        switch (op) {
            case '<':
                if (this->orequal_flag) {
                    this->genInstr(Instructions::LEQ);
                } else {
                    this->genInstr(Instructions::LES);
                }
                break;
            case '>':
                if (this->orequal_flag) {
                    this->genInstr(Instructions::GEQ);
                } else {
                    this->genInstr(Instructions::GRT);
                }
                break;
            default:
                exit(1);
        }
        this->orequal_flag = false;
        this->jump_stack.push(this->instr_address);
        this->genInstr(Instructions::JUMPZ);
    } else if (this->current->first == "=" && this->getNext()->first == "=") {
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        this->printLexeme();
        this->printRule(11);
        this->increment(1);
        this->Expression();
        this->increment(1);
        this->genInstr(Instructions::EQU);
        this->jump_stack.push(this->instr_address);
        this->genInstr(Instructions::JUMPZ);
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
        if (this->symbol_table.find(this->getNext()->first) == this->symbol_table.end()) {
            // Instruction ???
            std::string current_type = this->current->first;
            this->increment(1);
            this->printLexeme();
            this->symbol_table.emplace(
                    this->current->first,
                    std::pair<int, int>(next_avail_memloc++, this->getType(current_type))
            );
            while (this->getNext()->first == ",") {
                this->increment(1);
                this->printLexeme();
                this->increment(1);
                this->printLexeme();
                if (this->symbol_table.find(this->current->first) == this->symbol_table.end()) {
                    this->symbol_table.emplace(
                            this->current->first,
                            std::pair<int, int>(next_avail_memloc++, this->getType(current_type))
                    );
                } else {
                    std::cerr << "Error::Attempted redeclaration of variable: " << this->current->first;
                    exit(1);
                }
            }
            if (this->getNext()->first != "=") {
                this->increment(1);
            }
        } else {
            std::cerr << "Error::Attempted redeclaration of variable: " << this->getNext()->first;
            exit(1);
        }

    } else {
        std::cerr << "Expected valid identifier\n";
    }

}

void Parser::Assignment() {
    this->printRule(5);
    if (this->current->second.first == Category::Identifiers) {
        std::string save = this->current->first;
        this->increment(1);
        this->printLexeme();
        this->increment(1);
        if (this->getNext()->first == ";") {
            this->Expression();
            this->increment(1);
        } else {
            this->Expression();
        }
        this->genInstr(Instructions::POPM, this->getAddress(save));
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
        this->increment_flag = false;
        this->increment(1);
        this->ExpressionPrime();
    }
}

void Parser::ExpressionPrime() {

    if (this->current->first == "+" || this->current->first == "-") {
        std::string oper = this->current->first;
        this->printLexeme();
        this->increment(1);
        this->printLexeme();
        this->Term();
        this->increment(1);
        if (oper == "+") {
            this->genInstr(Instructions::ADD);
        } else {
            this->genInstr(Instructions::SUB);
        }
        this->ExpressionPrime();
    }
}

void Parser::Term() {
//    this->printLexeme();
    this->printRule(1);
    this->Factor();
    if (this->getNext()->first == "*" || this->getNext()->first == "/") {
        this->increment_flag = false;
        this->increment(1);
        this->TermPrime();
    }
}

void Parser::TermPrime() {

    if (this->current->first == "*" || this->current->first == "/") {
        std::string oper = this->current->first;
        this->printLexeme();
        this->increment(1);
        this->printLexeme();
        this->Factor();
        this->increment(1);
        if (oper == "*") {
            this->genInstr(Instructions::MULT);
        } else {
            this->genInstr(Instructions::DIV);
        }
        this->TermPrime();
    }
}


//    /*syntax[2]*/ this->syntax.push_back("\t<Factor> -> ( <Expression> ) | <ID> | <Num>");
void Parser::Factor() {
    // this->printLexeme();
    this->printRule(2);
    if (this->current->second.first == Category::Identifiers)
    {
        this->genInstr(Instructions::PUSHM, getAddress(this->current->first));
        this->printRule(3);
    } else if (this->current->second.first == Category::Literals) {
        if (this->current->second.second == Numbers::Integer || this->current->second.second == Numbers::Float) {
            this->genInstr(Instructions::PUSHI, std::stoi(this->current->first));
        } else {
            std::cerr << "Invalid numerical factor\n";
        }
    }
    else if (this->current->first == "(") {
        this->increment(1);
        this->Expression();
        if (this->increment_flag) {
            this->increment(1);
        }
        if (this->current->first != ")") {
            std::cerr << "Expected \")\"\n";
            exit(1);
        } else {
            this->printLexeme();
        }
    }
    else if (this->current->first == "True" || this->current->first == "False") {
        this->genInstr(Instructions::PUSHI, (int)(this->current->first == "True") );
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

std::vector<std::pair<std::string, std::pair<int, int>>>::const_iterator Parser::getNext()
{
    if (this->current != this->lexemes->end()) {
        return std::next(this->current);
    }
    else {
        return this->lexemes->end();
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

void Parser::printInstructions() {
    std::cout << std::endl;
    this->parser_output << std::endl;
    for (int i = 0; i < this->instr_table.size(); i++) {
        Instruction instr = instr_table.at(i);
        std::cout << std::to_string(instr.address) + "\t" + instr.operation + "\t" + instr.operand + "\n";
        this->parser_output << std::to_string(instr.address) + "\t" + instr.operation + "\t" + instr.operand + "\n";
    }
    std::cout << std::endl;
    this->parser_output << std::endl;
}

void Parser::printSymbolTable() {
    std::cout << std::endl;
    this->parser_output << std::endl;
    std::cout << std::setw(20) << "Identifier" << std::setw(20) << "MemoryLocation" << std::setw(20) << "Type\n";
    this->parser_output << std::setw(20) << "Identifier" << std::setw(20) << "MemoryLocation" << std::setw(20) << "Type\n";
    for (auto symbol : this->symbol_table) {
        std::cout << std::setw(20) << symbol.first << std::setw(20) << symbol.second.first << std::setw(20) << type_names[symbol.second.second] << "\n";
        this->parser_output << std::setw(20) << symbol.first << std::setw(20) << symbol.second.first << std::setw(20) << type_names[symbol.second.second] << "\n";
    }
    std::cout << std::endl;
    this->parser_output << std::endl;
}
