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
char inputSamFileName[MAX_FILE_NAME_LENGTH]="";
char outputSamFileName[MAX_FILE_NAME_LENGTH]="";
char dipmapFileName[MAX_FILE_NAME_LENGTH]="";

bool removeB = true;
bool removeHeader = false;
int readLength=0;

class posMap
{
public:
	int from;
	int length;
	int to;
};

map<string, vector<posMap>* > chrMap; // a map from chr name to sequence string

int readMapFile(const char *fileName)
{
	CLineFields mapFile;
    if (mapFile.openFile(fileName)==false) { 
            cerr << "Can not open map file: " << fileName << endl;
            exit (-1);
    }
	mapFile.readline();
	while (mapFile.endofFile()==false) {
		posMap cm;
		string chrName = mapFile.field[0];
		cm.from = atoi(mapFile.field[1].c_str());
		cm.length = atoi(mapFile.field[2].c_str());
		cm.to = atoi(mapFile.field[3].c_str());
		
		if (chrMap[chrName]==NULL) {
			chrMap[chrName] = new vector<posMap>;
		}
		(*chrMap[chrName]).push_back(cm);
		mapFile.readline();
	}
	mapFile.fp->close();
	return 0;
}

// return true if parameters are right.
static bool parseParameters (int argc, char *argv[])
{
	int c;
	while ((c = getopt (argc, argv, "i:m:o:")) != -1) {
		switch(c) {
			case 'i':
				strcpy(inputSamFileName, optarg);
				break;
			case 'm':
				strcpy(dipmapFileName, optarg);
				break;
			case 'o':
				strcpy(outputSamFileName, optarg);
				break;
			case '?':
				return false;
				break;
		}
	}  
	if (strlen(inputSamFileName)==0 || strlen(dipmapFileName)==0 || strlen(outputSamFileName)==0)
		return false;
	return true;
}


int mapExtendedChrome(GenomePosition &gp2)
{
    int target = gp2.pos+readLength/2; //the middle of the read, in case the read overlap with 2 mini chromosomes.
    if (chrMap[gp2.chrName] != NULL)
	{
		vector<posMap> *v = chrMap[gp2.chrName];
		int min=0;
		int max=v->size()-1;
		int middle = (min+max+1)/2;

		while(min != max) {
			if ((*v)[middle].from <= target) {
				min=middle;
			} else {
				max=middle-1;
			}
			middle = (min+max+1)/2;
		}
		int offset = 0;
		if ( (*v)[min].from <= target)
			offset = (*v)[min].from - (*v)[min].to;
		gp2.pos -= offset;
	}
    return 0;
}

int main(int argc, char *argv[])
{
	if (parseParameters (argc, argv) == false) 	{
		cerr << "Version: 1.0-r7" << endl;
		cerr << "Contact: shuaiyuan.emory@gmail.com" << endl << endl;	
		cerr << "Usage: " << argv[0] <<" -i input.sam -m dipmap -o output.sam" << endl << endl;
		cerr << "Convert the sam file created from being mapped to a diploid reference genome " << endl;
		cerr << "  to being mapped to a haploid reference genome according to a dipmap file." << endl << endl;
		cerr << "Example: " << argv[0] << " -i input.sam -m hg19.di.fa.dipmap -o output.sam" << endl << endl;
		cerr << "Parameters (mandatory): " << endl;
		cerr << "  -i\t\tinput SAM file " << endl;
		cerr << "  -m\t\tdipmap file created by DiploidConstructor" << endl;
		cerr << "  -o\t\toutput SAM file" << endl;
		exit(-1);
	}

	CLineFields inputSamFile;
	if (inputSamFile.openFile(inputSamFileName)==false) {
		cerr << "Can not open sam file: " << inputSamFileName << endl;
		exit (-1);
	}
	
	ofstream outputSamFile;
	outputSamFile.open(outputSamFileName);

	// process the map file
	readMapFile(dipmapFileName);
	
	// process sam file
	inputSamFile.readline(removeHeader);
	vector<string> locations;
	while (inputSamFile.endofFile()==false) {
		if (inputSamFile.field[0][0]=='@' || inputSamFile.field[0][0]=='#') {// it is the header
			outputSamFile << inputSamFile.line << endl;
		} else {
			unsigned int flag = atoi(inputSamFile.field[1].c_str());
			if (flag & 4) {
				// the flag indicates unmapped read, so we set mapping quality to zero.
				inputSamFile.field[4] = "0";
			}
			// convert  (chr3b.300, 2)  to (chr3, 301)
			GenomePosition gp = GenomeLib::toRef(inputSamFile.field[2], inputSamFile.field[3], removeB);
			readLength = inputSamFile.field[9].length();
			// map from the extended reference allele minichromes to the real position on reference allelel.
            mapExtendedChrome(gp);
			int mm = 0;
			if (inputSamFile.field.size()>=14)
				mm = atoi(inputSamFile.field[13].substr(5).c_str());

			locations.clear();
			if (inputSamFile.field[inputSamFile.field.size()-1].substr(0,5)=="XA:Z:") { 
				// mapping quality==0 and we have chance to fix it                
    			inputSamFile.divide(inputSamFile.field[inputSamFile.field.size()-1].substr(5),";",locations);

				for (int i=0; i<(int)locations.size(); i++) {
					if(locations[i].length()==0) {
						locations.erase(locations.begin()+i);
						i=-1;//invalidate the for loop.
						continue;
					}
					vector<string> f;
					inputSamFile.divide(locations[i],",",f);
					if (f.size()<4)
						break; //skip reads with positions like:  chr3;.28387,+24,36M,2

					GenomePosition gp2 = GenomeLib::toRef(f[0], f[1], removeB);
					
					// map from the extended reference allele minichromes to the real position on reference allelel.
					mapExtendedChrome(gp2);					

					int new_mis = atoi(f[3].c_str());
					if (gp2.allele != 0) { // on non-ref alleles, maybe only partially cover the allele.
						int more_mis = abs(atoi(f[1].c_str()))-readLength;
						if (more_mis>0)
							new_mis += more_mis;  // the tail of the read is outside of the mini-chromosome.
					}
					if ((gp.chrName==gp2.chrName && gp.pos==gp2.pos) || new_mis != mm) {
						// eg. the read is mapped to both (chr1, 201) and (chr1b.200, 2).
						// or the mismatch != the first hit
						if (new_mis < mm) {
							// this hit is better than the first hit, use this to replace the first hit.
							gp = gp2;
							mm=new_mis;
						}
						locations.erase(locations.begin()+i);
						i=-1;//invalidate the for loop.
					}						
				}
				if (locations.empty()) {
					if (mm/readLength < 0.06)
						inputSamFile.field[4]="37"; // make up the mapping quality
					else
						inputSamFile.field[4]="25"; // make up the mapping quality
					flag &= ~4; //clear the "unmapped read bit"
					if (gp.strand<0)
						flag |= 16; //negtive strand;
				}
			}
			outputSamFile << inputSamFile.field[0] << "\t" << flag << "\t" << gp.chrName << "\t" << gp.pos;
			for (int i=4, size=inputSamFile.field.size(); i<size; i++) {
				outputSamFile << "\t" << inputSamFile.field[i];
			}
			outputSamFile << endl;
		}
		inputSamFile.readline(removeHeader);
	}
	inputSamFile.fp->close();
	outputSamFile.close();
}


