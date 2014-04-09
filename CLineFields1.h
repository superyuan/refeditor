#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;



class GenomePosition
{
public:
	string chrName;
	int pos;
	int allele;
	int strand;
};


class GenomeLib
{
public:
	static GenomePosition toRef(string name, string pos, bool removeb=true);
};

class CLineFields
{
public:
//#define FIELD_NUM 200
//#define FIELD_SIZE 1024
//	char sect[FIELD_NUM][FIELD_SIZE];
//	int sect_num;
	vector<string> field;
//	int fieldNum;
	ifstream *fp;
	string line;
	CLineFields();	
	int divide(string s, string divisor, vector<string> &field);
	bool endofFile();
	bool readline (bool ignoreComment=true, string divisor="\t");
	bool openFile(istream *instream);
	bool openFile(const char *fileName);
	void closeFile ();
	bool isOpen();
	bool seek ( streampos pos );

};

int substr2int(string &s, int &p);

void initPileupInfo(map<char,int> & pileupInfo);

void pileupShortStr2Info(string &s, map<char,int> &pileupInfo);

char pileupInfoMajor(map<char,int> &pileupInfo, float threadhold);

void pileupLongStr2Info(string &s, char base, map<char,int>& pileupInfo);

string pileupLongStr2ShortStr(string &s, char base);



