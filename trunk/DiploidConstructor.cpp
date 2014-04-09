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
//#include <my_global.h>
//#include <mysql.h>
#include "CLineFields.h"

#define LINE_LENGTH 60

#define STUFFING "NNNN"

int readLength = 0;
int contexLength = 0;
map<string, string> chrMap; // a map from chr name to sequence string
vector<string> chrNameList; // use this list keep the order of chromosomes
ofstream dipMapFile;


// print out the haploid chromosome sequence
int printHaploid(string chrName)
{
	static string newLine = "";
	assert(chrName[chrName.length()-1] != 'b');
	string value = chrMap[chrName];
	cout << newLine << ">" << chrName;
	newLine = "\n";
	for (int i=0, length=value.length(); i<length; i++) {
		if (i%LINE_LENGTH == 0)
			cout << newLine;
		cout << value[i];
	}
	return 0;
}

string int2string(const char *format, int i)
{
	char buf[100];
	sprintf(buf, format, i);
	return string(buf);	
}
// print out the diploid chromosome sequence
int printDiploid(string chrNameDi)
{
	assert(chrNameDi[chrNameDi.length()-1] == 'b');
	string valueDi = chrMap[chrNameDi];
	string chrNameHa = chrNameDi.substr(0, chrNameDi.length()-1);
	string valueHa = chrMap[chrNameHa];
//	int preSNP = -1;  // so that the new SNP will not be too close to previous SNP.
	int counter = 0;
	int miniCount = 0;
	for (int i=0, chrLength=valueDi.length(); i<chrLength; i++) {
		
		if (valueDi[i] != valueHa[i]) {

			int start;// end;
			vector<string> miniChrome;
			// the first half we just use the string from reference allele and non-ref allele
			start = i-readLength+1<0?0:i-readLength+1;
			//miniChrome.push_back(valueHa.substr(start, i-start /* readLength-1 */));
			////miniChrome.push_back(valueDi.substr(start, readLength));
			miniChrome.push_back(""); //kick off the combinations.
			for (int j=start; j<i+readLength && j<chrLength; j++) {
				miniCount = miniChrome.size();
				if (valueDi[j] != valueHa[j]) {
					// another difference between ha and di doubles the possible combinations.
					//for (int k=0;
					//miniChrome.insert(miniChrome.end(), miniChrome.begin(), miniChrome.end());//double the vector
					for (int k=0; k<miniCount; k++) {
						miniChrome.push_back(miniChrome[k]);
						miniChrome[k] = miniChrome[k] + valueHa[j]; // append the base from ref allele
						miniChrome[k+miniCount] = miniChrome[k+miniCount] + valueDi[j];  // append the base from non-ref allele
					}
				} else {
					// no SNP, just append it to all mini chromosomes.
					for (int k=0; k<miniCount; k++) {
						//miniChrome.push_back(miniChrome[k]);
						miniChrome[k] = miniChrome[k] + valueHa[j]; // append the base from ref allele
					//	miniChrome[k+miniCount] = miniChrome[k+miniCount] + valueDi[j];  // append the base from non-ref allele
					}
				}
			}
			miniCount = miniChrome.size();
			if (miniCount>30)
				miniCount = 30;
			// start from 1 because 0 is the reference allele
			for (int j=1; j<miniCount; j++) {
				cout << endl <<  ">" << chrNameHa << (char)('a'+j) << "." << start+1;  //output 1-based coordinate.
				counter = 0;
				for (int k=0, minilength=miniChrome[j].length(); k<minilength; k++) {
					if (counter%LINE_LENGTH == 0) {
						counter = 0;
						cout << endl;
					}
					cout << miniChrome[j][k];
					counter ++;
				}
				//for (int k=0; k<readLength; k++)
				//	cout << "N"; // output some 'N's so that mini chromosomes won't be concatenated by bwa
			}


/*	
			if (preSNP>=0 &&  i-preSNP<readLength) {
				//this SNP is so close to previous one, merge these two pieces of seq now.
				start = preSNP + readLength;
				end = i + readLength -1;
			} else {
				start = i-contexLength/2;
				end = start+contexLength-1;
				counter = 0;
			}
			// confine the start and end inside the valid range.
			if (start < 0) {
				start = 0;
				end = start + contexLength;
			} else if (end >= (int)valueDi.length()) {
				end = valueDi.length()-1;
				start = end - contexLength;
			}
			if (counter == 0) // new piece of sequence.
				cout << endl <<  ">" << chrNameDi << "." << start+1;  //output 1-based coordinate.
			assert(end-start+1>=0);
			string subValueDi = valueDi.substr(start, end-start+1);
			for (int k=0, length=subValueDi.length(); k<length; k++) {
				if (counter%LINE_LENGTH == 0)
					cout << endl;
				cout << subValueDi[k];
				counter ++;
			}
			preSNP = i;*/
		}
	}
	return 0;
}

int extendFromDiploid(string chrNameDi)
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
				dipMapFile << chrNameHa << "\t" << chrMap[chrNameHa].length()+1 << "\t" << miniChrome[j].length() << "\t";
				chrMap[chrNameHa] += miniChrome[j];
				chrMap[chrNameHa] += stuffing;
				dipMapFile << start+1 << endl;
			}
		}

	}
	return 0;
}


int main(int argc, char *argv[])
{
	CLineFields file;
	if (argc != 3) {
		cerr << "usage: " << argv[0] <<" diploid.fa readLength > diploid.strip.fa" << endl;
		cerr << "example: " << argv[0] <<" diploid.fa 35 > diploid.strip.fa   # 35-mer reads" << endl;
		exit(-1);
	}
	//CLineFields file;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	//contexLength = atoi(argv[2]);
	//readLength = (contexLength+1)/2;
	readLength =  atoi(argv[2]);
	contexLength = 2*readLength - 1;
	file.readline();
	string chrName = "";
	dipMapFile.open("dipMapFile.txt");
	while (file.endofFile()==false){
		if (file.line[0]=='>') {
			chrName = file.line.substr(1);
			chrMap[chrName] = "";
			chrNameList.push_back(chrName);
		} else {
			if (chrName.length()>0)
				chrMap[chrName] += file.line;
		}
		file.readline();
	}

	// create mini chromosomes and append them to the end of reference allele.
	for (int i=0; i<(int)chrNameList.size(); i++) {
		string chrName = chrNameList[i];		
		if (chrName[chrName.length()-1] == 'b')
			extendFromDiploid(chrName);
	}

	// print out the reference allele, which has been extended by mini chromosomes.
	for (int i=0; i<(int)chrNameList.size(); i++) {
		string chrName = chrNameList[i];		
		if (chrName[chrName.length()-1] != 'b')
			printHaploid(chrName);
	}
	cout << endl;
	file.fp->close();
	dipMapFile.close();
}


