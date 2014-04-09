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
#include "CLineFields1.h"

bool with_header = true;

int main(int argc, char *argv[])
{
	map <string, string> refChrSeq;
	//string chrPos;
	if (argc < 3) {
		cerr << "this program extends the information in the genotype file" << endl;
		cerr << "usage: " << argv[0] <<" reference genotype [-h]" << endl;
		cerr << "example: " << argv[0] <<" hg18.fa  genotype  > genotype.ext" << endl;
		exit(-1);
	}
	CLineFields fileFa, fileSnp;
	if (fileFa.openFile(argv[1])==false) {
		cerr << "Can not open reference file: " << argv[1] << endl;
		exit (-1);
	}
	if (fileSnp.openFile(argv[2])==false) {
		cerr << "Can not open SNPs definition file: " << argv[2] << endl;
		exit (-1);
	}
	if ((argc >= 4) && (strcmp(argv[3],"-h")==0))
		with_header = false;
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
	// genotype ext header
	if (with_header)
		cout << "@rs#	Chr	Position	Ref	NonRef	intron/exon	Ref_in_Fasta" << endl;
	fileSnp.readline();
	while (fileSnp.endofFile()==false) {
		//chrPos = fileSnp.field[1]+"." + fileSnp.field[2];
		char Ref, NonRef, faRef;
		int pos = atoi(fileSnp.field[2].c_str()); // 1-based position
		if ((int)refChrSeq[fileSnp.field[1]].length()>=pos) {
			faRef = toupper(refChrSeq[fileSnp.field[1]][pos-1]);
			if (fileSnp.field[4][0]==faRef) {
				// for example: (base1, base2, ref) = ('A', 'T', 't'),  then (Ref, NonRef) = ('T', 'A')
				Ref = fileSnp.field[4][0];
				NonRef = fileSnp.field[3][0];
			} else {
				// for example: (base1, base2, ref) = ('A', 'T', 'A'), then  (Ref, NonRef) = ('A', 'T')
				// another example: (base1, base2, ref) = ('A', 'T', 'G'), then  (Ref, NonRef) = ('A', 'T')
				Ref = fileSnp.field[3][0];
				NonRef = fileSnp.field[4][0];
			}
			cout << fileSnp.field[0] << "\t" << fileSnp.field[1] << "\t" << fileSnp.field[2] << "\t"<< Ref << "\t"<< NonRef << "\t" ;
			if (refChrSeq[fileSnp.field[1]][pos-1]<'a') // this is capital letter, so it is in exon
				cout << "exon\t" << faRef << endl;
			else
				cout << "intron\t" << faRef << endl;
		} else {
			cerr << "warning!!! SNP out of range at: " << fileSnp.field[1] <<" " << fileSnp.field[2] << endl;
		}
		fileSnp.readline();
	}
	fileFa.closeFile();
	fileSnp.closeFile();
}

