#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

int main(int argc, char *argv[])
{
	CLineFields file;
	long long int s1count = 0, s2count=0;//how many letters are there matching s1 or s2.
	int columnOfS1 = -1, columnOfS2 = -1;
	int columnOfChild = -1;
	bool diffS1S2 = false;
	map<char,int> pileupInfoS1, pileupInfoS2, pileupInfoChild;
	
	if (argc < 5) {
		cerr << "error! input should be:" << endl;
		cerr << argv[0] <<" file s1  s2  child [-d]"<< endl;
		cerr << "-d: s1 must not be the same as s2" << endl;
		exit(-1);
	}
	
	columnOfS1 = atoi(argv[2]);
	columnOfS2 = atoi(argv[3]);
	columnOfChild = atoi(argv[4]);
	if (argc >= 6)
		diffS1S2 = (strcmp(argv[5], "-d")==0);

	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}

	file.readline();
	while (file.endofFile()==false){
		pileupShortStr2Info(file.field[columnOfS1], pileupInfoS1);
		pileupShortStr2Info(file.field[columnOfS2], pileupInfoS2);
		pileupShortStr2Info(file.field[columnOfChild], pileupInfoChild);
		char majorS1 = pileupInfoMajor(pileupInfoS1, 0.8);
		char majorS2 = pileupInfoMajor(pileupInfoS2, 0.8);

		if ((diffS1S2==false) || (majorS1 != majorS2)) {		
			if (majorS1)
				s1count += pileupInfoChild[majorS1];
			if (majorS2)
				s2count += pileupInfoChild[majorS2];		
		}
		file.readline();
	}
	cout << s1count << "\t" << s2count <<"\t(ratio: 1:"<<(double)((double)s2count/(double)s1count)<< ")" <<endl;
	file.fp->close();
	return 0;
}



