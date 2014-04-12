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
#include <unistd.h>

#include "CLineFields1.h"

#define MAX_FILE_NAME_LENGTH 256
#define LINE_LENGTH 60
#define STUFFING "NNNN"

char inputReferenceGenomeFileName[MAX_FILE_NAME_LENGTH]="";
char outputReferenceGenomeFileName[MAX_FILE_NAME_LENGTH]="";
char dipmapFileName[MAX_FILE_NAME_LENGTH]="";
char genotypesFileName[MAX_FILE_NAME_LENGTH]="";
int readLength = 0;
int maxDeletion = 0;
char gender='m';
ofstream dipmapFile;
ofstream outputReferenceGenomeFile;
int contexLength = 0;
map<string, string> chrMap; // a map from chr name to sequence string
vector<string> chrNameList; // use this list keep the order of chromosomes

// return true if parameters are right.
static bool parseParameters (int argc, char *argv[])
{
	int c;
	while ((c = getopt (argc, argv, "r:g:o:l:d:s:")) != -1) {
		switch(c) {
			case 'r':
				strcpy(inputReferenceGenomeFileName, optarg);
				break;
			case 'g':
				strcpy(genotypesFileName, optarg);
				break;
			case 'o':
				strcpy(outputReferenceGenomeFileName, optarg);
				strcpy(dipmapFileName, optarg);
				strcat(dipmapFileName, ".dipmap");
				break;
			case 'l':
				readLength=atoi(optarg);
				break;
			case 'd':
				maxDeletion=atoi(optarg);
				break;
			case 's':
				gender=*optarg;
				break;
			case '?':
				return false;
				break;
		}
	}  
	if (strlen(inputReferenceGenomeFileName)==0 || strlen(genotypesFileName)==0 || strlen(outputReferenceGenomeFileName)==0 || readLength==0)
		return false;
	return true;
}

// print out the haploid chromosome sequence
static int printHaploid(string chrName)
{
	static string newLine = "";
	assert(chrName[chrName.length()-1] != 'b');
	string value = chrMap[chrName];
	outputReferenceGenomeFile << newLine << ">" << chrName;
	newLine = "\n";
	for (int i=0, length=value.length(); i<length; i++) {
		if (i%LINE_LENGTH == 0)
			outputReferenceGenomeFile << newLine;
		outputReferenceGenomeFile << value[i];
	}
	return 0;
}

static int extendFromDiploid(string chrNameDi)
{
	assert(chrNameDi[chrNameDi.length()-1] == 'b');
	string stuffing = STUFFING;
	string valueDi = chrMap[chrNameDi];
	string chrNameHa = chrNameDi.substr(0, chrNameDi.length()-1);
	string valueHa = chrMap[chrNameHa];
	int miniCount = 0;
	for (int i=0, chrLength=valueDi.length(); i<chrLength; i++) {
		
		if (valueDi[i] != valueHa[i]) {
			int start; // 0-based
			vector<string> miniChrome;
			start = i-readLength+1<0?0:i-readLength+1;
			// the first half we just use the string from reference allele and non-ref allele
			miniChrome.push_back(valueHa.substr(start, i-start));
			for (int j=i; j<i+readLength && j<chrLength; j++) {
				miniCount = miniChrome.size();
				if (valueDi[j] != valueHa[j]) {
					// another difference between ha and di doubles the possible combinations.
					for (int k=0; k<miniCount; k++) {
						miniChrome.push_back(miniChrome[k]);
						miniChrome[k] = miniChrome[k] + valueHa[j]; // append the base from ref allele
						miniChrome[k+miniCount] = miniChrome[k+miniCount] + valueDi[j];  // append the base from non-ref allele
					}
				} else {
					// no SNP, just append it to all mini chromosomes.
					for (int k=0; k<miniCount; k++) {
						miniChrome[k] = miniChrome[k] + valueHa[j]; // append the base from ref allele
					}
				}
			}
			miniCount = miniChrome.size();
			// start from 1 because 0 is the reference allele
			for (int j=1; j<miniCount; j++) {				
				dipmapFile << chrNameHa << "\t" << chrMap[chrNameHa].length()+1 << "\t" << miniChrome[j].length() << "\t";
				chrMap[chrNameHa] += miniChrome[j];
				chrMap[chrNameHa] += stuffing;
				dipmapFile << start+1 << endl;
			}
		}

	}
	return 0;
}



