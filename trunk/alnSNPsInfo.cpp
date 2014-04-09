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
#include "CLineFields.h"

typedef struct __BaseFreq_t {
	int A, C, G, T;
	bool care;
} BaseFreq_t;

// return the max value among a, b and c
int max3(int a, int b, int c)
{
	int m = a;
	if (b>m)
		m = b;
	if (c>m)
		m = c;
	return m;
}

bool exonOnly = false;
int minSnp = 0;
int alnSamFile = false;

inline int baseCount(BaseFreq_t & loci, char base)
{
	int mapCount = 0;
	switch (base) {
	case 'A':	case 'a':
		mapCount += loci.A;
		break;
	case 'C':	case 'c':
		mapCount += loci.C;
		break;
	case 'G':	case 'g':
		mapCount += loci.G;
		break;
	case 'T':	case 't':
		mapCount += loci.T;
		break;
	}
	return mapCount;
}

// return true if parameters are right.
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-exon", 5)==0) {
			exonOnly = true;
		} else if (strncmp(argv[i], "-mintotal", 9)==0) {
			minSnp = atoi(&argv[i][9]);
		} else if (strncmp(argv[i], "-sam", 4)==0) {
			alnSamFile = true;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	map <string, string> refChrSeq;
	map <string, BaseFreq_t> loci;
	string chrPos;
	int seqLength = 0;
	int chrColumn = 1, posColumn=2, seqColumn=14; //default values match *.map.txt file

	if (argc < 4) {
		cerr << "this program shows SNPs information for the aligned reads" << endl;
		cerr << "usage: " << argv[0] <<" aligned  reference snps  [-sam] [-exon] [min_map_number]" << endl;
		cerr << "example: " << argv[0] <<" aln.txt  hg18.fa  hapmap.txt  -exon -mintotal21 > snpInfo.txt" << endl;
		exit(-1);
	}
	CLineFields fileAln, fileFa, fileSnp;
	if (fileAln.openFile(argv[1])==false) {
		cerr << "Can not open aligned file: " << argv[1] << endl;
		exit (-1);
	}
	if (fileFa.openFile(argv[2])==false) {
		cerr << "Can not open reference file: " << argv[2] << endl;
		exit (-1);
	}
	if (fileSnp.openFile(argv[3])==false) {
		cerr << "Can not open SNPs definition file: " << argv[3] << endl;
		exit (-1);
	}
	parseParameters(argc, argv);
	if (alnSamFile) {
		 chrColumn = 2;
		 posColumn=3;
		 seqColumn=9;
	}
	// get the seq length
	fileAln.readline();
	seqLength =  fileAln.field[seqColumn].length();
	// try to find the start position of all reads that cover SNPs.
	fileSnp.readline();
	while (fileSnp.endofFile()==false) {
		int start, end;
		end = atoi(fileSnp.field[2].c_str());
		start = end -seqLength + 1;
		if (start < 1)
			start = 1;
		for (int i=start; i<=end; i++) {
			stringstream poss;
			poss << i;
			chrPos =  fileSnp.field[1]+"." + poss.str();
			loci[chrPos].care = true; //this is a start position of the reads we really care about.
		}
		fileSnp.readline();
	}
	// process the aligned  file
	fileAln.seek(0);
	fileAln.readline();
	while (fileAln.endofFile()==false) {
		string chrOld = fileAln.field[chrColumn];
		string chr = chrOld[chrOld.length()-1]=='b'?  chrOld.substr(0, chrOld.length()-1): chrOld; // eg. "chr4b" -> "chr4"
		if (loci[ chr + "." + fileAln.field[posColumn]].care) {
			// we care about this read, because according to its start position and length, it will cover a SNP.
			int pos = atoi(fileAln.field[posColumn].c_str());
			string seq = fileAln.field[seqColumn];
			for (int i=0; i<seqLength; i++) {
				stringstream poss;
				poss << pos++;
				chrPos = chr + "."+poss.str();
				switch (seq[i]) {
				case 'A':	case 'a':
					loci[chrPos].A++;
					break;
				case 'C':	case 'c':
					loci[chrPos].C++;
					break;
				case 'G':	case 'g':
					loci[chrPos].G++;
					break;
				case 'T':	case 't':
					loci[chrPos].T++;
					break;
				}
			}
		}
		fileAln.readline();
	}
	// process the reference genome file.
	fileFa.readline();
	string chrName = "";
	while (fileFa.endofFile()==false) {
		if (fileFa.line[0]=='>') {
			chrName = fileFa.line.substr(1);
			refChrSeq[chrName] = "";
		} else {
			refChrSeq[chrName] += fileFa.line;
		}
		fileFa.readline();
	}
	// output file header
	cout << "@rs#	Chr	Position	Ref	NonRef	A#	C#	G#	T#	Ref#	NonRef#" << endl;
	// output result according to the SNPs file.
	int ACount=0, CCount=0, GCount=0, TCount=0, RefCount=0, NonRefCount=0;
	fileSnp.seek(0);
	fileSnp.readline();
	while (fileSnp.endofFile()==false) {
		chrPos = fileSnp.field[1]+"." + fileSnp.field[2];
		int mapCount = 0;
		mapCount += baseCount(loci[chrPos], fileSnp.field[3][0]);
		mapCount += baseCount(loci[chrPos], fileSnp.field[4][0]);

		if (mapCount>=minSnp) {
			int iRef=0, iNonRef=0; // Ref and NonRef count
			char Ref, NonRef;
			int pos = atoi(fileSnp.field[2].c_str()); // 1-based position
			if (fileSnp.field[4][0]==toupper(refChrSeq[fileSnp.field[1]][pos-1])) {
				// for example: (base1, base2, ref) = ('A', 'T', 't'),  then (Ref, NonRef) = ('T', 'A')
				Ref = fileSnp.field[4][0];
				NonRef = fileSnp.field[3][0];
			} else {
				// for example: (base1, base2, ref) = ('A', 'T', 'A'), then  (Ref, NonRef) = ('A', 'T')
				// another example: (base1, base2, ref) = ('A', 'T', 'G'), then  (Ref, NonRef) = ('A', 'T')
				Ref = fileSnp.field[3][0];
				NonRef = fileSnp.field[4][0];
			}
			if (!exonOnly || refChrSeq[fileSnp.field[1]][pos-1]<'a') {// it is in exon
				iRef = baseCount(loci[chrPos], Ref);
				iNonRef = baseCount(loci[chrPos], NonRef);
				cout << fileSnp.field[0] << "\t" << fileSnp.field[1] << "\t" << fileSnp.field[2] << "\t"<< Ref << "\t"<< NonRef << "\t" ;
				cout << loci[chrPos].A<< "\t"<< loci[chrPos].C<< "\t"<< loci[chrPos].G<< "\t"<< loci[chrPos].T<< "\t";
				ACount += loci[chrPos].A;
				CCount += loci[chrPos].C;
				GCount += loci[chrPos].G;
				TCount += loci[chrPos].T;
				cout << iRef << "\t" << iNonRef << "\t" << endl;
				RefCount += iRef;
				NonRefCount += iNonRef;
			}
		}
		fileSnp.readline();
	}
	cout << "@total:"<<"\tA:"<<ACount<<"\tC:"<<CCount<<"\tG:"<<GCount<<"\tT:"<<TCount<<"\tRef:"<<RefCount<<"\tNonRef:"<<NonRefCount<<endl;
	//fileAln.closeFile();
	//fileFa.closeFile();
	//fileSnp.closeFile();
	return 0;
}

