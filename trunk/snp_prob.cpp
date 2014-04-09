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
#include "CLineFields.h"

typedef enum __NucleotideBase_e {
	A = 0, C, G, T
} NucleotideBase_e;

typedef struct __BaseFreq_t {
	long A, C, G, T;
	bool care;
} BaseFreq_t;


long alnSamFile = false;

long factorialTable[2000];

double probTable[T+1][T+1];
double errRate = 0;

#define DIVISOR_TABLE_SIZE 2000

//typedef struct __DivisorTable_t {
//	long count[2000];
//} DivisorTable_t;

class DivisorTable
{
public:
	long count[DIVISOR_TABLE_SIZE];
	DivisorTable();
	void decompose(long n);
	void decomposeMore(long n);
	void decomposeFactorial(long n);
	DivisorTable operator+ (DivisorTable const& other); 
	DivisorTable operator- (DivisorTable const& other); 
	long product();
	void reset();
};

void DivisorTable::reset()
{
	for (long i=0; i<DIVISOR_TABLE_SIZE; i++)
		count[i] = 0;
}

DivisorTable::DivisorTable()
{
	reset();
}


// return true if parameters are right.
bool parseParameters (long argc, char *argv[])
{
	for (long i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-sam", 4)==0) {
			alnSamFile = true;
		}
	}
	return true;
}



// return n!
long factorial(long n)
{
	if (n==0)
		return 1;
	if (factorialTable[n] == 0)
		factorialTable[n] = factorial(n-1)*n;
	return factorialTable[n];
}

long DivisorTable::product()
{
	long result = 1;
	for (long i=0; i<DIVISOR_TABLE_SIZE; i++) {
		result *= (long)pow(i, count[i]);
	}
	return result;
}

void DivisorTable::decompose(long n)
{
	//assert(n>0);
	reset();
	decomposeMore(n);
}

void DivisorTable::decomposeMore(long n)
{
	if (n==0) 
		return; // 0!==1
	long a = 2;
	while (n>1) {
		if (n%a == 0) { // a is a divisor
			n = n/a;
			count[a]++;
		} else {
			a ++;
		}
	}	
}
// decompose n!
void DivisorTable::decomposeFactorial(long n)
{
	reset();
	for (long i=2; i<=n; i++)
		decomposeMore(i);
}

DivisorTable DivisorTable::operator+ (DivisorTable const& other)
{
	DivisorTable result = *this; // make a copy of current object
	for (long i=0; i<DIVISOR_TABLE_SIZE; i++) {
		result.count[i] += other.count[i];
	}
	return result;	
}

DivisorTable DivisorTable::operator- (DivisorTable const& other)
{
	DivisorTable result = *this; // make a copy of current object
	for (long i=0; i<DIVISOR_TABLE_SIZE; i++) {
		result.count[i] -= other.count[i];
	}
	return result;	
}


// given the count of A, C, G and T, compute the number of combinations=factor(A+C+G+T)/(factor(A)*factor(C)*factor(G)*factor(T))
// for example, if countA=2 and countC=1, then return 3!/(2!*1!)=3 because the combinations are:
// (AAC) (ACA) (CAA)
long combination (long countA, long countC, long countG, long countT)
{
	DivisorTable divident; // factor(A+C+G+T)
	DivisorTable divisorA, divisorC, divisorG, divisorT; // factor(A), factor(C), factor(G), factor(T) 
	long total = countA+countC+countG+countT;
	divident.decomposeFactorial(total);
	divisorA.decomposeFactorial(countA);
	divisorC.decomposeFactorial(countC);
	divisorG.decomposeFactorial(countG);
	divisorT.decomposeFactorial(countT);
	divident = divident - divisorA - divisorC -divisorG - divisorT;
	return divident.product();
}


//    Given the reference from two alleles and error rate, compute the likelihood of the observation of
//    ref1 is on reference allele, ref2 is on alternative allele.
double likelihood (NucleotideBase_e ref1, NucleotideBase_e ref2, long countA, long countC, long countG, long countT)
{
	double pA = pow((probTable[ref1][A]+probTable[ref2][A])/2, countA);
	double pC = pow((probTable[ref1][C]+probTable[ref2][C])/2, countC);
	double pG = pow((probTable[ref1][G]+probTable[ref2][G])/2, countG);
	double pT = pow((probTable[ref1][T]+probTable[ref2][T])/2, countT);
	long n = combination(countA,countC,countG,countT);
	return pA*pC*pG*pT*n;
}

double initProbTable(void)
{
	for (NucleotideBase_e ref=A; ref<=T; ref=(NucleotideBase_e)(ref+1)) {
		for (NucleotideBase_e read=A; read<=T; read=(NucleotideBase_e)(read+1)) {
			if (ref == read) {
				probTable[ref][read] = 1- errRate;
			} else {
				probTable[ref][read] =  errRate/3;
			}
				
		}
	}
	return 0;
}



int main(int argc, char *argv[])
{
	if (argc<5) {
		cerr<< "Usage: " << argv[0] << " countA  countC  countG  countT  [errRate]" << endl;
		cerr << "Example: "  << argv[0] << " 2 1 0 0" << endl;
		exit(-1);
	}
	errRate = 0;
	memset(factorialTable, 0, sizeof(factorialTable));
	initProbTable();
	long countA = strtol(argv[1], NULL, 10);
	long countC = strtol(argv[2], NULL, 10);
	long countG = strtol(argv[3], NULL, 10);
	long countT = strtol(argv[4], NULL, 10);
	if (argc >= 6)
		errRate = strtod(argv[5], NULL);
	double t = likelihood(A, C, countA, countC, countG, countT);
	cout << "t=" << t << endl;
	DivisorTable a;
	for (long i=5; i<=25; i++) {
		a.decomposeFactorial(i);
		cout << a.product() << endl;
	}

	
	//long long int cc;
	//cout << sizeof(cc) << endl;
	return 0;
}


int main2(int argc, char *argv[])
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
		while (fileFa.endofFile()==false){
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


