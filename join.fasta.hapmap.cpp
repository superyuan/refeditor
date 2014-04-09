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

int main(int argc, char *argv[])
{
	map<string, string> chrMap; // a map from chr name to sequence string

	if (argc != 3) {
		cerr << "this program join fasta file and hapmap file to decide which is reference and which is alternative" << endl;
		cerr << "usage: " << argv[0] <<" fasta  hapmap " << endl;
		cerr << "example: " << argv[0] <<" hg18.fa  hapmap.txt > hapmap_ext.txt" << endl;
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
	file.readline();
	string chrName = "";
	while (file.endofFile()==false) {
		if (file.line[0]=='>') {
			chrName = file.line.substr(1);
			chrMap[chrName] = "";
		} else {
			chrMap[chrName] += file.line;
		}
		file.readline();
	}
	file.closeFile();
	cout << "@rs#	Chr	Position	Base1	Base2	Ref" << endl;
	hapmap.readline();
	while(hapmap.endofFile()==false) {
		unsigned int pos = atoi(hapmap.field[2].c_str());
		if (chrMap[hapmap.field[1]].length()<pos) {
			cerr << "SNP out of range at: " << hapmap.field[1] << ": " << hapmap.field[2] << endl;
			exit (-1);
		}
		cout << hapmap.line;
		if (toupper(hapmap.field[4][0])==hapmap.field[4][0]) {
			// if hapmap is in capital
			cout << "\t"<< char(toupper(chrMap[hapmap.field[1]][pos-1])) << endl;
		} else {
			cout << "\t"<< char(tolower(chrMap[hapmap.field[1]][pos-1])) << endl;
		}
		hapmap.readline();
	}
	hapmap.closeFile();
}

