#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <string.h>
//#include <my_global.h>
//#include <mysql.h>
#include "CLineFields.h"

#define LINE_LENGTH 60

vector<string> chrNameList; // use this list keep the order of chromosomes


// print out the whole chromosomes map (chr1, chr2, etc.)
int printChrMap(map<string, string> chrMap, string suffix)
{
	//for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
	for (int i=0; i<(int)chrNameList.size(); i++) {
		string chrName = chrNameList[i];
		string value = chrMap[chrName];
		cout << endl <<">"<< chrName << suffix;
		//cout << endl <<">"<< (*it).first << suffix;
		//string value = (*it).second;
		int counter = 0;
		for (int i=0, length=value.length(); i<length; i++) {
			if (counter % LINE_LENGTH == 0)
				cout << endl;
			cout << value[i];
			counter ++;
		}
	}
	return 0;
}

char maskTable[128][128];

void initMaskTable(void)
{
	maskTable[(int)'A'][(int)'C']=(int)'G'; maskTable[(int)'A'][(int)'G']=(int)'T'; maskTable[(int)'A'][(int)'T']=(int)'G';
	maskTable[(int)'C'][(int)'A']=(int)'T'; maskTable[(int)'C'][(int)'G']=(int)'T'; maskTable[(int)'C'][(int)'T']=(int)'A';
	maskTable[(int)'G'][(int)'A']=(int)'C'; maskTable[(int)'G'][(int)'C']=(int)'A'; maskTable[(int)'G'][(int)'T']=(int)'A';
	maskTable[(int)'T'][(int)'A']=(int)'C'; maskTable[(int)'T'][(int)'C']=(int)'G'; maskTable[(int)'T'][(int)'G']=(int)'C';
}

int main(int argc, char *argv[])
{
	map<string, string> chrMap; // a map from chr name to sequence string
	if (argc != 3) {
		cerr << "usage: " << argv[0] <<" haploid.fa hapmap > masked.fa" << endl;
		cerr << "example: " << argv[0] <<" mm9.fa  hapmap.txt  > mm9masked.fa" << endl;
		exit(-1);
	}
	CLineFields file, hapmap;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	if (hapmap.openFile(argv[2])==false) {
		cerr << "Can not open file: " << argv[2] << endl;
		exit (-1);
	}
	initMaskTable();
	file.readline();
	string chrName = "";
	while (file.endofFile()==false) {
		if (file.line[0]=='>') {
			chrName = file.line.substr(1);
			chrMap[chrName] = "";
			chrNameList.push_back(chrName);
		} else {
			chrMap[chrName] += file.line;
		}
		file.readline();
	}
	int swap = 0; // we psedo-randomly swap the two columns in hapmap file. so that ['A']['G'] can be masked to 'C' or 'T'.
	hapmap.readline();
	while(hapmap.endofFile()==false) {
		unsigned int pos = atoi(hapmap.sect[2]);
		if (pos<=0) {
			cerr << "Warning: read wrong format data: " << hapmap.line << endl;
			hapmap.readline();
			continue;
		}
		if (chrMap[hapmap.sect[1]].length()<pos) {
			cerr << "SNP out of range at: " << hapmap.sect[1] << ": " << hapmap.sect[2] << endl;
			exit (-1);
		}
		if (chrMap[hapmap.sect[1]][pos-1] >='A' && chrMap[hapmap.sect[1]][pos-1] <='Z')
			chrMap[hapmap.sect[1]][pos-1] = toupper(maskTable[(int)hapmap.sect[3+swap][0]][(int)hapmap.sect[4-swap][0]]);
		else
			chrMap[hapmap.sect[1]][pos-1] = tolower(maskTable[(int)hapmap.sect[3+swap][0]][(int)hapmap.sect[4-swap][0]]);
		swap = 1-swap;
		hapmap.readline();
	}
	printChrMap(chrMap, "");  // print out reference allele
	hapmap.closeFile();
	cout << endl;
	file.closeFile();
}

