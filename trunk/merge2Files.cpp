#include <sstream>
#include <iterator>
#include "CLineFields.h"
//////////////////////////////////////////
#include <stdio.h>

typedef enum __DataType_e{
	LONG_NUM,
	FLOAT,
	STRING,
} DataType_e;

class MyData
{
public:	
	char str[1024];  // store the data as a string
	bool operator - (const MyData & y);
	bool operator == (const MyData & y);
	bool operator < (const MyData & y);
	bool operator <= (const MyData & y);
	bool operator > (const MyData & y);
	bool operator >= (const MyData & y);
	bool operator != (const MyData & y);
};

long long int MyData::operator -= (const MyData& y)
{
	int i=0;
	DataType_e type = LONG_NUM;
	while (str[i]) 
	{
		if (str[i]>='0' && str[i]<='9') {
		} else if (str[i]=='.'){
			type = FLOAT;
		}else{
			type = STRING;
			break;
		}
		i++;
	}
	if (type != STRING) {
		i = 0;
		while (y.str[i]) 
		{
			if (y.str[i]>='0' && y.str[i]<='9') {
			} else if (y.str[i]=='.'){
				type = FLOAT;
			}else{
				type = STRING;
				break;
			}
			i++;
		}
	}
	if (type==LONG_NUM){
		long long int llx =	strtoll(str);
		long long int lly =	strtoll(y.str);
		return llx - lly;
	} else if (type==FLOAT) {
		float fx =	atof(str);
		float fy =	atof(y.str);
		return fx - fy;
	} else {
		return strcmp(str, y.str);
	}
	
	return true;
}
bool MyData::operator == (const MyData& y)
{
	return (this - y)==0;
}
bool MyData::operator < (const MyData& y)
{
	return (this - y)<0;
}
bool MyData::operator <= (const MyData& y)
{
	return (this - y)<=0;
}
bool MyData::operator > (const MyData& y)
{
	return (this - y)>0;
}
bool MyData::operator >= (const MyData& y)
{
	return (this - y)>=0;
}
bool MyData::operator != (const MyData& y)
{
	return (this - y)!=0;
}

class FileStruct
{
public:
	FILE *fp;
	MyData C[30]; // 30 columns at most.
};

void step(FileStruct &F)
{
	int i=0;
	if (!feof(F.fp)) 
	{
		while (fscanf(F.fp, "%s\t", &F.C[i]) > 0)
			i++;
	}
}

bool endfile(FileStruct &F)
{
	return feof(F.fp);
}


int main(int argc, char *argv[])
{

	int i=0;
	int numFiles = 0;
	FileStruct F[4];
	for (i=1; i<argc; i++)
	{		
		F[numFiles].fp = fopen(argv[i], "rb");
		numFiles ++;		
	}
	
	
	
	while (!endfile(F[0]) ||!endfile(F[1])) 
	{
		


		
	}
	




	for (i=0; i<numFiles; i++)
	{
		fclose(F[i].fp);
	}
	return 0;


}

