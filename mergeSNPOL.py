#! /usr/bin/env python
import sys
import math
from bioshared import *


###keys = ['.',',','A','a','T','t','G','g','C','c','N','n','+','-','<','>']
keys = ['A','T','G','C','N']

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
   

def info2Str(info):
    ret = ''
    for key in keys:
        if info[key]>0:
            ret += str(info[key])+key
    return ret 

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

def SNPInfoMajor(SNPInfo, percent):
    ''' return the majority of A/C/T/G if the one has the percentage > percent '''
    ''' return '-' if no one has such large percentage '''
    total = SNPInfo['A']+SNPInfo['C']+SNPInfo['G']+SNPInfo['T'] #+SNPInfo['N']
    if total == 0:
        return 'NoInfo'
    for key in ['A', 'C', 'T', 'G']:
        if float(SNPInfo[key])/float(total) >= percent:
            return key
    return 'NoMajor'


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

def shortStr2Info(s):
    p = 0
    info = {}
    for key in keys:
        info[key] = 0
    if s == 'NoInfo':
        return info
    else:
        while p < len(s):
            count, p = substr2int(s, p)
            info[s[p]] = count
            p += 1
        return info

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
f = open(sys.argv[1], 'r')
fields = readFields(f)

while fields != None:
    sys.stdout.write(fields[0]+'\t'+str(fields[1])+'\t'+fields[2]+'\t')
    SNPInfo = [] # a list of dictionaries 
    SNPInfo.append(shortStr2Info(fields[3]))
    SNPInfo.append(shortStr2Info(fields[4]))
    SNPInfo.append(shortStr2Info(fields[5]))
    SNPInfo.append(shortStr2Info(fields[6]))
    SNPMajor = []
    SNPMajor.append(SNPInfoMajor(SNPInfo[0], 0.8))    
    SNPMajor.append(SNPInfoMajor(SNPInfo[1], 0.8))
    sys.stdout.write(SNPMajor[0]+'\t'+SNPMajor[1]+'\t') #output single letter for s1, s2
    sys.stdout.write(fields[5]+'\t'+str(fields[6])+'\t') #output s3, s4
    if fields[5] != 'NoInfo' and len(SNPMajor[1]) == 1:
        sys.stdout.write(str(float(SNPInfo[2][SNPMajor[1]])/float(SNPInfo[2]['A']+SNPInfo[2]['C']+SNPInfo[2]['G']+SNPInfo[2]['T']+SNPInfo[2]['N']))+'\t')
    else:
        sys.stdout.write('0\t')
    if fields[6] != 'NoInfo' and len(SNPMajor[0]) == 1:
        sys.stdout.write(str(float(SNPInfo[3][SNPMajor[0]])/float(SNPInfo[3]['A']+SNPInfo[3]['C']+SNPInfo[3]['G']+SNPInfo[3]['T']+SNPInfo[3]['N']))+'\t')
    else:
        sys.stdout.write('0\t')          
    print  
    fields = readFields(f)

f.close()   


