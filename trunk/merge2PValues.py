#! /usr/bin/env python
''' merge two P Values from male and female '''

import sys
from bioshared import *

value = {'chr1':1,'chr2':2,'chr3':3,'chr4':4,'chr5':5,'chr6':6,'chr7':7,'chr8':8,'chr9':9,\
         'chr10':10,'chr11':11,'chr12':12,'chr13':13,'chr14':14,'chr15':15,'chr16':16,'chr17':17,'chr18':18,'chr19':19,\
         'chrX':1000,'chrY':1001,'chrM':1002,'chrZ':10000}
keys = ['A','T','G','C','N']
    
def compareChrPos(chr1, pos1, chr2, pos2):
    if chr2 in value:
        if (value[chr1] != value[chr2]):
            return value[chr1] - value[chr2]
        else: #chr1 == chr2
            return pos1 - pos2
    else:
        return 1   

def substr2int(s, p):
    ''' convert substring of s started from p to an interger '''
    ''' return the value and the position of last digit + 1 '''
    sum = 0
    while s[p]>='0' and s[p]<='9':
        sum = sum*10 + int(s[p])
        p += 1
    return sum, p
    
def summaryPileupInfo(s, base):   
    ''' return a summary of the match/unmatch string in pileup file '''
    ''' case insensitive ''' 
    info = {}
    for key in keys:
        info[key] = 0
    i = 0    
    while i<len(s):
        if s[i].upper()=='A' or s[i].upper()=='C' or s[i].upper()=='G' or s[i].upper()=='T' or s[i].upper()=='N': 
            info[s[i].upper()] += 1
        elif s[i]=='.' or s[i]==',':
            info[base] += 1    
        elif s[i]=='+' or s[i]=='-': 
            value, i = substr2int(s, i+1)
            i += value-1   
        elif s[i]=='^': i += 1     
        elif s[i]=='<' or s[i]=='>':
            info['N'] += 1
        elif s[i]=='$': i = i
        else:
            print 'unexpected letter ' + s[i]
            sys.exit(-1)             
        i += 1 
    return info 
     
def summaryPileupStr(s, base):
    ''' return a summary string of the match/unmatch string in pileup file '''
    info = summaryPileupInfo(s, base)
    ret = ''    
    for key in keys:
        if info[key]>0:
            ret += str(info[key])+key
    return ret    

def readFields (f):
	''' read a line of fields from file f '''
	fileStr = freadline(f)
	if len(fileStr)>0:
		fields = fileStr.split('\t')
		fields[1] = int(fields[1])
		return fields 
	else:
		return None
	
''' the program starts here '''
''' usage: program [file1] [file2] [file3] [file4]... '''
fileNum = 0
f = []
fileField = []

for i in range(1,len(sys.argv)):
    f.append(open(sys.argv[i], 'r'))
    fileField.append(readFields(f[fileNum]))
    fileNum += 1            
    
while (fileField[0] != None) and (fileField[1] != None):
	for j in range(1, fileNum):
		result = compareChrPos(fileField[0][0], fileField[0][1], fileField[j][0], fileField[j][1])
		if (result == 0): #match
			print(fileField[0][0]+'\t'+str(fileField[0][1])+'\t'+fileField[0][2]+'\t'+fileField[0][3]+'\t'+fileField[0][4]+'\t'+fileField[0][5]+'\t'+fileField[0][6]+'\t'+fileField[0][7]+'\t'+fileField[0][8]+'\t'+fileField[0][9]+'\t'+fileField[0][10]+'\t'+fileField[0][11]+'\t'+fileField[0][12]+'\t'+fileField[j][5]+'\t'+fileField[j][6]+'\t'+fileField[j][7]+'\t'+fileField[j][8]+'\t'+fileField[j][9]+'\t'+fileField[j][10]+'\t'+fileField[j][11]+'\t'+fileField[j][12]+'\t')
			fileField[0] = readFields(f[0])
			fileField[j] = readFields(f[j])
		elif (result < 0):
			fileField[0] = readFields(f[0])
		elif (result > 0):
			fileField[j] = readFields(f[j])
	
for i in range(fileNum):
    f[i].close()   
