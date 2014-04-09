/*
	this program is used to tell the reads are from which allele (reference or non-reference)

*/

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

typedef struct __SNPInfo_t {
	string rs;
	//string chrName;
	int position;
	char refBase;
	char nonRefBase;
} SNPInfo_t;

bool exonOnly = false;
bool planB = false;
bool alnSamFile = false;

// return true if parameters are right.
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-exon", 5)==0) {
			exonOnly = true;
		} else if (strncmp(argv[i], "-planB", 6)==0) {
			planB = true;
		} else if (strncmp(argv[i], "-sam", 4)==0) {
			alnSamFile = true;
		}
	}
	return true;
}

string processIndel(string &s, string &desc)
{
	string ret="";
	string tmp;
	int i=0;
	int p=0;
	while (i<(int)desc.length()) {
		int num = substr2int(desc, i);
		switch (desc[i]) {
			case 'M':
				ret += s.substr(p, num);
				p += num;
				break;
			case 'I':
				p += num;
				break;
			case 'D':
				tmp.assign(num, 'N');
				ret += tmp;
				break;
		}		
		i++;
	}
	// cerr << ret << endl;
	return ret;
}

int main(int argc, char *argv[])
{
	map<string, vector <SNPInfo_t> *> snpMap;
	vector <SNPInfo_t> *snpList;
	int chrColumn = 1, posColumn=2, descColumn=5, seqColumn=14; //default values match *.map.txt file

	if (argc < 3) {
		cerr << "this program shows all reads that mapped to selected SNPs" << endl;
		cerr << "usage: " << argv[0] <<" aligned  snps  [-sam] [-planB] [-exon]" << endl;
		cerr << "example: " << argv[0] <<" aln.txt  selectedSNPs.txt -sam > aln.allele.txt" << endl;
		exit(-1);
	}
	CLineFields fileAln, fileSnp;
	if (fileAln.openFile(argv[1])==false) {
		cerr << "Can not open aligned file: " << argv[1] << endl;
		exit (-1);
	}
	if (fileSnp.openFile(argv[2])==false) {
		cerr << "Can not open SNPs definition file: " << argv[2] << endl;
		exit (-1);
	}
	parseParameters(argc, argv);
	if (alnSamFile) {
		 chrColumn = 2;
		 posColumn=3;
		 seqColumn=9;
	}
	// process the reference genome file.
	string chrName = "";
	// process the SNPs file.
	fileSnp.readline();
	while (fileSnp.endofFile()==false) {
		SNPInfo_t snp;
		snp.rs = fileSnp.field[0];
		//snp.chrName = fileSnp.field[1];
		snp.position = atoi(fileSnp.field[2].c_str());
		snp.refBase = fileSnp.field[3][0]; // this SNP file is processed so we can use column 3 as ref and column 4 as nonref
		snp.nonRefBase = fileSnp.field[4][0];
		if (!exonOnly || snp.refBase==toupper(snp.refBase)) {
			snpList = snpMap[fileSnp.field[1]];
			if (snpList == NULL)
				snpMap[fileSnp.field[1]] = snpList = new vector <SNPInfo_t>;
			(*snpList).push_back(snp);
		}
		fileSnp.readline();
	}
	// process the aligned  file
	fileAln.readline();
	while (fileAln.endofFile()==false) {
		chrName = fileAln.field[chrColumn];
		if (chrName[chrName.length()-1]=='b') // the chr name is like 'chr12b'
			chrName = chrName.substr(0, chrName.length()-1);
		int start = atoi(fileAln.field[posColumn].c_str());
		//string seq = fileAln.field[seqColumn];
		string seq = processIndel(fileAln.field[seqColumn], fileAln.field[descColumn]);
		int end = start + seq.length()-1;
		bool isRef=false, isNonRef=false;
		int mark = -1;
		// go through the SNPs List to see if this read covers any SNP.
		snpList = snpMap[chrName];
		if (snpList != NULL) {
			for (int i=0, size=(*snpList).size(); i<size; i++) {
				if (((*snpList)[i].position>=start)&&((*snpList)[i].position<=end)) {
					// this read covers a SNP in the (*snpList).
					if (toupper(seq[(*snpList)[i].position-start])==toupper((*snpList)[i].refBase)) {
						isRef = true;
						mark = i;
					}
					if (toupper(seq[(*snpList)[i].position-start])==toupper((*snpList)[i].nonRefBase)) {
						isNonRef = true;
						mark = i;
					}
				}
			}
		}
		if (isRef) {
			if (isNonRef)
				cout << fileAln.line << "\t"  << "-\t" <<  "-\t"<<  "-\t"<<  "-\t"<<"-\t"<<"isMixed" << endl;
			else
				cout << fileAln.line << "\t" << (*snpList)[mark].rs << "\t" << (*snpList)[mark].position << "\t"<< (*snpList)[mark].refBase<< "\t"<< (*snpList)[mark].nonRefBase<< "\t"<<seq[(*snpList)[mark].position-start]<<"\t" << "isRef" << endl;
		} else {
			if (isNonRef)
				cout << fileAln.line << "\t" << (*snpList)[mark].rs << "\t" << (*snpList)[mark].position << "\t"<< (*snpList)[mark].refBase<< "\t"<< (*snpList)[mark].nonRefBase<< "\t"<<seq[(*snpList)[mark].position-start]<<"\t" << "isNonRef" << endl;
			else
				cout << fileAln.line << "\t"  << "-\t" <<  "-\t"<<  "-\t"<<  "-\t"<<"-\t"<<"isNone" << endl;
		}
		fileAln.readline();
	}
	fileAln.closeFile();
	fileSnp.closeFile();
}

