// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

map<string, string> results;

// f1, f2 are 0-based
int handlefile (char *filename, int f1, int f2)
{
	CLineFields file;

	file.openFile(filename);
	file.readline();
	while (file.endofFile()==false) {

		if( f1 >= (int)file.sect_num ||  f2 >= (int)file.sect_num)
		{
			cerr << "file format error! make sure your files have enough columns seperated by Tabs" << endl;
			exit(-1);
		}
		string f1f2 = string(file.sect[f1])+","+string(file.sect[f2]); //f1f2 is used as the index
		if  (results[f1f2] == "")
			results[f1f2] = string(filename)+"\t"+file.line;
		else
			results[f1f2] = results[f1f2]+"\t"+string(filename)+"\t"+file.line;
		file.readline();
	}
	file.closeFile();
	return 0;
}

int main(int argc, char* argv[])
{
	CLineFields file;

	if (argc != 4) {
		cerr << "join the file with itself according to fields (1-based) so that duplicate lines can be removed" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " file field1 field2 > output" << endl;
		return -1;
	}
	// handle the first file
	int f1 = atoi(argv[2])-1;
	int f2 = atoi(argv[3])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[1], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}
	// output the final results
	for (map<string, string>::iterator  it= results.begin(); it!= results.end(); it++) {
		cout << (*it).second << endl;
	}

	return 0;
}
