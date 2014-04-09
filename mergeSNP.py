#! /usr/bin/env python
import sys
import math
from bioshared import *


###keys = ['.',',','A','a','T','t','G','g','C','c','N','n','+','-','<','>']
keys = ['A','T','G','C','N']

#def magnitude(v):
#    return math.sqrt(sum(v[i]*v[i] for i in range(len(v))))

#def normalize(v):
#    vmag = magnitude(v)
#    return [ v[i]/vmag  for i in range(len(v)) ]

def Numeric_dot(u,v):
	r = 0.
	for i in range(len(u)):
		r += u[i]*v[i]	
	return r

def cosine_distance(u, v):
    """ Returns the cosine of the angle between vectors v and u. This is equal to u.v / |u||v|. """
    return Numeric_dot(u, v) / (math.sqrt(Numeric_dot(u, u)) * math.sqrt(Numeric_dot(v, v))) 

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
    
    
def summaryPileupInfoCase(s):
    ''' return a summary of the match/unmatch string in pileup file '''
    ''' case sensitive '''
    info = {}
    for key in keys:
        info[key] = 0
 
    i = 0    
    while i<len(s):
        if s[i]=='.' or s[i]==',' or s[i]=='<' or s[i]=='>':  info[s[i]] += 1   
        elif s[i].upper()=='A' or s[i].upper()=='C' or s[i].upper()=='G' or s[i].upper()=='T' or s[i].upper()=='N': 
            info[s[i]] += 1
        elif s[i]=='+' or s[i]=='-': 
            value, i = substr2int(s, i+1)
            i += value-1
        elif s[i]=='$': i = i 
        elif s[i]=='^': i += 1
        else:
            print 'unexpected letter'+ s[i]
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

def allEmptyStr(strList):
    '''return True if all strings in the list strList are empty strings '''
    n = len(strList)
    for i in range(n):
        if len(strList[i])>0:
            return False
    return True    

def findMinPosFile(fileField):
    ''' return which element in fileField has minimal position '''
    value = {'chr1':1,'chr2':2,'chr3':3,'chr4':4,'chr5':5,'chr6':6,'chr7':7,'chr8':8,'chr9':9,\
             'chr10':10,'chr11':11,'chr12':12,'chr13':13,'chr14':14,'chr15':15,'chr16':16,'chr17':17,'chr18':18,'chr19':19,\
             'chrX':1000,'chrY':1001,'chrM':1002,'zzzzzz':10000}
    minChrName = 'zzzzzz'
    minPos = sys.maxint
    minFile = -1
    fileNum = len(fileField)
    for i in range(fileNum):
        if len(fileField[i][0])>0:  
            if value[fileField[i][0]] < value[minChrName]:
                minChrName = fileField[i][0]
                minPos = fileField[i][1]
                minFile = i
            elif (fileField[i][0]==minChrName) and (fileField[i][1]< minPos):
                minPos = fileField[i][1]    
                minFile = i
    return minFile    

''' the program starts here '''
''' usage: program [file1] [file2] [file3] [file4]... '''
flag_YN = False
fileNum = 0
f = []
fileStr = []

for i in range(1,len(sys.argv)):
    if sys.argv[i][0]=='-':
        for j in range(1, len(sys.argv[i])):
            if sys.argv[i][j]=='Y':
                flag_YN = True
    else:
        f.append(open(sys.argv[i], 'r'))
        fileStr.append(freadline(f[fileNum]))
        fileNum += 1            

fileField = [0 for col in range(fileNum)]
    
while allEmptyStr(fileStr)==False:
    for i in range(fileNum):
        fileField[i] = fileStr[i].split('\t')
        if len(fileStr[i])>0:    
            fileField[i][1] = int(fileField[i][1])
    i = findMinPosFile(fileField)     
    #output a position 
    sys.stdout.write(fileField[i][0]+'\t'+str(fileField[i][1])+'\t'+fileField[i][2]+'\t')
    for j in range(fileNum):
        if (fileField[i][0]==fileField[j][0]) and (fileField[i][1]==fileField[j][1]):
            if flag_YN:
                sys.stdout.write('YesSNP\t')
            else:
                sys.stdout.write(summaryPileupStr(fileField[j][8], fileField[i][2])+'\t')
            fileStr[j] = freadline(f[j])
        else:
            sys.stdout.write('NoInfo\t')
    #output similarity s1/s4  s2/s3  etc.
    s1_s2 = 0
    s1_s3 = 0
    s1_s4 = 0
    s2_s3 = 0
    s2_s4 = 0
    s3_s4 = 0
    
    print str(s1_s4) + '\t'+ str(s2_s3) + '\t'+ str((s1_s4+s2_s3-s1_s3-s2_s4)/2) + '\t'

for i in range(fileNum):
    f[i].close()   


