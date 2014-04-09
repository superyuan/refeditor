#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <my_global.h>
#include <mysql.h>
#include "CLineFields.h"

#define DATABASE "imprinting"

// delete a table from the database
int drop_table(MYSQL *conn, char *tableName)
{
	stringstream cmd;
	cmd << "DROP TABLE "<< tableName <<";";
	mysql_query(conn, cmd.str().c_str());
}

int main(int argc, char *argv[])
{
	bool writeit = false; // 1ST TODO
	if (argc != 3) {
		cerr << "usage: " << argv[0] <<" reference.fa  tableName" << endl;
		cerr << "example: " << argv[0] <<" mm9.fa  mm9_fa" << endl;
		exit(-1);
	}
	CLineFields file;
	if (file.openFile(argv[1])==false) {
		cerr << "Can not open file: " << argv[1] << endl;
		exit (-1);
	}
	MYSQL *conn;
	conn = mysql_init(NULL);
	if (mysql_real_connect(conn, "localhost", "mysql", "", "imprinting", 0, NULL, 0) == NULL) {
		cerr << "Can not connect to database: " << DATABASE << endl;
		exit(-1);
	}
	//drop_table(conn, argv[2]); // delete the old table first.  2ND TODO
	stringstream cmd;
	cmd << "CREATE TABLE "<<argv[2]<<"(chr  CHAR(16), position INT, reference CHAR(1), id bigint not null, PRIMARY KEY(chr,position));";
	cout << cmd.str() << endl;
	//mysql_query(conn, cmd.str().c_str());// create the fa table. 3RD TODO
	long long int id = 1;
	file.readline();
	string chrName = "";
	int position = 1;
	while (file.endofFile()==false){
		for (int i=0; i<file.line.length(); i++) {
			switch (file.line[i]) {
				case '>':
					chrName = "";
					i++;
					while (i<file.line.length() && file.line[i] != 0xA) {
						chrName += file.line[i];
						i++;
					}
					position = 1;
					cout << "start to handle " << chrName << endl;
					break;
				case 'A':
				case 'C':
				case 'G':
				case 'T':
				case 'N':{
					if (writeit) {
						stringstream cmd2;
						cmd2 << "INSERT INTO "<<argv[2]<<" (chr, position, reference, id)  VALUES( '" <<
							chrName<<"', "<<
							position<<",'"<<
							file.line[i]<<"',"<<
							id<<");";
						int ret = mysql_query(conn, cmd2.str().c_str());//insert a record into the table
					}
					if (chrName.compare("chr17")==0 && position==67787708) writeit = true;  // 4TH TODO
					position ++;
					id ++;
				}	break;
				case 0xA:
					break;
				default:
					cerr << "error!  read symbol: " << file.line[i] << " in line: " << file.line << endl;
					exit(-1);
					break;
			}
		}
		file.readline();
	}
	mysql_close(conn);
	file.fp->close();
}


