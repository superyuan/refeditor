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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <string>
//#include <my_global.h>
//#include <mysql.h>
#include "CLineFields1.h"

map<string, string> chrMap; // a map from chr name to sequence string

// convert string s to upper case
string toUpperString(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}
// output reads on the other stand of s
string onOtherStrand (string s)
{
	string ret="";
	int l=s.length();
	for (int i=l-1; i>=0; i--) {
		switch (s[i]) {
			case 'A': ret+='T'; break;
			case 'T': ret+='A'; break;
			case 'C': ret+='G'; break;
			case 'G': ret+='C'; break;
			case 'a': ret+='t'; break;
			case 't': ret+='a'; break;
			case 'c': ret+='g'; break;
			case 'g': ret+='c'; break;
			default: cout<<'N'; break;
		}   
	}
	return ret;
}

int main(int argc, char *argv[])
{
	map<string, string> chrMap; // a map from chr name to sequence string

	if (argc != 2) {
		cerr << "usage: " << argv[0] <<" fasta_file" << endl;
		cerr << "example: " << argv[0] <<" mm9.fa" << endl;
		exit(-1);
	}
	CLineFields file;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	cout << "reading fasta file ...." << flush;
	file.readline();
	string chrName = "";
	while (file.endofFile()==false){
		if (file.line.length() > 0) {
			if (file.line[0]=='>') {
				chrName = file.line.substr(1);
				chrMap[chrName] = "";
			} else {
				chrMap[chrName] += file.line;
			}
		}
		file.readline();
	}
	file.fp->close();

	cout << "done." << endl;

	///////////////////////
	int count_chromosomes=0;
	long long int total_length=0;
	for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
		count_chromosomes ++;
		total_length += (*it).second.length();
	}
	cout << "There are " << count_chromosomes << " chromosomes with total length " << total_length << endl;
	///////////////////////

	cout <<  "Now enter commands:  % > chr  start  length [+/-]" << endl;
	cout << "To search, use /STRING_YOU_WANT_TO_SEARCH" << endl;
	cout << "% > " << flush;
	CLineFields command;
	command.openFile(&cin);
	command.readline(true," ");
	while (command.field.size()==0 || (command.field[0] != "quit" && command.field[0] != "exit")) {
		if (command.field.size()>=3) {
			chrName = command.field[0];
			string value = chrMap[chrName];
			int start = atoi(command.field[1].c_str())-1;
			int end = start + atoi(command.field[2].c_str());
			if (start<0) start=0;
			if (start>(int)value.length()) start=value.length();
			if (end>(int)value.length()) end=value.length();
			string read=value.substr(start, end-start);
			if (command.field.size()>=4  &&  command.field[3]=="-") {
				// output the reversed strand
				cout << onOtherStrand(read);
			} else {
				// output the forward strand
				cout << read;
			}
			cout << endl;
		}
		else if (command.field.size()>=1 && command.field[0]=="list" ) {
			for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
				cout << (*it).first <<":"<<(*it).second.length()<< endl;
			}
		} else if (command.field.size()>=1 && command.field[0][0]=='/' ) {
			//search a string
			string s = command.field[0].substr(1, command.field[0].length()-1);
			s = toUpperString(s);
			string s2=onOtherStrand(s);
			for (map<string, string>::iterator  it= chrMap.begin(); it!= chrMap.end(); it++) {
				chrName = (*it).first;
				string value = toUpperString((*it).second);
				size_t found = value.find(s);
				while (found != string::npos) {
					// found the read
					cout << chrName << ":" << boost::lexical_cast<std::string>(found+1) << " +"<<endl;
					found = value.find(s, found+1);
				}
				found = value.find(s2);
				while (found != string::npos) {
					// found the read
					cout << chrName << ":" << boost::lexical_cast<std::string>(found+1) << " -" <<endl;
					found = value.find(s2, found+1);
				}
			}

		}
		cout << "% > " << flush;
		command.readline(true, " ");
	}
	command.fp->close();
}

