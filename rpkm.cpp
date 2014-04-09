#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>
#include <algorithm>

int main(int argc, char *argv[])
{
	CLineFields file;
	int s1count = 0, s2count=0, s3count = 0, s4count=0;//how many letters are there matching s1 or s2.
	int columnOfChr = -1, columnOfPos = -1;
	int columnOfS1 = -1, columnOfS2 = -1, columnOfS3 = -1, columnOfS4 = -1;
	int bucketSize = -1;
	long long int oldBucket = -1;
	string oldChr = "";
	map<char,int> pileupInfoS1, pileupInfoS2, pileupInfoS3, pileupInfoS4;

	
	if (argc < 9) {
		cerr << "error! input should be:" << endl;
		cerr << argv[0] <<" file chrsome position s1  s2  s3  s4 size"<< endl;
		exit(-1);
	}

	columnOfChr = atoi(argv[2]);
	columnOfPos= atoi(argv[3]);
	columnOfS1 = atoi(argv[4]);
	columnOfS2 = atoi(argv[5]);
	columnOfS3 = atoi(argv[6]);
	columnOfS4 = atoi(argv[7]);
	bucketSize= atoi(argv[8]);

	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}

	cout << "#chr\tbucket\ts1\ts2\ts3\ts4\t" << endl;
	file.readline();
	while (file.endofFile()==false){
		int chr = atoi(file.field[columnOfChr].c_str() + 3);
		long long int pos = strtoll(file.field[columnOfPos].c_str(), NULL, 10);
		long long int chrPos = chr*10000000000 + pos;
		long long int bucket = chrPos/bucketSize;

		pileupShortStr2Info(file.field[columnOfS1], pileupInfoS1);
		pileupShortStr2Info(file.field[columnOfS2], pileupInfoS2);
		pileupShortStr2Info(file.field[columnOfS3], pileupInfoS3);
		pileupShortStr2Info(file.field[columnOfS4], pileupInfoS4);

		if (bucket != oldBucket) {
			//output oldBucket first
			if (oldBucket > 0) {
				cout << oldChr << "\t" << oldBucket <<"\t" <<s1count<<"\t"<<s2count<<"\t"<<s3count<<"\t"<<s4count<<"\t"<<endl;
			}	
			s1count = s2count = s3count = s4count =0;
			oldBucket = bucket;
			oldChr = file.field[columnOfChr];
		} 
		s1count += pileupInfoS1['A']+pileupInfoS1['T']+pileupInfoS1['G']+pileupInfoS1['C'];
		s2count += pileupInfoS2['A']+pileupInfoS2['T']+pileupInfoS2['G']+pileupInfoS2['C'];
		s3count += pileupInfoS3['A']+pileupInfoS3['T']+pileupInfoS3['G']+pileupInfoS3['C'];
		s4count += pileupInfoS4['A']+pileupInfoS4['T']+pileupInfoS4['G']+pileupInfoS4['C'];		
		
		file.readline();
	}
	//output oldBucket first
	if (oldBucket > 0) {
		cout << oldChr<< "\t" << oldBucket <<"\t" <<s1count<<"\t"<<s2count<<"\t"<<s3count<<"\t"<<s4count<<"\t"<<endl;
	}
	file.fp->close();
	return 0;
}



