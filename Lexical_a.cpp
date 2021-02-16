
#include "stdafx.h"
#include <iostream>		//to let us read/write to the screen
#include <string>			//for working with Text/String data

#include < fstream>  // to do file IO

using namespace std;

int main()
{
	ifstream myReader("Lex_test1.txt");

	string lineFromFile;

	if (myReader.is_open())										//make sure file can be opened
	{
		while (getline(myReader, lineFromFile))		//read to end of file
		{
			cout << lineFromFile << '\n';				//print out each line
		}
		myReader.close();										//always close the file
	}

	else cout << "Unable to open file";

	return 0;
}