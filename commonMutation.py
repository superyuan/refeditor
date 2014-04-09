#! /usr/bin/env python
import sys
from bioshared import *

def allEmptyStr(strList):
    '''return True if all strings in the list strList are empty strings'''
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
flag_YN = True
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
commonMatrix = [[0 for col in range(fileNum)] for row in range(fileNum)]
    
while allEmptyStr(fileStr)==False:
    for i in range(fileNum):
        fileField[i] = fileStr[i].split('\t')
        if len(fileStr[i])>0:    
            fileField[i][1] = int(fileField[i][1])
    i = findMinPosFile(fileField)          
    for j in range(fileNum):
        if i != j:    
            if len(fileStr[i])>0 and len(fileStr[j])>0:
                if (fileField[i][0]==fileField[j][0]) and (fileField[i][1]==fileField[j][1]):
                    commonMatrix[i][j] += 1
                    commonMatrix[j][i] += 1     
    commonMatrix[i][i] += 1             
    fileStr[i] = freadline(f[i])        
    
for i in range(fileNum):
    f[i].close()  
#output final result    
for i in range(fileNum):
    for j in range(fileNum): 
        #print commonMatrix[i][j], '\t',
	sys.stdout.write(str(commonMatrix[i][j])+'\t') #print out without a return or space
    print

