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
#include "CLineFields1.h"

#define LINE_LENGTH 60

vector<string> chrNameList; // use this list keep the order of chromosomes

// print out the whole chromosomes map (chr1, chr2, etc.)
int printChrMap(map<string, string> chrMap, string suffix)
{
//	for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
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
	char baseSet[128][4];
	//int originalCount = 0;
	//int mutationCount = 0;
	map<string, string> chrMap; // a map from chr name to sequence string

	if (argc != 3) {
		cerr << "usage: " << argv[0] <<" haploid.fa  mutationProbability/hapmap [-planB]> diploid.fa" << endl;
		cerr << "example: " << argv[0] <<" mm9.fa  0.001 > mm9diploid.fa" << endl;
		cerr << "              " << argv[0] <<" mm9.fa  hapmap.txt > mm9diploid.fa" << endl;
		exit(-1);
	}
	parseParameters (argc, argv);
	CLineFields file, hapmap;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	errno = 0;
	char *endptr;
	float prob = strtod(argv[2], &endptr);
	if (errno != 0  || endptr == argv[2]) {
		// error happened when trying to get the mutation probability, so now we treat it as a hapmap file
		if (hapmap.openFile(argv[2])==false) {
			cerr << "Can not open file: " << argv[2] << endl;
			exit (-1);
		}
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
			unsigned int pos = atoi(hapmap.field[2].c_str());
			if (chrMap[hapmap.field[1]].length()<pos) {
				cerr << "Warning!!! SNP out of range at: " << hapmap.field[1] << ": " << hapmap.field[2] << endl;
				// exit (-1);
			} else {
				if (planB) target = 4;
				else {
					// decide which is ref and which is non-ref
					if (toupper(chrMap[hapmap.field[1]][pos-1]) ==toupper(hapmap.field[4][0]))
						target = 4;
					else
						target = 3;
				}
				if (chrMap[hapmap.field[1]][pos-1] >='A' && chrMap[hapmap.field[1]][pos-1] <='Z')
					chrMap[hapmap.field[1]][pos-1] = toupper(hapmap.field[target][0]);
				else
					chrMap[hapmap.field[1]][pos-1] = tolower(hapmap.field[target][0]);
			}
			hapmap.readline();
		}
		printChrMap(chrMap, "");  // print out reference allele
		hapmap.seek(0);// seek to the beginning of the file
		hapmap.readline();
		while(hapmap.endofFile()==false) {
			unsigned int pos = atoi(hapmap.field[2].c_str());
			if (chrMap[hapmap.field[1]].length()>=pos) {
				if (planB) target = 3;
				else {
					if (toupper(chrMap[hapmap.field[1]][pos-1]) ==toupper(hapmap.field[4][0]))
						target = 3;
					else
						target = 4;
				}
				if (chrMap[hapmap.field[1]][pos-1] >='A' && chrMap[hapmap.field[1]][pos-1] <='Z')
					chrMap[hapmap.field[1]][pos-1] = toupper(hapmap.field[target][0]);
				else
					chrMap[hapmap.field[1]][pos-1] = tolower(hapmap.field[target][0]);
			}
			hapmap.readline();
		}
		printChrMap(chrMap, "b");  // print out non-reference allele
		hapmap.closeFile();
	} else {
		printChrMap(chrMap, ""); // print out original haploid genome first.
		// initialize the transform table;   A-C-G-T
		baseSet[(int)'A'][1]='C'; baseSet[(int)'A'][2]='G'; baseSet[(int)'A'][3]='T';
		baseSet[(int)'C'][1]='G'; baseSet[(int)'C'][2]='T'; baseSet[(int)'C'][3]='A';
		baseSet[(int)'G'][1]='T'; baseSet[(int)'G'][2]='A'; baseSet[(int)'G'][3]='C';
		baseSet[(int)'T'][1]='A'; baseSet[(int)'T'][2]='C'; baseSet[(int)'T'][3]='G';
		baseSet[(int)'a'][1]='c'; baseSet[(int)'a'][2]='g'; baseSet[(int)'a'][3]='t';
		baseSet[(int)'c'][1]='g'; baseSet[(int)'c'][2]='t'; baseSet[(int)'c'][3]='a';
		baseSet[(int)'g'][1]='t'; baseSet[(int)'g'][2]='a'; baseSet[(int)'g'][3]='c';
		baseSet[(int)'t'][1]='a'; baseSet[(int)'t'][2]='c'; baseSet[(int)'t'][3]='g';
		srand(time(NULL));
		for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
			string value = (*it).second;
			for (int i=0, length=value.length(); i<length; i++) {
				if (value[i]=='N' || value[i]=='n')
					continue;
				float randomNumber = (float) rand () / RAND_MAX;
				if (randomNumber >= prob) {
					// we don't have mutation
					// originalCount ++;
				} else {
					int offset = (int)(randomNumber/(prob/3)) + 1;  // offset in [1, 3]
					((*it).second)[i] = baseSet[(int)value[i]][offset];
					//mutationCount ++;
				}
			}
		}
		printChrMap(chrMap, "b");  // print out non-reference allele
	}
	cout << endl;
	file.closeFile();
}

