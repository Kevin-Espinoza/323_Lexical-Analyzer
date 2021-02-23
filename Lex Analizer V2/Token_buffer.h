#pragma once
#include <string>
using namespace std;

// token buffer builds a token from the input string by matching one character at a time

struct token_buffer
{
	//this is the token currently being construted
	string token;

	// this is the most recently read character from the input
	int now;

	//resets the buffer
	void clear();

	//matches exactly one terminal symbol
	bool match(const int m, string& line);
};

void token_buffer::clear()
{
	token = "";
	now = 0;
}

bool token_buffer::match(const int m, string& line)
{
	int length = line.length();

	// if the current character in the buffer now is a match for m
	if (now == m)
	{
		if (now != 0)
		{
			token = to_string(static_cast <char>(now)) + token;
		}
		now = static_cast<int>(line[0]);

		// this section removes the first character of the input line
		line = line.substr(1, length - 1);
	}
	else
	{
		return false;
	}

	return true;
}

