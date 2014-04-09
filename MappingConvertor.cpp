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

//vector<string> chrNameList; // use this list keep the order of chromosomes


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
bool parseParameters (int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-i")==0) {
			removeB = false;
		} else if (strcmp(argv[i], "-h")==0) {
			removeHeader = true;
		}
	}
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

        //int previous_offset = 0;
        //for (int j=0; j<chrMap[gp2.chrName]->size(); j++)
        //{       
        //    if ( (*chrMap[gp2.chrName])[j].from > gp2.pos+(*chrMap[gp2.chrName])[j].length/4)
        //        break;
        //    previous_offset = (*chrMap[gp2.chrName])[j].from - (*chrMap[gp2.chrName])[j].to;
        //}
        //gp2.pos -= previous_offset;
    }
    return 0;
}

int mapExtendedChrome2(GenomePosition &gp2)
{
    if (chrMap[gp2.chrName] != NULL)
    {       
        int previous_offset = 0;
        for (int j=0; j<(int)chrMap[gp2.chrName]->size(); j++)
        {       
            if ( (*chrMap[gp2.chrName])[j].from > gp2.pos+(*chrMap[gp2.chrName])[j].length/4)
                break;
            previous_offset = (*chrMap[gp2.chrName])[j].from - (*chrMap[gp2.chrName])[j].to;
        }
        gp2.pos -= previous_offset;
    }
    return 0;
}

int main(int argc, char *argv[])
{

	if (argc < 2) {
		cerr << "convert the sam file created from the stripped reference file to sam file created from unstripped reference file " << endl;
		cerr << "usage: " << argv[0] <<" result.sam  [options] > result.new.sam" << endl;
		cerr << "Options:" << endl;
		cerr << "\t-i:\tkeep 'b' in chr name" << endl;
		cerr << "\t-h:\tremove file header lines" << endl;
		exit(-1);
	}
	CLineFields fileSam;
	if (fileSam.openFile(argv[1])==false) {
		cerr << "Can not open sam file: " << argv[1] << endl;
		exit (-1);
	}
	parseParameters(argc, argv);
	// process the map file
	readMapFile("dipMapFile.txt");
	// process sam file
	fileSam.readline(removeHeader);
	vector<string> locations;
	//int readLength = 0;
	while (fileSam.endofFile()==false) {
		if (fileSam.field[0][0]=='@' || fileSam.field[0][0]=='#') {// it is the header
			if (removeB==false || fileSam.field[1].find(".")==string::npos) {
				// there are no b. c. d. etc in this header.
				// or if there are, and we want to keep them
				cout << fileSam.line << endl;
			}			
		} else {
			unsigned int flag = atoi(fileSam.field[1].c_str());
			if (flag & 4) {
				// the flag indicates unmapped read, so we set mapping quality to zero.
				fileSam.field[4] = "0";
			}
			// convert  (chr3b.300, 2)  to (chr3, 301)
			GenomePosition gp = GenomeLib::toRef(fileSam.field[2], fileSam.field[3], removeB);
			if (readLength==0)
				readLength = fileSam.field[9].length();
			// map from the extended reference allele minichromes to the real position on reference allelel.
                        mapExtendedChrome(gp);
			int mm = 0;
			if (fileSam.field.size()>=14)
				mm = atoi(fileSam.field[13].substr(5).c_str());
			if (gp.allele != 0) { // on non-ref alleles, maybe only partially cover the allele.
				int more_mis = abs(atoi(fileSam.field[3].c_str()))-readLength;
				if (more_mis>0)
					mm += more_mis;
			}
			locations.clear();
			if ( /* fileSam.field[4]=="0" && */ fileSam.field[fileSam.field.size()-1].substr(0,5)=="XA:Z:") { 
				// mapping quality==0 and we have chance to fix it                
    				fileSam.divide(fileSam.field[fileSam.field.size()-1].substr(5),";",locations);

				for (int i=0; i<(int)locations.size(); i++) {
					if(locations[i].length()==0) {
						locations.erase(locations.begin()+i);
						i=-1;//invalidate the for loop.
						continue;
					}
					vector<string> f;
					fileSam.divide(locations[i],",",f);
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
							//gp.chrName=gp2.chrName;
							//gp.pos=gp2.pos;
							//gp.allele=gp2.allele;
							gp = gp2;
							mm=new_mis;
						}
						locations.erase(locations.begin()+i);
						i=-1;//invalidate the for loop.
					}						
				}
				if (locations.empty()) {
					if (mm/readLength < 0.06)
						fileSam.field[4]="37"; // make up the mapping quality
					else
						fileSam.field[4]="25"; // make up the mapping quality
					flag &= ~4; //clear the "unmapped read bit"
					if (gp.strand<0)
						flag |= 16; //negtive strand;
					//fileSam.field[1] = static_cast<ostringstream*>( &(ostringstream() << flag) )->str();
				}
			}
			cout << fileSam.field[0] << "\t" << flag << "\t" << gp.chrName << "\t" << gp.pos;
			for (int i=4, size=fileSam.field.size(); i<size; i++) {
				cout << "\t" << fileSam.field[i];
			}
			cout << endl;
		}
		fileSam.readline(removeHeader);
	}
	fileSam.fp->close();
}


