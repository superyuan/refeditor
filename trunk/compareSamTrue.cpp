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
#include "CLineFields1.h"


typedef struct __samInfo_t {
	string chrPos;
	char strand;
	int quality;
} samInfo_t, *pSamInfo_t;;


int main(int argc, char *argv[])
{
	bool ignoreDiploid = false;
	map<string, samInfo_t> samMap; // a map from reads to aligned locations output from bwa
	map<string, string> trueMap; // a map from reads to true locations, decided when generating reads

	if (argc < 3) {
		cerr << "compare the resulted sam file and the true alignment" << endl;
		cerr << "usage: " << argv[0] <<" result.sam  result.fq.true [-i]" << endl;
		//cout << "example: " << argv[0] <<" mm9.fa  0.001 > mm9diploid.fa" << endl;
		exit(-1);
	}
	CLineFields fileSam, fileTrue;
	if (fileSam.openFile(argv[1])==false) {
		cerr << "Can not open sam file: " << argv[1] << endl;
		exit (-1);
	}
	if (fileTrue.openFile(argv[2])==false) {
		cerr << "Can not open true file: " << argv[2] << endl;
		exit (-1);
	}
	if (argc >=4 && strcmp(argv[3], "-i")==0) {
	// ignore the diploid position, treat it as haploid position.
		ignoreDiploid = true;
	}
	// process sam file
	fileSam.readline();
	while (fileSam.endofFile()==false){
		string readName = fileSam.field[0];
		string chrName = fileSam.field[2];
		int pos = atoi(fileSam.field[3].c_str());
		size_t found = chrName.find('.');
		if (found != string::npos) {
			// it is a stripped diploid chromosome
			pos += atoi(chrName.substr(found+1).c_str()) -1;
			chrName = chrName.substr(0, found);
		}
		if (ignoreDiploid) {
			found = chrName.find('b');// remove all 'b' in the chr name
			while (found != string::npos){
				chrName.replace(found,1,"");
				found = chrName.find('b');
			}
		}
		stringstream poss;
		poss << pos;
		samInfo_t samInfo;
		samInfo.chrPos = "<"+chrName+":"+poss.str()+">";
		samInfo.strand = (atoi(fileSam.field[1].c_str())&0x10)==0 ? '+' : '-';
		samInfo.quality = atoi(fileSam.field[4].c_str());
		samMap[readName] = samInfo;
		fileSam.readline();
	}
	// process the true file
	fileTrue.readline();
	while (fileTrue.endofFile()==false){
		size_t found = fileTrue.line.find("<");
		if (found != string::npos) {
			string chrPos = fileTrue.line.substr(found);
			if (ignoreDiploid) {
				found = chrPos.find('b');// remove all 'b' in the chr name
				while (found != string::npos){
					chrPos.replace(found,1,"");
					found = chrPos.find('b');
				}
			}
			trueMap[fileTrue.field[0]] = chrPos;
		}
		fileTrue.readline();// skip the sequence line
		fileTrue.readline();
	}

	int matchCount = 0;
	int mismatchCount = 0;
	int snpMatchCount = 0;
	int snpMismatchCount = 0;
	int nsnpMatchCount = 0;
	int nsnpMismatchCount = 0;

	for (map<string, samInfo_t>::iterator  it= samMap.begin(); it!= samMap.end(); it++) {
		string readName = (*it).first;
		samInfo_t samInfo = (*it).second;
		string trueChrPos = trueMap[readName];
		size_t found = trueChrPos.find(samInfo.chrPos);
		cout << readName << "\t" << samInfo.chrPos << samInfo.strand<<samInfo.quality<<"\t"<< trueChrPos << "\t";
		if (found != string::npos) {
			cout << "true" << endl;
			matchCount ++;
			if (trueChrPos.find("><")==string::npos)
				snpMatchCount++;
			else
				nsnpMatchCount++;
		} else {
			cout << "false" << endl;
			mismatchCount ++;
			if (trueChrPos.find("><")==string::npos)
				snpMismatchCount++;
			else
				nsnpMismatchCount++;
		}
	}
	cout << "match count:" << matchCount << "(" <<  100*(float)matchCount/(matchCount+mismatchCount)<< "%)"
		<< "    mismatch count:" << mismatchCount << "(" <<  100*(float)mismatchCount/(matchCount+mismatchCount)<< "%)"
		<< "    total count:" << matchCount+mismatchCount << endl;

	cout << "snp match count:" << snpMatchCount << "(" <<  100*(float)snpMatchCount/(snpMatchCount+snpMismatchCount)<< "%)"
		<< "    snp mismatch count:" << snpMismatchCount << "(" <<  100*(float)snpMismatchCount/(snpMatchCount+snpMismatchCount)<< "%)"
		<< "    snp total count:" << snpMatchCount+snpMismatchCount << endl;

	cout << "non-snp match count:" << nsnpMatchCount << "(" <<  100*(float)nsnpMatchCount/(nsnpMatchCount+nsnpMismatchCount)<< "%)"
		<< "    non-snp mismatch count:" << nsnpMismatchCount << "(" <<  100*(float)nsnpMismatchCount/(nsnpMatchCount+nsnpMismatchCount)<< "%)"
		<< "    non-snp total count:" << nsnpMatchCount+nsnpMismatchCount << endl;
	fileSam.fp->close();
	fileTrue.fp->close();
}


