#include <stdlib.h>
#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

#define MAX_POS 256000000
//class Cgenotype
//{
//public:
//	string name;
//	string chr;
//	char allele1, allele2;
//};

typedef struct __Cgenotype
{
	char allele1, allele2;
} Cgenotype;


//map<int, Cgenotype*> mapGenotype;
//Cgenotype ** mapGenotype;
Cgenotype *mapGenotype;
string chr;

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cerr << "merge genotypes from two genotype callings based on reference allele and alternative allele" << endl; 
		cerr << "all genotypes MUST on the same chromosome!" << endl;
		cerr << "Example:" <<endl;
		cerr << "\t" << argv[0] << "  alleleA.genotypes  alleleB.genotypes  >  allel.genotypes" << endl;
		exit(-1);
	}
	
	mapGenotype = (Cgenotype *)calloc(MAX_POS, sizeof(Cgenotype));
	
	CLineFields file1, file2;

	if (file1.openFile(argv[1])==false) {
            cerr << "Can not open file: " << argv[1] << endl;
            exit (-1);
        }

	if (file2.openFile(argv[2])==false) {
            cerr << "Can not open file: " << argv[1] << endl;
            exit (-1);
        }

	int location;
	// process genotypes from genotype calling based on reference allele.
	file1.readline();
	chr = file1.sect[1];
	while (file1.endofFile()==false){
		location = atoi(file1.sect[2]);
//		if (mapGenotype[location] == NULL)
//			mapGenotype[location] = new Cgenotype;
//		mapGenotype[location]->name = file1.sect[0];
//		mapGenotype[location]->chr = file1.sect[1];
		mapGenotype[location].allele1 = file1.sect[3][0]; 
		mapGenotype[location].allele2 = file1.sect[4][0]; 
		file1.readline();
	}

	// process genotypes from genotype calling based on alternative allele.
	file2.readline();
	while (file2.endofFile()==false){
		location = atoi(file2.sect[2]);
		if (mapGenotype[location].allele1 == '\0') {
//			mapGenotype[location] = new Cgenotype;
//			mapGenotype[location]->name = file2.sect[0];
//			mapGenotype[location]->chr = file2.sect[1];
			mapGenotype[location].allele1 = file2.sect[3][0]; 
			mapGenotype[location].allele2 = file2.sect[4][0]; 
		} else {
			if (file2.sect[3][0] != mapGenotype[location].allele1)
				mapGenotype[location].allele2 = file2.sect[3][0];	
			if (file2.sect[4][0] != mapGenotype[location].allele1)
				mapGenotype[location].allele2 = file2.sect[4][0];	
		}
		file2.readline();
	}
	
	for (int i=0; i<MAX_POS; i++) {
		if (mapGenotype[i].allele1 != '\0') {
			cout << ".\t" << chr << "\t" << i << "\t" << mapGenotype[i].allele1 << "\t"<< mapGenotype[i].allele2 << endl;
		}
	}
//	for (map<int,Cgenotype*>::iterator it_pos = mapGenotype.begin(); it_pos != mapGenotype.end(); ++it_pos)
//        {
//		cout << it_pos->second->name << "\t" << it_pos->second->chr << "\t" << it_pos->first << "\t" << it_pos->second->allele1 << "\t"<< it_pos->second->allele2 << endl;
//        }

	file1.fp->close();
	file2.fp->close();

	return 0;
}

