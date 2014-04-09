#! /usr/bin/env python
import sys
import math
from bioshared import *

value = {'chr1':1,'chr2':2,'chr3':3,'chr4':4,'chr5':5,'chr6':6,'chr7':7,'chr8':8,'chr9':9,\
         'chr10':10,'chr11':11,'chr12':12,'chr13':13,'chr14':14,'chr15':15,'chr16':16,'chr17':17,'chr18':18,'chr19':19,\
         'chrX':1000,'chrY':1001,'chrM':1002,'chrZ':10000}
    
def compareChrPos(chr1, pos1, chr2, pos2):
    if chr2 in value:
        if (value[chr1] != value[chr2]):
            return value[chr1] - value[chr2]
        else: #chr1 == chr2
            return pos1 - pos2
    else:
        return 1   

''' the program starts here '''
''' usage: program [file1] [file2] [file3] [file4]... '''
sf = open(sys.argv[1], 'r')
db = open(sys.argv[2], 'r')
s1 = freadline(sf)
s2 = freadline(db)
s1field = s1.split('\t')#split a line of string into fields.
s2field = s2.split('\t')#split a line of string into fields.
while len(s1) > 0:
    #print s2field[11]
    #if (72510827==int(s2field[11])):
     #   xxx = 3
     
    if s2field[1] != '2':
        mmm = 6;
    if len(s2field[11].strip())> 0:    
        result = compareChrPos(s1field[0], int(s1field[1]), 'chr'+s2field[6], int(s2field[11]))
    else:
        result = 1
    if result < 0:
        print s1.strip('\n')+'rsNull'+'\t'
        s1 = freadline(sf)
        if len(s1) > 0:
            s1field = s1.split('\t')
    elif result > 0:
        s2 = freadline(db) 
        if len(s2) > 0:
            s2field = s2.split('\t')
        else:
            s2field[6] = 'Z'
    else: # match
         print s1.strip('\n')+'rs'+s2field[0]+'\t'
         s1 = freadline(sf)
         s2 = freadline(db) 
         if len(s1) > 0:
            s1field = s1.split('\t')  
         if len(s2) > 0:
            s2field = s2.split('\t')
         else:
            s2field[6] = 'Z'       

sf.close()
db.close()   