int main(int argc, char *argv[])
{	
	string chrName = "", chrNameB = "";
	CLineFields inputReferenceGenomeFile;
	CLineFields genotypesFile;

	if (parseParameters (argc, argv) == false) 	{
		cerr << "Version: 1.0-r7" << endl;
		cerr << "Contact: shuaiyuan.emory@gmail.com" << endl << endl;		
		cerr << "Usage: " << argv[0] <<" -r haploid.fa -g genotypes -l length -o diploid.fa" << endl << endl;
		cerr << "Convert haploid reference genome to diploid reference genome according to the genotypes and sequencing read length" << endl << endl;
		cerr << "Example: " << argv[0] << " -r hg19.fa -g known.genotypes -l 36 -o diploid.fa" << endl;
		cerr << endl << "Parameters (mandatory): " << endl;
		cerr << "  -r\t\thaploid reference genome file " << endl;
		cerr << "  -g\t\tgenotypes file" << endl;
		cerr << "  -l\t\tread length" << endl;
		cerr << "  -o\t\toutput diploid reference genome file " << endl;
		cerr << endl << "Parameters (optional): " << endl;
		cerr << "  -d\t\tmaximal deletion in a read and mapped to alternative alleles [default=0]" << endl;
		cerr << "  -s\t\tsex of the individual [default=\"m\"]" << endl;
		exit(-1);
	}
	
	if (inputReferenceGenomeFile.openFile(inputReferenceGenomeFileName)==false) {
		cerr << "Can not open file: " << inputReferenceGenomeFileName << endl;
		exit (-1);
	}

	if (genotypesFile.openFile(genotypesFileName)==false) {
		cerr << "Can not open file: " << genotypesFileName << endl;
		exit (-1);
	}	

	// step 1: read haploid reference genome file, double the total number of chromosomes.
	bool readin = false;
	inputReferenceGenomeFile.readline();
	while (inputReferenceGenomeFile.endofFile()==false) {
		if (inputReferenceGenomeFile.line[0]=='>') {
			chrName = inputReferenceGenomeFile.line.substr(1);
			chrNameB = chrName+"b";
			if (chrName != "chrY" || gender=='m') {
				chrMap[chrName] = "";
				chrMap[chrNameB] = "";
				chrNameList.push_back(chrName);			
				chrNameList.push_back(chrNameB);
				readin = true;
			} else {
				readin = false;  // skip chromosome Y for female individuals.
			}
		} else if (readin){
			
			chrMap[chrName] += inputReferenceGenomeFile.line;
			chrMap[chrNameB] += inputReferenceGenomeFile.line;
		}
		inputReferenceGenomeFile.readline();
	}	
	inputReferenceGenomeFile.closeFile();

	// step 2: use genotype file to modify the chromosomes
	if (genotypesFile.isOpen()) {
		int refColumn = -1, altColumn=-1;
		genotypesFile.readline();
		while(genotypesFile.endofFile()==false) {
			unsigned int pos = atoi(genotypesFile.field[2].c_str());
			chrName = genotypesFile.field[1];
			chrNameB = chrName+"b";
			
			if (chrMap[chrName].length()<pos) {
				cerr << "Warning!!! genotype is out of range at: " << chrName << ": " << genotypesFile.field[2] << endl;
			} else {
				// decide which is ref and which is non-ref
				if (toupper(chrMap[chrName][pos-1]) == toupper(genotypesFile.field[4][0])) {						
					refColumn = 4;
					altColumn = 3;
				}
				else {
					refColumn = 3;
					altColumn = 4;
				}

				// if genotype is alt/alt, then we still need to change reference allele.
				if (chrMap[chrName][pos-1] >='A' && chrMap[chrName][pos-1] <='Z') {
					chrMap[chrName][pos-1] = toupper(genotypesFile.field[refColumn][0]);
					chrMap[chrNameB][pos-1] = toupper(genotypesFile.field[altColumn][0]);
				}
				else {
					chrMap[chrName][pos-1] = tolower(genotypesFile.field[refColumn][0]);
					chrMap[chrNameB][pos-1] = tolower(genotypesFile.field[altColumn][0]);
				}
			}
			genotypesFile.readline();
		}
		genotypesFile.closeFile();
	} 
	contexLength = 2*readLength - 1 + 2*maxDeletion;
	outputReferenceGenomeFile.open(outputReferenceGenomeFileName);
	dipmapFile.open(dipmapFileName);

	// step 3: create mini chromosomes and append them to the end of reference allele.
	for (int i=0; i<(int)chrNameList.size(); i++) {
		chrName = chrNameList[i];		
		if (chrName[chrName.length()-1] == 'b')
			extendFromDiploid(chrName);
	}

	// step 4: print out the reference allele, which has already been extended by mini chromosomes.
	for (int i=0; i<(int)chrNameList.size(); i++) {
		chrName = chrNameList[i];		
		if (chrName[chrName.length()-1] != 'b')
			printHaploid(chrName);
	}
	outputReferenceGenomeFile << endl;
	dipmapFile.close();
	outputReferenceGenomeFile.close();
	
	return 0;
}


