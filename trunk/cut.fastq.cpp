//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

int main(int argc, char* argv[])
{
	CLineFields file;
	int long_length, short_length;
	string long_id, long_seq, long_desc, long_quality;

	if (argc != 3) {
		cerr << "cut fastq file with long reads into short reads" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << "  input  short_length > output" << endl;
		return -1;
	}

	short_length = atoi(argv[2]);
	file.openFile(argv[1]);
	file.readline(false);
	while(file.endofFile()==false)
	{
		long_id=string(file.sect[0]);
		file.readline(false);
		long_seq=string(file.sect[0]);
		long_length=long_seq.length();
		file.readline(false);
		long_desc=string(file.sect[0]);
		file.readline(false);
		long_quality=string(file.sect[0]);
		
		if (long_length<short_length) {
			cerr << "warning! the length of source reads should be >= target reads" << endl;
		}

		int start = (long_length%short_length)/2;
		int short_id=1;
		while (long_length-start >= short_length) {
			cout << long_id << short_id << endl;
			cout << long_seq.substr(start, short_length) << endl;
			cout << long_desc << endl;
			cout << long_quality.substr(start, short_length) << endl;
			start += short_length;
			short_id++;
		}
		file.readline(false);
	}

	file.closeFile();
	return 0;
}

