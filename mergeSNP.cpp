#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

map<string, int> chrValue;

void initChrValue()
{
	for (int i=1; i<100; i++){
		stringstream ss;
		ss << "chr" << i;
		chrValue[ss.str()] = i;
	}
	chrValue["chrX"]=1000;
	chrValue["chrY"]=1001;
	chrValue["chrM"]=1002;
	chrValue["zzzzzz"]=10000;

}

int findMinPosFile(CLineFields file[], int fileNum)
{
	int min = 0;
	for (int i=1; i< fileNum; i++) {
		if (file[i].field.size()>0) {
		if (    (file[min].field.size()==0) ||
				(chrValue[file[i].field[0]]<chrValue[file[min].field[0]]) ||
				((file[i].field[0]==file[min].field[0]) && (strtoll(file[i].field[1].c_str(), NULL, 10) <strtoll(file[min].field[1].c_str(), NULL, 10)) ))
			min = i;
		}
	}
	return min;

}

bool allEndofFile(CLineFields file[], int fileNum)
{
	for (int i=0; i<fileNum; i++)
		if (file[i].endofFile()==false)
			return false;
	return true;		
}
int main(int argc, char *argv[])
{
	CLineFields file[32]; // 32 files at most.
	int fileNum = 0;
	initChrValue();

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-')
			continue;
		if (file[fileNum].openFile(argv[i])==false) {
			cerr << "Can not open file: " << argv[i] << endl;
			exit (-1);
		}
		fileNum ++;		
	}
	for(int i=0; i<fileNum; i++)
		file[i].readline();
	while (allEndofFile(file, fileNum)==false) {
		int i = findMinPosFile(file, fileNum);
		// skip the reference "*"
		if (file[i].field[2][0]=='*') {
			file[i].readline();//skip this line
			continue;
		}
		//output a position
		cout << file[i].field[0]<<"\t"<<file[i].field[1]<<"\t"<<file[i].field[2]<<"\t";
		string chrname = file[i].field[0];
		string position = file[i].field[1];
		char ref = file[i].field[2][0];
		for (int j=0; j<fileNum; j++) {
			if ((j==i)||((file[j].field.size()>2) && (file[j].field[0]==chrname) &&(file[j].field[1]==position))) {
				cout << pileupLongStr2ShortStr(file[j].field[8], ref) << "\t";
				file[j].readline();
			}
			else
            	cout << "NoInfo\t";
		}
		cout << 0 << "\t"<< 0 << "\t"<< 0 << "\t\n";
	}
	for (int i=0; i<fileNum; i++) 
		file[i].fp->close();
	return 0;
}

