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
#include <errno.h>
//#include <my_global.h>
//#include <mysql.h>
#include "CLineFields.h"

#define LINE_LENGTH 60

vector<string> chrNameList; // use this list keep the order of chromosomes
bool first_chr=true;
// print out the whole chromosomes map (chr1, chr2, etc.)
int printChrMap(map<string, string> chrMap, string suffix)
{
//	for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
	for (int i=0; i<(int)chrNameList.size(); i++) {
		string chrName = chrNameList[i];
		string value = chrMap[chrName];
		if (first_chr) {
			cout << ">"<< chrName << suffix;
			first_chr = false;
		} else {
			cout << endl << ">"<< chrName << suffix; 
		}
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

bool planB = false;

// return true if parameters are right.
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-planB", 6)==0) {
			planB = true;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	//int originalCount = 0;
	//int mutationCount = 0;
	map<string, string> chrMap; // a map from chr name to sequence string

	if (argc != 3) {
		cerr << "usage: " << argv[0] <<" haploid.fa  genotypes > alter.fa" << endl;
		cerr << "    this program create an alternative reference genome" << endl;
		cerr << "For example:" << endl;
		cerr << "if haploid.fa contains:   " << endl;
		cerr << "  >chr1" << endl;
		cerr << "  ACGTACGT" << endl;
		cerr << "genotypes contains:" << endl;
		cerr << ".	chr1	2	G	C" << endl;
		cerr << "then alter.fa contains:" << endl;
		cerr << "  >chr1" << endl;
		cerr << "  AGGTACGT" << endl;
		exit(-1);
	}
	parseParameters (argc, argv);
	CLineFields file, hapmap;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	// error happened when trying to get the mutation probability, so now we treat it as a hapmap file
	if (hapmap.openFile(argv[2])==false) {
		cerr << "Can not open file: " << argv[2] << endl;
		exit (-1);
	}
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

	if (hapmap.isOpen()) {// use hapmap file to create the ref and non-ref allele

		int target = -1;
		hapmap.readline();
		while(hapmap.endofFile()==false) {
			unsigned int pos = atoi(hapmap.sect[2]);
			if (chrMap[hapmap.sect[1]].length()>=pos) {
				{
					if (toupper(chrMap[hapmap.sect[1]][pos-1]) ==toupper(hapmap.sect[4][0]))
						target = 3;
					else
						target = 4;
				}
				if (chrMap[hapmap.sect[1]][pos-1] >='A' && chrMap[hapmap.sect[1]][pos-1] <='Z')
					chrMap[hapmap.sect[1]][pos-1] = toupper(hapmap.sect[target][0]);
				else
					chrMap[hapmap.sect[1]][pos-1] = tolower(hapmap.sect[target][0]);
			}
			hapmap.readline();
		}
		printChrMap(chrMap, "");  // print out non-reference allele
		hapmap.closeFile();
	} 
	cout << endl;
	file.closeFile();
}

