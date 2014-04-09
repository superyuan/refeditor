#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
using namespace std;

int main(int argc, char *argv[])
{
	string line;
	if (argc != 3) {
		cerr << "Error input! Should be: "<< argv[0] << " file offset" << endl; 
		exit (-1);
	}
	ifstream *fp;
	fp = new ifstream(argv[1]);
	if (fp==NULL) {
		cerr<<"Can not open file: " << argv[1] << endl;
		exit(-1);
	}
	
	long long int transOffset = strtoll(argv[2], NULL, 10);
	
	getline(*fp, line);
	size_t chrname_start = line.find("range=")+string("range=").length();
	size_t chrname_end = line.find(":", chrname_start);
	string chrname = line.substr(chrname_start, chrname_end-chrname_start);
	chrname_start = chrname_end +1;
	chrname_end=line.find("-", chrname_start);
	long long int chrStart = strtoll(line.substr(chrname_start, chrname_end-chrname_start).c_str(), NULL, 0);
	chrname_start = chrname_end +1;
        chrname_end=line.find(" ", chrname_start);
        long long int chrEnd = strtoll(line.substr(chrname_start, chrname_end-chrname_start).c_str(), NULL, 0);
	chrname_start = chrname_end +1;
        chrname_end=line.find("strand=", chrname_start);
	char strand_char = line[chrname_end+strlen("strand=")];
	int strand = strand_char=='+'? 1:-1;

	int curOffset = 0;
	long long int curPosition = strand>0?chrStart:chrEnd;

	getline(*fp, line);
	while (fp->eof() == false) {
		for (int i=0; i<(int)line.length(); i++) {
			if (isupper(line[i])) {// it is in exon
				curOffset ++;
			}
			if (curOffset == transOffset) {
				// we found the position
				cout << chrname << ": " << curPosition << endl;
				fp->close();
				exit(0);
			}
			curPosition += strand;
		}
		getline(*fp, line);
	}
	cerr << "Out of range! max=" << curOffset << endl;
	fp->close();
	return 0;
}

