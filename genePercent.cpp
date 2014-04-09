#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

class CGeneSNP {
public:
	double child1_percentS1;
	double child1_percentS2;
	double child2_percentS1;
	double child2_percentS2;
	long long int numSNPs;
	CGeneSNP();
};
CGeneSNP::CGeneSNP()
{
	child1_percentS1 = child1_percentS2 = 0;
	child2_percentS1 = child2_percentS2 = 0;
	numSNPs = 0;
}

int main(int argc, char *argv[])
{
	CLineFields file;
	int child1_s1count = 0, child1_s2count=0, child2_s1count = 0, child2_s2count=0;//how many letters are there matching s1 or s2.
	int columnOfS1 = -1, columnOfS2 = -1;
	int columnOfChild1 = -1, columnOfChild2 = -1;
	int columnOfGeneName = -1, columnOfExon = -1;
	bool diffS1S2 = false, onlyExon=false;
	map<char,int> pileupInfoS1, pileupInfoS2, pileupInfoChild1, pileupInfoChild2;
	map<string, CGeneSNP *> geneInfo;
	
	if (argc < 7) {
		cerr << "error! input should be:" << endl;
		cerr << argv[0] <<" file s1  s2  child1 child2 GeneName [-d] [-e] [exon]"<< endl;
		cerr << "-d: s1 must not be the same as s2" << endl;
		exit(-1);
	}
	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], "-d")==0)
			diffS1S2 = true;
		if (strcmp(argv[i], "-e")==0) {
                        onlyExon = true;
			columnOfExon = atoi(argv[i+1]);
			i ++;
		}
	}
	
	columnOfS1 = atoi(argv[2]);
	columnOfS2 = atoi(argv[3]);
	columnOfChild1 = atoi(argv[4]);
	columnOfChild2 = atoi(argv[5]);
	columnOfGeneName = atoi(argv[6]);

	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}

	cout << "#chr	pos	ref	s1	s2	s3	s4	name	name2	(intr/ex)on	s1/child1	s2/child1	s1/child2	s2/child2" << endl;
	file.readline();
	while (file.endofFile()==false){
		child1_s1count = child1_s2count = child2_s1count = child2_s2count = 0;
		if (file.field[columnOfGeneName] != "NA") {// Gene name is valid
			if ((onlyExon==false) || (file.field[columnOfExon].find("exon")!=string::npos)) {
				string geneName = file.field[columnOfGeneName].substr(0,file.field[columnOfGeneName].find('_'));
//				pileupShortStr2Info(file.field[columnOfS1], pileupInfoS1);
//				pileupShortStr2Info(file.field[columnOfS2], pileupInfoS2);
//				char majorS1 = pileupInfoMajor(pileupInfoS1, 0.8); // get majority of s1. eg. for string "3T4A65G" majority is G
//				char majorS2 = pileupInfoMajor(pileupInfoS2, 0.8);
				char majorS1 = file.field[columnOfS1][0];
				char majorS2 = file.field[columnOfS2][0];
				if ((majorS1 !=0 && majorS2 !=  0)&&((diffS1S2==false) || (majorS1 != majorS2))) {
					pileupShortStr2Info(file.field[columnOfChild1], pileupInfoChild1);
					pileupShortStr2Info(file.field[columnOfChild2], pileupInfoChild2);

					child1_s1count = pileupInfoChild1[majorS1];// doesn't matter even if majorS1==0
					child1_s2count = pileupInfoChild1[majorS2];
					int child1_total = child1_s1count + child1_s2count;

					child2_s1count = pileupInfoChild2[majorS1];// doesn't matter even if majorS1==0
					child2_s2count = pileupInfoChild2[majorS2];
					int child2_total = child2_s1count + child2_s2count;

					cout << file.line;  // output original line first.
					if (child1_total>0 && child2_total>0) {
						cout << ((double)(child1_s1count))/(double)(child1_total) << "\t";
						cout << ((double)(child1_s2count))/(double)(child1_total) << "\t";
						cout << ((double)(child2_s1count))/(double)(child2_total) << "\t";
						cout << ((double)(child2_s2count))/(double)(child2_total) << "\t"<<endl;
	
//						if (geneInfo[geneName]==NULL) {//first SNP in the gene
//							geneInfo[geneName] = new CGeneSNP;
//						}
//						geneInfo[geneName]->child1_percentS1 += ((double)(child1_s1count))/(double)(child1_total);
//						geneInfo[geneName]->child1_percentS2 += ((double)(child1_s2count))/(double)(child1_total);
//						geneInfo[geneName]->child2_percentS1 += ((double)(child2_s1count))/(double)(child2_total);
//						geneInfo[geneName]->child2_percentS2 += ((double)(child2_s2count))/(double)(child2_total);
//						geneInfo[geneName]->numSNPs ++;
//						cout << "geneName:"<<geneName<<"\t"<<file.field[columnOfS1]<<"\t"<<file.field[columnOfS2]
//							<<"\t"<<file.field[columnOfChild1]<<"\t"<<file.field[columnOfChild2]<<"\t"<<file.field[columnOfExon]<<"\t"<<endl;
					} else {
						cout << "0\t0\t0\t0\t"<<endl;
					}

				}
			}
		}
		file.readline();
	}
//	cout << "#GeneName	s1/child1	s2/child1	s1/child2	s2/child2" << endl;
	for (map<string, CGeneSNP*>::iterator it= geneInfo.begin(); it!= geneInfo.end(); it++) {
		string geneName = (*it).first;
		CGeneSNP * genePercent = (*it).second;  //percentage of s1 and s2 in child1 and child2
		genePercent->child1_percentS1 /= genePercent->numSNPs;
		genePercent->child1_percentS2 /= genePercent->numSNPs;
		genePercent->child2_percentS1 /= genePercent->numSNPs;
		genePercent->child2_percentS2 /= genePercent->numSNPs;
//		cout << geneName <<"\t" << genePercent->child1_percentS1 <<"\t"<< genePercent->child1_percentS2<<"\t" << genePercent->child2_percentS1 <<"\t"<< genePercent->child2_percentS2 <<"\t"<<endl;
	}
	file.fp->close();
	return 0;
}
