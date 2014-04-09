// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

map<string, string> results;
map<string, int> exist;

// f1, f2 are 0-based
int handlefile (char *filename, int f1, int f2, int flag)
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
			results[f1f2] = file.line;
		//else
		//	results[f1f2] = results[f1f2]+"\t"+string(filename)+"\t"+file.line;
		exist[f1f2] = (exist[f1f2] | flag);
		file.readline();
	}
	file.closeFile();
	return 0;
}

int main(int argc, char* argv[])
{
	CLineFields file;

	if (argc != 7) {
		cerr << "join 2 files according to fields (1-based), print out the ones ONLY in the first file." <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " file1 file2  file1_field1 file1_field2 file2_field1 file2_field2 > output" << endl;
		return -1;
	}
	// handle the first file
	int f1 = atoi(argv[3])-1;
	int f2 = atoi(argv[4])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[1], f1, f2, 1);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}
	// handle the second file
	f1 = atoi(argv[5])-1;
	f2 = atoi(argv[6])-1;
	if (f1>=0 && f2>=0)
	{
		handlefile(argv[2], f1, f2, 2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}
	// output the final results
	for (map<string, int>::iterator  it= exist.begin(); it!= exist.end(); it++) {
		if ( (*it).second == 1 ) {
			// only in file 1
			cout << results[(*it).first] << endl;
		}
		//cout << (*it).second << endl;
	}

	return 0;
}
