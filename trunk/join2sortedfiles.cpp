// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

//map<string, string> results;

// f1, f2 are 0-based
int handlefiles (char *filename, char *filename2, int f1, int f2)
{
	CLineFields file, file2;

	file.openFile(filename);
	file.readline();
	file2.openFile(filename2);
	file2.readline();
	int num1=atoi(file.sect[f1]);
	int num2=atoi(file2.sect[f2]);

	while (file.endofFile()==false || file2.endofFile()==false) {

		if ((num1<num2 && file.endofFile()==false) || file2.endofFile()) {
			cout << filename << "\t" << file.line << endl;
			file.readline();
			num1=atoi(file.sect[f1]);
		} else if ((num1>num2 && file2.endofFile()==false) || file.endofFile()) {
			cout << filename2 <<"\t" <<file2.line << endl;
			file2.readline();
			num2=atoi(file2.sect[f2]);
		} else if (num1==num2) {
			cout << filename << "\t" << file.line <<"\t"<< filename2 <<"\t" <<file2.line << endl;
			file.readline();
			file2.readline();
			num1=atoi(file.sect[f1]);
			num2=atoi(file2.sect[f2]);
		} else {
			cerr << "error!" << endl;
			cerr << "file1:" << file.line << endl;
			cerr << "file2:" << file2.line << endl;
			exit(-1);
		}
	}
	file.closeFile();
	file2.closeFile();
	return 0;
}

int main(int argc, char* argv[])
{
	CLineFields file;

	if (argc != 5) {
		cerr << "join 2 files according to a numerical field (1-based)" <<endl;
		cerr << "files must already be sorted in ascending order by the given field" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " file1 file2  file1_field file2_field > output" << endl;
		return -1;
	}
	// handle the files
	int f1 = atoi(argv[3])-1;
	int f2 = atoi(argv[4])-1;
	if (f1>=0 && f2>=0)
	{
		handlefiles(argv[1], argv[2], f1, f2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}

	return 0;
}
