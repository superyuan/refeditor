// rotate.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CLineFields1.h"

typedef vector<string> Row;
typedef vector<Row> Table;

int main(int argc, char* argv[])
{
    CLineFields file;
    Table table;
    Row row;
    
    if (argc != 2) {
	cerr << "rotate the input file 90 degrees, exchange rows and columns" <<endl;
        cerr << "Usage:" << endl;
        cerr << "    " << argv[0] << "  file1 " << endl;
        exit(-1);
    }

    int max_rows=0, max_columns=0;

    file.openFile(argv[1]);
    file.readline();

    while (file.endofFile()==false) {

        if (max_columns<(int)file.field.size())
            max_columns=(int)file.field.size();
        row.clear();
        for (int i=0; i<(int)file.field.size(); i++) {
            row.push_back(file.field[i]);
        }
        table.push_back(row);
        max_rows++;
        file.readline();
    }
    file.closeFile();
    
    for (int i=0; i<max_columns; i++) {
        cout << table[0][i];
        for (int j=1; j<max_rows; j++) {
            if ((int)(table[j].size()) > i)
                cout << "\t" << table[j][i];
            else
                cout << "\t";
        }
        cout << endl;
    }
    return 0;
}
