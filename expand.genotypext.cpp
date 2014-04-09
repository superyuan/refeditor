// join2files.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

//map<string, string> results;
unsigned char mark[300*1024*1024];

// f1, f2 are 0-based
int handlefile1 (char *filename, int f1, int f2)
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
		int pos = atoi(file.sect[f2]);
		if (mark[pos]==0) {
                        cout << file.line << endl;
                        mark[pos] = (unsigned char)1;
                }
                else {
                }   

		//string f1f2 = file.sect[f1]+","+file.sect[f2]; //f1f2 is used as the index
		//if  (results[f1f2] == "")
		//	results[f1f2] = file.line;
		file.readline();
	}
	file.closeFile();
	return 0;
}

int handlefile2 (char *filename, int f1, int f2)
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
		int pos = atoi(file.sect[f2]);
		if (mark[pos]==0) {
                        cout << file.sect[0]<<"\t"<<file.sect[1]<<"\t"<<file.sect[2]<<"\t"<<file.sect[6]<<"\t"<<file.sect[6]<<"\t"<<file.sect[5]<<"\t"<<file.sect[6] << endl;
                        mark[pos] = (unsigned char)1;
                }   
                else {
                }   
//		string f1f2 = file.sect[f1]+","+file.sect[f2]; //f1f2 is used as the index
//		if  (results[f1f2] == "")
//			results[f1f2] = file.sect[0]+"\t"+file.sect[1]+"\t"+file.sect[2]+"\t"+file.sect[6]+"\t"+file.sect[6]+"\t"+file.sect[5]+"\t"+file.sect[6];
		file.readline();
	}
	file.closeFile();
	return 0;
}

int main(int argc, char* argv[])
{
	CLineFields file;

	if (argc != 3) {
		cerr << "use the second genotypext file to expand the first genotypxt" <<endl;
		cerr << "locations that don't exist in the first genotypext but exist in the second file will be set as ref/ref" <<endl;
		cerr << "inconsitente genotypes will only be kept for the ones in the first genotypext file" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << " file1 file2 > output" << endl;
		cerr << "    note: this program can only handle one chromosome!!!" << endl;
		return -1;
	}
	memset(mark,0,sizeof(mark));
	// handle the first file
	int f1 = 1;
	int f2 = 2;
	handlefile1(argv[1], f1, f2);
	// handle the second file
	handlefile2(argv[2], f1, f2);
	// output the final results
//	for (map<string, string>::iterator  it= results.begin(); it!= results.end(); it++) {
//		cout << (*it).second << endl;
//	}

	return 0;
}
