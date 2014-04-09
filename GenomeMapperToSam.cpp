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
#include <boost/lexical_cast.hpp>
#include "CLineFields.h"

class SamLine
{
public:	
	string f[21];
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cerr << "convert the mapped file output by GenomeMapper to sam file format." << endl;
		cerr << "usage: " << argv[0] <<" result.mapped > result.sam" << endl;
		exit(-1);
	}
	map<string, SamLine> sam;
	CLineFields fileMapped;
	if (fileMapped.openFile(argv[1])==false) {
		cerr << "Can not open mapped file: " << argv[1] << endl;
		exit (-1);
	}
	// process mapped file
	fileMapped.readline();
	while (fileMapped.endofFile()==false) {
		string id = fileMapped.field[6];
		if (sam[id].f[0] == "") {
			sam[id].f[0]=id;//id in sam
			sam[id].f[1]="0";//assume on the + strand
			sam[id].f[2]=fileMapped.field[1]; // chr
			sam[id].f[3]=fileMapped.field[2]; // pos
			sam[id].f[4]="23"; // score
			sam[id].f[5]="35M"; // 
			sam[id].f[6]="*";
			sam[id].f[7]="0";
			sam[id].f[8]="0";
			sam[id].f[9]="";
			bool inBrace = false;
			int mm = 0;
			for (int i=0, length=fileMapped.field[4].length(); i<length; i++) {
				char c = fileMapped.field[4][i];
				if (c=='[' || c=='(') {
					mm++;
					inBrace = true;
				} else if (c==']' || c==')') {
					sam[id].f[9]+=fileMapped.field[4][i-1];
					inBrace = false;
				} else if (!inBrace){
					sam[id].f[9]+=c;
				}
			}
			sam[id].f[10]="~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
			sam[id].f[11]="XT:A:U";
			sam[id].f[12]="NM:i:0";
			sam[id].f[13]="NM:i:"+boost::lexical_cast<std::string>(mm);
			sam[id].f[14]="X0:i:1";
			sam[id].f[15]="X1:i:0";
			sam[id].f[16]="XM:i:0";
			sam[id].f[17]="XO:i:0";
			sam[id].f[18]="XG:i:0";
			sam[id].f[19]="MD:Z:35";
			sam[id].f[20]="XA:Z:";
		} else {
			sam[id].f[20] = sam[id].f[20] + fileMapped.field[1]+","+fileMapped.field[2]+",35M,1;";
		}
		fileMapped.readline();
	}
	for(map<string,SamLine>::iterator it=sam.begin(); it!=sam.end(); it++) {
		SamLine sl = (*it).second;
		cout << sl.f[0];
		for (int i=1; i<20; i++)
			cout << "\t" << sl.f[i];
		if (sl.f[20] != "XA:Z:")
			cout << "\t" << sl.f[20];
		cout << endl;
	}
	fileMapped.fp->close();
}


