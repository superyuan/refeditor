#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <my_global.h>
#include <mysql.h>
#include <mysql/errmsg.h>
//#include <sstream>
//#include <iterator>
#include "CLineFields.h"

#define DATABASE "imprinting"

int mysql_query_safe (MYSQL *conn, const char *cmd) 
{
	int result = mysql_query(conn, cmd);
	if (result) {
		cerr <<mysql_error(conn) << endl;
		cerr << "command: " << cmd << endl;
		exit(-1);
	}
}

// delete a table from the database
int drop_table(MYSQL *conn, char *tableName)
{
	stringstream cmd;
	cmd << "DROP TABLE "<< tableName <<" ;";
	mysql_query(conn, cmd.str().c_str()); // don't need to report error if table doesn't exist.
}

int main(int argc, char *argv[])
{

	stringstream cmd;
	string strDrop("-drop");
	if (argc < 4) {
		cout << argv[0] <<"  geneRefFileName  geneTableName  exonTableName [-drop]" << endl;
		cout << " -drop: drop previous tables" <<endl;
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
	if (argc >=5 && strDrop.compare(argv[4])==0) {
		drop_table(conn, argv[2]); // delete the old gene table first.
		drop_table(conn, argv[3]); // delete the old exon table first.
	}
	cmd.str("");
	//cmd << "CREATE TABLE "<<argv[2]<<"(chr  CHAR(16), start INT, end INT, exon_num INT, name VARCHAR(20), name2 VARCHAR(20), strand CHAR(1), PRIMARY KEY(chr,start,end,name,name2));";
	cmd << "CREATE TABLE IF NOT EXISTS "<<argv[2]<<"(chr  CHAR(16), start INT, end INT, exon_num INT, name VARCHAR(255), name2 VARCHAR(255), strand CHAR(1), PRIMARY KEY(name2, chr, start, end));";
	cout << cmd.str() << endl;
	mysql_query_safe(conn, cmd.str().c_str());// create the table.
	cmd.str("");
	//cmd << "CREATE TABLE "<<argv[3]<<"(name2 VARCHAR(20), start INT, end INT, is_exon BOOL, seq_num INT, PRIMARY KEY(name2,start,end));";
	cmd << "CREATE TABLE IF NOT EXISTS "<<argv[3]<<"(name VARCHAR(255), name2 VARCHAR(255), chr CHAR(16), start INT, end INT, is_exon BOOL, seq_num INT, strand CHAR(1), PRIMARY KEY(name2, chr, start, end));";
	cout << cmd.str() << endl;
	mysql_query_safe(conn, cmd.str().c_str());// create the table.
	file.readline();
	while (file.endofFile()==false){
		cmd.str("");
		cmd << "INSERT INTO "<<argv[2]<<" (chr, start, end, exon_num, name, name2, strand)  VALUES( '"<<
			file.field[1]<<"', "<<
			file.field[3]<<","<<
			file.field[4]<<","<<
			file.field[7]<<",'"<<
			file.field[0]<<"','"<<
			file.field[11]<<"','"<<
			file.field[2]<<"');";
		mysql_query(conn, cmd.str().c_str());//insert a record into the gene table		
		{
			int exon_num = atoi(file.field[7].c_str());
			size_t found=file.field[8].find(',');
 			while (found!=string::npos) {
				file.field[8].replace(found, 1, "\t");
				found=file.field[8].find(',');
 			}
			found=file.field[9].find(',');
 			while (found!=string::npos) {
				file.field[9].replace(found, 1, "\t");
				found=file.field[9].find(',');
 			}
			vector<string> exon_start, exon_end;
			{
				istringstream iss(file.field[8]);
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter<vector<string> >(exon_start));
			}
			{
				istringstream iss(file.field[9]);
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter<vector<string> >(exon_end));
			}
			for (int i=0; i<exon_num; i++) {
				if (i>0)
				{//insert an intron first
					cmd.str("");
					int intron_start = atoi(exon_end[i-1].c_str())+1;
					int intron_end = atoi(exon_start[i].c_str())-1;
					cmd << "INSERT INTO "<<argv[3]<<" (name, name2, chr, start, end, is_exon, seq_num, strand)  VALUES( '"<<
					file.field[0]<<"', '"<<
					file.field[11]<<"', '"<<
					file.field[1]<<"', "<<
					intron_start<<","<<
					intron_end<<","<<
					0<<","<<
					i<<",'"<< 
					file.field[2] <<"');";
					mysql_query(conn, cmd.str().c_str());//insert a record into the exon table		
				}				
				cmd.str("");
				cmd << "INSERT INTO "<<argv[3]<<" (name, name2, chr, start, end, is_exon, seq_num, strand)  VALUES( '"<<
				file.field[0]<<"', '"<<
				file.field[11]<<"', '"<<
				file.field[1]<<"', "<<
				exon_start[i]<<","<<
				exon_end[i]<<","<<
				1<<","<<
				i+1<<",'"<<
				file.field[2] <<"');";
				mysql_query(conn, cmd.str().c_str());//insert a record into the exon table		
			}


			
		}
		file.readline();
	}
	mysql_close(conn);
	file.fp->close();
}

