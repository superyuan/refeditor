#include <stdlib.h>
#include <sstream>
#include <iterator>
#include "CLineFields.h"
#include <string.h>
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

#define MAX_POS 256000000

typedef struct __Cgenotype
{
	char allele1, allele2;
	char intron_exon;
	char ref;
	int count;
} Cgenotype;

Cgenotype *mapGenotype;
const char *chr;

int main(int argc, char *argv[])
{
	if (argc < 4) {
		cerr << "count the coverage of each genotype in the sam file. The sam file must already passed the quality control!" << endl; 
		cerr << "Example:" <<endl;
		cerr << "\t" << argv[0] << "  a.genotypext  a.sam  chr1  >  covearage" << endl;
		exit(-1);
	}

	chr=argv[3];	
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
	const char *current_chr;
	// process genotypes from genotype calling based on reference allele.
	file1.readline();
	while (file1.endofFile()==false){
		current_chr=file1.sect[1];
                if (strcmp(current_chr, chr)==0) {
			location = atoi(file1.sect[2]);
			mapGenotype[location].allele1 = file1.sect[3][0]; 
			mapGenotype[location].allele2 = file1.sect[4][0];
			mapGenotype[location].intron_exon = file1.sect[5][0];
			mapGenotype[location].ref = file1.sect[6][0];			
                } 
		file1.readline();
	}

	int read_length=0;
	// process genotypes from genotype calling based on alternative allele.
	file2.readline();
	while (file2.endofFile()==false){
		current_chr=file2.sect[2];
		if (strcmp(current_chr, chr)==0) {
			location = atoi(file2.sect[3]);
			read_length = strlen(file2.sect[9]);
			for (int i=location; i<location+read_length; i++) {
				mapGenotype[i].count++; 
			}
		}
		file2.readline();
	}
	
	for (int i=0; i<MAX_POS; i++) {
		if (mapGenotype[i].allele1 != '\0') {
			cout << ".\t" << chr << "\t" << i << "\t" << mapGenotype[i].allele1 << "\t"<< mapGenotype[i].allele2 << "\t" << mapGenotype[i].intron_exon << "\t" << mapGenotype[i].ref << "\t" << mapGenotype[i].count << endl;
		}
	}

	file1.fp->close();
	file2.fp->close();

	return 0;
}

