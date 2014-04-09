//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields.h"

//map<string, string> results;
unsigned char mark[300*1024*1024];

// f1, f2 are 0-based

int main(int argc, char* argv[])
{
	CLineFields file;
	int chr_col, pos_col;
	map< string, map<int,string> * > chr2List;

	if (argc != 4) {
		cerr << "sort the content by chromosomes and positions" <<endl;
		cerr << "syntax:" << endl;
		cerr << "    " << argv[0] << "  input  column_of_chromosomes  column_of_positions > output" << endl;
		cerr << "      columns are 1-based" << endl;
		return -1;
	}

	chr_col = atoi(argv[2])-1;
	pos_col = atoi(argv[3])-1;

	file.openFile(argv[1]);
	file.readline();
	while(file.endofFile()==false)
	{
		string chr=string(file.sect[chr_col]);
		if (chr.length()>3)
		{
			chr = chr.substr(3, chr.length()-3);
			if (chr.length()==1) 
			{
				if (chr[0]>='1' && chr[0]<='9')
				{
					chr = "0"+chr;
				}
			}
		}
		if (chr2List[chr]==NULL)
		{
			chr2List[chr] = new map<int, string>;
		}
		int pos = atoi(file.sect[pos_col]);
		(*chr2List[chr])[pos] = file.line;
		

		file.readline();
	}

	for (map< string, map<int,string> * >::iterator it_chr=chr2List.begin(); it_chr!=chr2List.end(); ++it_chr)
	{
		for (map<int,string>::iterator it_pos = (*it_chr).second->begin(); it_pos != (*it_chr).second->end(); ++it_pos)
		{
			cout << (*it_pos).second << endl;
		}
	}


	file.closeFile();

	return 0;
}
