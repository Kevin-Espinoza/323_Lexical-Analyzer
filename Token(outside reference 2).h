#pragma once
#include <iostream>
#include <string>


using namespace std;

enum LexErrorCode_t { LEX_NO_ERROR, LEX_UNKNOWN_CHAR, LEX_ILLEGAL_ESCAPE_CHAR, LEX_END_OF_COMMENT_NOT_FOUND };

const string LexErrorCode_t_str[]={ "LEX_NO_ERROR","LEX_UNKNOWN_CHAR","LEX_ILLEGAL_ESCAPE_CHAR", "LEX_END_OF_COMMENT_NOT_FOUND" };

enum TokenTypes_t { LEX_UNDEFINED,
					LEX_LINE_COMMENT, LEX_BEGIN_COMMENT, LEX_END_COMMENT,
					LEX_IDENTIFIER, LEX_KEYWORD,
					LEX_CHAR,   LEX_INTEGER,  LEX_STRING,
					LEX_OPERATOR,
					LEX_ASSIGNMENT,
					LEX_COMMA, LEX_SEMICOLON,
					LEX_LEFT_PARENTHESIS, LEX_RIGHT_PARENTHESIS,
					LEX_LEFT_BRACE, LEX_RIGHT_BRACE,
					LEX_END };

const string TokenTypes_t_str[]={ "LEX_UNDEFINED","LEX_LINE_COMMENT","LEX_BEGIN_COMMENT", "LEX_END_COMMENT",
                            "LEX_IDENTIFIER","LEX_KEYWORD","LEX_CHAR", "LEX_INTEGER","LEX_STRING","LEX_OPERATOR","LEX_ASSIGNMENT", "LEX_COMMA",
                            "LEX_SEMICOLON","LEX_LEFT_PARENTHESIS","LEX_RIGHT_PARENTHESIS", "LEX_LEFT_BRACE",
                            "LEX_RIGHT_BRACE","LEX_END" };

enum Operator_t {   LEX_NEG,  LEX_INC,  LEX_DEC,  LEX_NOT,
    				LEX_NO_OPR, LEX_ADD,  LEX_MUL,  LEX_SUB,  LEX_DIV,
					LEX_EQ,   LEX_NE,   LEX_LT,   LEX_GT,  LEX_LE,  LEX_GE,
					LEX_COUT_OPR,  LEX_CIN_OPR };

const string Operator_t_str[]={ "LEX_NEG","LEX_INC","LEX_DEC", "LEX_NOT",
                            "LEX_NO_OPR","LEX_ADD","LEX_MUL", "LEX_SUB","LEX_DIV","LEX_EQ","LEX_NE", "LEX_LT",
                            "LEX_GT","LEX_LE","LEX_GE",
                            "LEX_COUT_OPR","LEX_CIN_OPR" };



enum Keyword_t { LEX_IF,     LEX_ELSE,  LEX_FOR,   LEX_DO,   LEX_WHILE,
                 LEX_SWITCH, LEX_CASE,  LEX_BREAK, LEX_COUT, LEX_CIN };

const string Keyword_t_str[]={ "LEX_IF","LEX_ELSE","LEX_FOR", "LEX_DO",
                            "LEX_WHILE","LEX_SWITCH","LEX_CASE", "LEX_BREAK","LEX_COUT","LEX_CIN" };


struct token_t
{
	TokenTypes_t TokenType;
	string       TokenString;
	union
	{
		Keyword_t  KeywordID;
		int        IntVal;
		char       CharVal;
		Operator_t OprID;
	};
};

void InitKeywordMap();


LexErrorCode_t GetToken        (string Buffer, int &Index, FILE ListingFile, token_t &token, bool testing = false);
LexErrorCode_t GetEndOfComment(string Buffer, int &Index, FILE &ListingFile, token_t &token, bool testing = false);
 


class InterpExc 
{
	LexErrorCode_t err;
public:
	InterpExc(LexErrorCode_t e) { err = e; }
	LexErrorCode_t get_err() { return err; }

};
