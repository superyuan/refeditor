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

class Mutate
{
private:
	char mutationTable[128][4];
public:
	Mutate();
	string genRead(string pattern, float errRate, int insertLength, int deleteLength);
	char genBase(char base, float mutationRate);
};


bool snpOnly = false;
float baseErrRate = -1;
string outFilename = "";
string refFilename = "";
int readNumber_ref = 0, readNumber_nonref = 0;
int readLength = 0;
int insertLength = 0;
int deleteLength = 0;
// return true if parameters are right.
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "-SNP", 4)==0) {
			snpOnly = true;
		} else if (strncmp(argv[i], "-e", 2)==0) {
		    baseErrRate = strtod(argv[i+1], NULL);
		} else if (strncmp(argv[i], "-o", 2)==0) {
			outFilename = string(argv[i+1]);
		} else if (strncmp(argv[i], "-r", 2)==0) {
			refFilename = string(argv[i+1]);
		} else if (strncmp(argv[i], "-n", 2)==0) {
			readNumber_ref = atoi(argv[i+1]);
			readNumber_nonref = atoi(argv[i+2]);
		} else if (strncmp(argv[i], "-l", 2)==0) {
			readLength = atoi(argv[i+1]);
		} else if (strncmp(argv[i], "-I", 2)==0) {
			insertLength = atoi(argv[i+1]);
		} else if (strncmp(argv[i], "-D", 2)==0) {
			deleteLength = atoi(argv[i+1]);
		}
	}
	return true;
}

Mutate::Mutate()
{
	// initialize the mutation table;   A-C-G-T
	mutationTable[(int)'A'][0]='A'; mutationTable[(int)'A'][1]='C';	mutationTable[(int)'A'][2]='G'; mutationTable[(int)'A'][3]='T';
	mutationTable[(int)'C'][0]='C'; mutationTable[(int)'C'][1]='G'; mutationTable[(int)'C'][2]='T'; mutationTable[(int)'C'][3]='A';
	mutationTable[(int)'G'][0]='G'; mutationTable[(int)'G'][1]='T'; mutationTable[(int)'G'][2]='A'; mutationTable[(int)'G'][3]='C';
	mutationTable[(int)'T'][0]='T'; mutationTable[(int)'T'][1]='A'; mutationTable[(int)'T'][2]='C'; mutationTable[(int)'T'][3]='G';
	mutationTable[(int)'a'][0]='a'; mutationTable[(int)'a'][1]='c';	mutationTable[(int)'a'][2]='g'; mutationTable[(int)'a'][3]='t';
	mutationTable[(int)'c'][0]='c'; mutationTable[(int)'c'][1]='g'; mutationTable[(int)'c'][2]='t'; mutationTable[(int)'c'][3]='a';
	mutationTable[(int)'g'][0]='g'; mutationTable[(int)'g'][1]='t'; mutationTable[(int)'g'][2]='a'; mutationTable[(int)'g'][3]='c';
	mutationTable[(int)'t'][0]='t'; mutationTable[(int)'t'][1]='a'; mutationTable[(int)'t'][2]='c'; mutationTable[(int)'t'][3]='g';
	srand(time(NULL));
}

// mutate a base according to the mutation rate
// this is a random function
char Mutate::genBase(char base, float mutationRate)
{
	if (base == 'N' || base == 'n')
		return base;
	float randomNumber = (float) rand () / RAND_MAX;
	if (randomNumber >= mutationRate) {
		// we don't have mutation
		return base;
	} else {
		int offset = (int)(randomNumber/(mutationRate/3)) + 1;  // offset in [1, 3]
		return mutationTable[(int)base][offset];
	}
}

// generate a read according to pattern read and error rate
// this is a random function
string Mutate::genRead(string pattern, float errRate, int insertLength, int deleteLength)
{
	string ret;
	stringstream Read;
	int length = pattern.length();
	for (int i=0; i<length; i++) {
		Read << genBase(pattern[i], errRate);
	}
	ret = Read.str();
	while (deleteLength > 0) {
		deleteLength --;
		int delete_pos = (int)((length-1) * ((float)rand () / RAND_MAX));
		ret.erase(delete_pos, 1);
		length --;		
	}
	while (insertLength > 0) {
		insertLength --;
		int ins_pos = (int)((length-1) * ((float)rand () / RAND_MAX));
		char ins_base = mutationTable[(int)ret[ins_pos]][(int)(4*(float)rand () / RAND_MAX)];
		ret = ret.substr(0,ins_pos) + ins_base + ret.substr(ins_pos, length-ins_pos-1);
	}
	return ret;
}

void showHelp(int argc, char *argv[])
{
	cerr << "Usage: " << argv[0] <<" -r di.fa.big -l length -n ref_n nonref_n -e error [-SNP] -o out.fq" << endl;
	cerr << "example: " << argv[0] <<" -r hg18.di.fa.big -l 35 -n 1000000 1000000 -e 0.01 -SNP -o test.fq" << endl;
}

