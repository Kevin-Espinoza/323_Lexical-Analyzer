
#include "stdafx.h"
//to let us read/write to the screen
			//for working with Text/String data
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define KW_COUNT 5 //count of keywords
#define SC_COUNT 9 //count of special characters
#define MAX_SYMBOLS 100 //This is maximum symbols that the program can handle. Increase if needed
#define MAX_TOK_SIZE 256 //Max token size
class LexParser {
private:
	static const string keywords[]; //list of keywords
	static const char spChar[]; //list of special characters
	enum tokenClass { KEYWORD, IDENTIFIER, INTEGER, REAL, SPECIAL };

	struct symbol {
		string token; //token
		int count; //token count
		tokenClass tc; //token class
	};
	symbol symbolTable[MAX_SYMBOLS]; //list of all symbols
	int symbolCount;
	char nextToken; //This holds first character of next token but not entire token
	string error;
public:
	LexParser() {
		symbolCount = 0;
		error = "";
		nextToken = 0;
	}
	bool parse(char* fn) {
		ifstream fin;
		string s;

		fin.open(fn);
		if (fin.bad()) {
			error = "Cannot open the file";
			fin.close();
			return false;
		}
		nextToken = fin.get();

		//Parse through entire file
		while (!fin.eof()) {
			//Read the next token and according to the type
			readToken(fin, s);

			if (isKeyword(s))
				addSymbol(s, KEYWORD);
			else if (isInteger(s)) {
				if (nextToken == '.') {
					//if "." is found after integer then this will be real so parse next token and
					//ass real number
					string s2;
					nextToken = fin.get();
					readToken(fin, s2);
					s = s + "." + s2;
					addSymbol(s, REAL);
				}
				else
					addSymbol(s, INTEGER);
			}
			else if (isSpecialChar(s))
				addSymbol(s, SPECIAL);
			else if (isIdentifier(s))
				addSymbol(s, IDENTIFIER);
			else {
				error = "Error occurred. Unknown token \"" + s + "\"";
				return false;
			}
		}
		fin.close();
		return true;
	}
	string getError(void) {
		return error;
	}

	//This function is used to read the next token. Token are seperated by delimiters and special
	//symbols. this function identifies those delimiters and Sp.Chars and returns the token
	void readToken(ifstream& fin, string& s) {
		char scanstring[] = "\n\t ()[]+-=,;.\0";
		char spacestring[] = "\n\t ";
		char specialstring[] = "()[]+-=,;";
		char token[MAX_TOK_SIZE];
		int i = 0;

		//Special Characters are to be handled seperately
		if (strchr(specialstring, nextToken)) {
			token[i++] = nextToken;
			nextToken = fin.get();
			while (strchr(spacestring, nextToken) && fin.good()) {
				nextToken = fin.get();
			}
			token[i] = '\0';
			s = string(token);
			return;
		}
		//This loop will handle all tokens except special characters
		while (fin.good()) {
			if (strchr(scanstring, nextToken)) {
				while (strchr(spacestring, nextToken) && fin.good()) {
					nextToken = fin.get();
				}
				token[i] = '\0';
				s = string(token);
				return;
			}
			else
				token[i++] = nextToken;
			nextToken = fin.get();
		}
	}
	bool isKeyword(string& s) {
		for (int i = 0; i < KW_COUNT; i++)
			if (!s.compare(keywords[i]))
				return true;
		return false;
	}
	bool isSpecialChar(string& s) {
		if (s.length() != 1)
			return false;
		char sc = s.at(0);
		for (int i = 0; i < SC_COUNT; i++)
			if (sc == spChar[i])
				return true;
		return false;
	}
	bool isIdentifier(string& s) {
		for (int i = 0; i < s.length(); i++)
			if (!((s.at(i) >= 'a' && s.at(i) <= 'z') ||
				(s.at(i) >= 'A' && s.at(i) <= 'Z')))
				return false;
		return true;
	}
	bool isInteger(string& s) {
		for (int i = 0; i < s.length(); i++)
			if (s.at(i) < '0' || s.at(i) > '9')
				return false;
		return true;
	}

	/*
	* This function will add symbols to the symbolTable. If a symbols already exists it increases
	* the symbol count instead to adding it again.
	*/
	void addSymbol(string& tok, tokenClass tc) {
		for (int i = 0; i < symbolCount; i++)
			if (!tok.compare(symbolTable[i].token)) {
				symbolTable[i].count++;
				return;
			}
		symbolTable[symbolCount].token = tok;
		symbolTable[symbolCount].count = 1;
		symbolTable[symbolCount++].tc = tc;
		return;
	}
	void printTokenList() {
		cout << "Symbol \t Count \t TokenType\n\n";
		for (int i = 0; i < symbolCount; i++) {
			cout << symbolTable[i].token << "\t" << symbolTable[i].count << "\t";
			if (symbolTable[i].tc == INTEGER)
				cout << "INTEGER" << endl;
			else if (symbolTable[i].tc == REAL)
				cout << "REAL" << endl;
			else if (symbolTable[i].tc == IDENTIFIER)
				cout << "IDENTIFIER" << endl;
			else if (symbolTable[i].tc == KEYWORD)
				cout << "KEYWORD" << endl;
			else
				cout << "SPECIAL" << endl;
		}
	}
};
const string LexParser::keywords[] = { "if", "then", "else", "begin", "end" };
const char LexParser::spChar[] = { '(', ')', '[', ']', '+', '-', '=', ',', ';' };

int main(int argc, char* argv[])
{
	LexParser parser;
	if (!parser.parse(argv[1])) {
		cout << "Parser returned error: " << parser.getError();
		exit(0);
	}
	parser.printTokenList();
	return 0;
	}