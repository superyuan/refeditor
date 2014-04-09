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


int alnSamFile = false;

// return true if parameters are right.
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-sam", 4)==0) {
			alnSamFile = true;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	map <string, int> chrList;
	map <string, string> refChrSeq;
	//map <string, BaseFreq_t> loci;
	string chrPos;
	int seqLength = 0;
	int chrColumn = 1, posColumn=2, seqColumn=14; //default values match *.map.txt file
	map<string, string> chrMap; // a map from chr name to sequence string

	if (argc < 2) {
		cerr << "this program does similar as pileup command in samtools. it handles *.map or *.sam format reads" << endl;
		cerr << "usage: " << argv[0] <<" aligned ref [-sam]" << endl;
		cerr << "example: " << argv[0] <<" aln.txt hg18.fa -sam > snpInfo.txt" << endl;
		exit(-1);
	}
	CLineFields fileAln, fileFa;//, fileSnp;
	if (fileAln.openFile(argv[1])==false) {
		cerr << "Can not open aligned file: " << argv[1] << endl;
		exit (-1);
	}
	if (argc>=3 && strncmp(argv[2], "-sam", 4)!=0) {
		if (fileFa.openFile(argv[2])==false) {
			cerr << "Can not open ref file: " << argv[2] << endl;
			exit (-1);
		}
		fileFa.readline();
		string chrName = "";
		while (fileFa.endofFile()==false) { // load the fasta file
			if (fileFa.line[0]=='>') {
				chrName = fileFa.line.substr(1);
				chrMap[chrName] = "";
			} else {
				chrMap[chrName] += fileFa.line;
			}
			fileFa.readline();
		}
	}
	parseParameters(argc, argv);
	if (alnSamFile) {
		 chrColumn = 2;
		 posColumn=3;
		 seqColumn=9;
	}
	// get the seq length
	fileAln.readline();
	if (fileAln.field.size() < (unsigned int)seqColumn+1) {
		cerr << "the alignment file " << argv[1] << " is incorrect !" << endl;
		exit (-1);
	}
	seqLength =  fileAln.field[seqColumn].length();
	int id=1;
	cout << "@id#	Chr	Position	Ref	NonRef	A#	C#	G#	T#	Ref#	NonRef#" << endl;
	// process the aligned  file
	chrList["chr1"] = 1;// add the first chr into the list to start following for loop.
	
	for (map<string, int>::iterator  itChrList= chrList.begin(); itChrList!= chrList.end(); itChrList++) {
		map <int, BaseFreq_t> loci;
		fileAln.seek(0);
		fileAln.readline();
		while (fileAln.endofFile()==false) {
			string chrOld = fileAln.field[chrColumn];
			string chr = chrOld[chrOld.length()-1]=='b'?  chrOld.substr(0, chrOld.length()-1): chrOld; // eg. "chr4b" -> "chr4"
			if ((*itChrList).first.compare(chr)==0) { // this is the chr we are handling.
				int pos = atoi(fileAln.field[posColumn].c_str());  // 1-based position
				string seq = fileAln.field[seqColumn];
				for (int i=0; i<seqLength; i++) {
					switch (seq[i]) {
					case 'A':	case 'a':
						loci[pos+i].A++;
						break;
					case 'C':	case 'c':
						loci[pos+i].C++;
						break;
					case 'G':	case 'g':
						loci[pos+i].G++;
						break;
					case 'T':	case 't':
						loci[pos+i].T++;
						break;
					}
				}
			}else {
				chrList[chr] = 1; //add this chr into the chrList.
			}
			fileAln.readline();
		}

		// now print out the result.
		if (fileFa.isOpen()) {
			for (map<int, BaseFreq_t>::iterator  it= loci.begin(); it!= loci.end(); it++) {
				char ref = chrMap[(*itChrList).first][(*it).first-1]; //chrMap is 0-based position
				int refCount = 0;
				switch (ref) {
					case 'A':	case 'a':
						refCount = (*it).second.A;
						break;
					case 'C':	case 'c':
						refCount = (*it).second.C;
						break;
					case 'G':	case 'g':
						refCount = (*it).second.G;
						break;
					case 'T':	case 't':
						refCount = (*it).second.T;
						break;
				}
				cout << id << '\t' << (*itChrList).first << "\t" << (*it).first << "\t" <<  ref <<  "\t-\t" << (*it).second.A << "\t"<< (*it).second.C << "\t"<< (*it).second.G << "\t"<< (*it).second.T << "\t" <<refCount<<"\t-" << endl;;
				id ++;
			}
			//fileFa.closeFile();
		}
		else {
			for (map<int, BaseFreq_t>::iterator  it= loci.begin(); it!= loci.end(); it++) {
				cout << id << '\t' << (*itChrList).first << "\t" << (*it).first << "\t" << "-\t-\t" << (*it).second.A << "\t"<< (*it).second.C << "\t"<< (*it).second.G << "\t"<< (*it).second.T << "\t-\t-" << endl;;
				id ++;
			}
		}
	}

	fileAln.closeFile();
	if (fileFa.isOpen())
		fileFa.closeFile();
	return 0;
}

