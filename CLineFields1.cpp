#include <sstream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CLineFields1.h"
//////////////////////////////////////////

#define MAX_CHR_NAME_LEN 5

// eg.
// convert	(chr3b.300, 2)  to  (chr3, 301)
GenomePosition GenomeLib::toRef(string name, string position, bool removeb)
{
	GenomePosition gp;
	gp.pos = atoi(position.c_str());
	gp.strand = 1;
	if (gp.pos < 0) {
		gp.pos = -gp.pos;
		gp.strand = -1;
	}
	gp.chrName = name;
	size_t found = gp.chrName.find('.');
	if (found != string::npos) {
		// it is a stripped diploid chromosome
		gp.pos += atoi(gp.chrName.substr(found+1).c_str()) -1;
		gp.chrName = gp.chrName.substr(0, found);
	}
	if (removeb) {
		char last_char=gp.chrName[gp.chrName.length()-1];
		if ((last_char>='0' && last_char<='9')||(last_char>='A' && last_char<='Z')||(gp.chrName=="*")) {
			// read is on reference allele
			gp.allele = 0;
		} else {
			// read is on alternative allele
			gp.allele = gp.chrName[gp.chrName.length()-1]-'a';
			gp.chrName = gp.chrName.substr(0,gp.chrName.length()-1);
		}
		//		if (gp.chrName.length()>MAX_CHR_NAME_LEN) {
		//convert mini chromosome coordinates to normal coordinates
		//			gp.chrName = gp.chrName.substr(0,gp.chrName.length()-3);
		//		}
		//found = gp.chrName.find('b');// remove all 'b' in the chr name
		//while (found != string::npos){
		//	gp.chrName.replace(found,1,"");
		//	found = gp.chrName.find('b');
		//}
	}
	return gp;
}


bool CLineFields::openFile(istream *instream)
{
	fp = (ifstream*) instream;
	return (fp)->is_open();
}

int CLineFields::divide(string s, string divisor, vector<string> &field)
{
	field.clear();
	size_t found;
	int start = 0;
	found = s.find(divisor);
	while (found != string::npos){
		field.push_back(s.substr(start,found-start));
		start = found+divisor.length();
		found = s.find(divisor,start);
	}
	field.push_back(s.substr(start,s.length()-start));
	return field.size();
}

//return true if open input file successfully.
bool CLineFields::openFile(const char *fileName)
{
	fp = new ifstream(fileName);
	return fp->is_open();
}

void CLineFields::closeFile ()
{
	return fp->close();
}

bool CLineFields::isOpen()
{
	return fp && (fp->is_open());
}

bool CLineFields:: seek ( streampos pos )
{
	fp->clear();
	fp->seekg(pos);
	return true;
}

CLineFields::CLineFields()
{
	//fieldNum = 0;
	fp = NULL;
}

bool CLineFields::endofFile ()
{
	return fp->eof();
}

bool CLineFields::readline (bool ignoreComment, string divisor)
{
	field.clear();
	do {
		getline(*fp, line);
		if (line.length()==0)
			return false;
	} while ((line[0]=='@' || line[0]=='#') &&(ignoreComment)) ;

	divide(line, divisor, field);

	return true;
}

//////////////////////////////////////////////////////////////////////////
char keys[] = {'A','T','G','C','N'};

int substr2int(string &s, int &p)
{
	//''' convert substring of s started from p to an interger '''
	//''' return the value and the position of last digit + 1 '''
	int sum = 0;
	char c = s[p];

	while (c>='0' && c<='9') {
		sum = sum*10 + c-'0';
		p ++;
		c = s[p];
	}
	return sum;
}

void initPileupInfo(map<char,int> & pileupInfo)
{
	for (int i=0; i<(int)sizeof(keys); i++) {
		pileupInfo[keys[i]] = 0;
	}
}

void pileupShortStr2Info(string &s, map<char,int> &pileupInfo)
{
	int i = 0;
	initPileupInfo(pileupInfo);
	if (s=="NoInfo")
		return;
	while (i<(int)s.length()) {
		int num = substr2int(s, i);
		pileupInfo[s[i]] = num;
		i++;
	}
}

char pileupInfoMajor(map<char,int> &pileupInfo, float threadhold)
{
	int total = 0;
	for (int i=0; i<(int)sizeof(keys); i++) {
		if (keys[i]=='N')
			continue; // we don't count 'N' into total
		total += pileupInfo[keys[i]];
	}
	if (total > 0) {
		for (int i=0; i<(int)sizeof(keys); i++) {
			if (keys[i]=='N')
				continue; // we don't count 'N' as majority
			if (((float)pileupInfo[keys[i]])/(float)total>=threadhold)  { //must be >= than threadhold(eg. 80%)
				return keys[i];
			}
		}
	}
	return (char)0;
}

void pileupLongStr2Info(string &s, char base, map<char,int>& pileupInfo)
{
	int value;
	int i = 0;
	initPileupInfo(pileupInfo);
	while (i<(int)s.length()) {
		char L = toupper(s[i]);
		if (L=='A' || L=='C' || L=='G' || L=='T' || L=='N')
			pileupInfo[L] += 1;
		else if (L=='.' || L==',')
			pileupInfo[base] += 1;
		else if (L=='+' || L=='-') {
			i ++;
			value = substr2int(s, i);
			i += value-1;
		}
		else if (L=='^')
			i += 1;
		else if (L=='<' || s[i]=='>'|| L=='*')
			pileupInfo['N'] += 1;
		else if (L=='$')
			;
		else {
			cout << "unexpected letter " << s[i];
			exit(-1);
		}
		i += 1;
	}

}

string pileupLongStr2ShortStr(string &s, char base)
{
	stringstream ss;
	map<char,int> pileupInfo;
	pileupLongStr2Info(s, base, pileupInfo);
	for (int i=0; i<(int)sizeof(keys); i++) {
		if (pileupInfo[keys[i]]>0)
			ss << pileupInfo[keys[i]] << keys[i];
	}
	return ss.str();
}

