// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields1.h"

map<string, string> results;

// f1, f2 are 0-based
int handlefile (char *filename, int f1, int f2)
{
	CLineFields file;

	file.openFile(filename);
	file.readline();
	while (file.endofFile()==false) {

		if( f1 >= (int)file.field.size() ||  f2 >= (int)file.field.size())
		{
			cerr << "file format error! make sure your files have enough columns seperated by Tabs" << endl;
			exit(-1);
		}
		string f1f2 = file.field[f1]+","+file.field[f2]; //f1f2 is used as the index
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

	if (argc != 16) {
		cerr << "join 5 files according to fields (1-based)" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " file1 file2 file3 file4 file5 file1_field1 file1_field2 file2_field1 file2_field2 file3_field1 file3_field2 file4_field1 file4_field2 file5_field1 file5_field2 > output" << endl;
		return -1;
	}
	// handle the first file
	int f1 = atoi(argv[6])-1;
	int f2 = atoi(argv[7])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[1], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}
	// handle the second file
	f1 = atoi(argv[8])-1;
	f2 = atoi(argv[9])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[2], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}
	// handle the third file
	f1 = atoi(argv[10])-1;
	f2 = atoi(argv[11])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[3], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}

	// handle the fourth file
	f1 = atoi(argv[12])-1;
	f2 = atoi(argv[13])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[4], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}

	// handle the fifth file
	f1 = atoi(argv[14])-1;
	f2 = atoi(argv[15])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[5], f1, f2);
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