int main(int argc, char *argv[])
{
	long long int totalLength = 0;
	map<string, string> chrMap; // a map from chr name to sequence string

	parseParameters(argc, argv);
	if (refFilename=="" ||
		readLength==0 ||
		baseErrRate<0 ||
		outFilename==""
	) {
		showHelp(argc, argv);
		exit(-1);
	}

	CLineFields file;
	// open reference genome file (big diploid).
	if (file.openFile(refFilename.c_str())==false) {
		cerr << "Can not open ref file: " << refFilename << endl;
		exit (-1);
	}
	cout << "reading " << refFilename << endl;
	file.readline();
	string chrName = "";
	while (file.endofFile()==false){
		if (file.line[0]=='>') {
			chrName = file.line.substr(1);
			chrMap[chrName] = "";
		} else {
			chrMap[chrName] += file.line;
			totalLength += file.line.length();
		}
		file.readline();
	}
	if (chrMap.size() > 1000) {
		cerr << "error: you should use unstripped reference genome ! " << endl;
		exit (-1);
	}

	ofstream *ofp = new ofstream(outFilename.c_str());
	ofstream *ofpLog = new ofstream(string(outFilename+".true").c_str());
	if (ofp==NULL) {
		cerr << "Can not open output file: " << outFilename << endl;
		exit (-1);
	}

	string quality;
	if (baseErrRate <0.000000001)
		quality = string(readLength, (char)126);
	else
		quality = string(readLength, (char)(33-10*log10(baseErrRate)));

	// now we start to create reads
	Mutate mutater;
	srand48(time(NULL));
	//int i=0;
	int i_ref=0, i_nonref=0;
	cout << "start to generate reads" << endl;
	while ( i_ref<readNumber_ref || i_nonref<readNumber_nonref) {
		long long int absLocation = (long long int)(drand48()*totalLength); // absolute position ralative to (chr1,pos0)
		long long int previous = 0;
		for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
			if (previous +  (long long int)((*it).second.length()) > absLocation) {
				// now we hit wanted chromosome
				int start = absLocation-previous;
				if ((*it).second[start]=='N') {
					break; // don't generate the "NNNNNNN..." read
				}
				if (start + readLength <= (int)((*it).second.length())) {
					string chrNameHa="", chrNameDi="";
					string seqHa="", seqDi="";
					bool hasSNP = false;
					bool isRef=false;
					if ( (*it).first[(*it).first.length()-1]=='b') {
						isRef=false;
						chrNameDi = chrName = (*it).first;
						chrNameHa = chrNameDi.substr(0, chrNameDi.length()-1);
					} else {
						isRef=true;
						chrNameHa = chrName = (*it).first;
						chrNameDi = chrNameHa + 'b';
					}
					if (chrMap[chrNameHa].empty()==false) {
						seqHa = chrMap[chrNameHa].substr(start, readLength);
					}
					if (chrMap[chrNameDi].empty()==false) {
						seqDi = chrMap[chrNameDi].substr(start, readLength);
					}
					hasSNP = (seqHa.compare(seqDi) != 0);
					if (snpOnly && !hasSNP)
						break;  // don't generate this read because there is no SNP in it.
					if (i_ref>=readNumber_ref && isRef)
						break;
					else if (i_nonref>=readNumber_nonref && isRef==false)
						break;

					*ofp << "@" << chrName << "." <<start+1<<".id" << i_ref+i_nonref << " USI-EAS7_15_3010BAAXX:1:1:130:374 length=" << readLength << endl;
					*ofp <<  mutater.genRead((*it).second.substr(start, readLength+deleteLength), baseErrRate, insertLength, deleteLength)<< endl;
					*ofp << "+" << chrName << "." <<start+1<<".id" << i_ref+i_nonref << " USI-EAS7_15_3010BAAXX:1:1:130:374 length=" << readLength << endl;
					*ofp << quality << endl;
					// generate the *.true file to record where the read is from
					*ofpLog << chrName << "." <<start+1<< ".id" <<  i_ref+i_nonref  << " length=" << readLength <<  " from: ";
					if (!hasSNP) {
						*ofpLog << "<" <<chrNameHa << ":" << start+1 << ">";
						*ofpLog << "<" <<chrNameDi << ":" << start+1 << ">";
					} else {
						*ofpLog << "<" << (*it).first << ":" << start+1 << ">";
					}
					*ofpLog << endl;
					*ofpLog << (*it).second.substr(start, readLength) << endl;
					if (isRef) 
						i_ref++;
					else
						i_nonref++;
					if ((i_ref+i_nonref)%10000 ==0)
						cout << "generated ref:" << i_ref<< "  nonref:" << i_nonref << "  total:" << i_ref+i_nonref<< endl;
					break;  // finished generate one read.
				}
			} else {
				previous +=  (*it).second.length();
			}
		}
	}
	file.fp->close();
	ofp->close();
	ofpLog->close();
}

