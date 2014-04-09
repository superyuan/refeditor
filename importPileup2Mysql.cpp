#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <my_global.h>
#include <mysql.h>
//#include <sstream>
//#include <iterator>
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
	if (argc != 3) {
		cout << argv[0] <<"  pileupFileName  tableName" << endl;
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
//	if (mysql_query(conn, "create database testdb")) {
//		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
//		exit(-1);
//	}
//create table friends (id int not null primary key auto_increment,
//                               name varchar(20), age int);
	drop_table(conn, argv[2]); // delete the old table first.
	stringstream cmd;
	//cmd << "CREATE TABLE "<<argv[2]<<"  (id int not null primary key auto_increment, chr  CHAR(16), position int, A int, C int, G int, T int, N int)";
	cmd << "CREATE TABLE "<<argv[2]<<"(chr  CHAR(16), position INT, reference CHAR(1), readbase CHAR(1), A int, C int, G int, T int, N int, id bigint not null, PRIMARY KEY(chr,position));";
	cout << cmd.str() << endl;
	//exit(-1);
	mysql_query(conn, cmd.str().c_str());// create the pileup table.
	map < char, int > pileupInfo;
	long long int id = 1;
	file.readline();
	while (file.endofFile()==false){
		pileupLongStr2Info(file.field[8], file.field[2][0], pileupInfo);
		string chrName = file.field[0];
		int position = atoi(file.field[1].c_str());
		if (position<=0) {
			cout << "error: position too large!" <<endl;
			exit(-1);
		}
//		cmd.flush();
		stringstream cmd2;
		cmd2 << "INSERT INTO "<<argv[2]<<" (chr, position, reference, readbase, A, C, G, T, N, id)  VALUES( '"<<
			chrName<<"', "<<
			position<<",'"<<
			file.field[2][0]<<"','"<<
			file.field[3][0]<<"',"<<
			//position<<","<<
			//file.field[2][0]<<","<<
			//file.field[3][0]<<","<<
			pileupInfo['A']<<","<<
			pileupInfo['C']<<","<<
			pileupInfo['G']<<","<<
			pileupInfo['T']<<","<<
			pileupInfo['N']<<","<<
			id++<<");";
		//cout << cmd2.str() << endl;
		//exit (-1);
		mysql_query(conn, cmd2.str().c_str());//insert a record into the table		
		file.readline();
	}
	mysql_close(conn);
	file.fp->close();
}

