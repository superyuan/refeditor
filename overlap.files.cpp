// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields1.h"

unsigned char mark[300*1024*1024];

// f2 are 0-based
int handlefile (char *filename, int f2, int files)
{
	CLineFields file;

	file.openFile(filename);
	file.readline();
	while (file.endofFile()==false) {

		if(f2 >= (int)file.field.size())
		{
			cerr << "file format error! make sure your files have enough columns seperated by Tabs" << endl;
			exit(-1);
		}
		int pos = atoi(file.field[f2].c_str());
		mark[pos]++;
		if (mark[pos]==files) {
		// this position is covered by all files.
			cout << file.line << endl;
		}

		file.readline();
	}
	file.closeFile();
	return 0;
}

int main(int argc, char* argv[])
{
	CLineFields file;

	if (argc < 4) {
		cerr << "join files according to a numerical field (1-based)" <<endl;
		cerr << "only keep the ones contained in all files, and print them out from the first file" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " field file1 file2 [file3...] > output" << endl;
		return -1;
	}
	memset(mark,0,sizeof(mark));

	// handle the files
	int f2 = atoi(argv[1])-1;
	if (f2>=0)
	{
		for (int i=3; i<argc; i++)
			handlefile(argv[i], f2, argc-2);
		handlefile(argv[2], f2, argc-2);
	}
	else
	{
		cerr << "parameter error! " << endl;
		return -1;
	}

	return 0;
}
