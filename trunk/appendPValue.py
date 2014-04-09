#! /usr/bin/env python
import sys
import math
import scipy.stats
from bioshared import *

def readFields (f):
    ''' read a line of fields from file f '''
    fileStr = freadline(f).replace('\n','')
    if len(fileStr)>0:
        fields = fileStr.split('\t')
        return fields 
    else:
        return None

''' the program starts here '''
''' usage: program file data1 data2 [options] ... '''
if (len(sys.argv) < 5):
    sys.stderr.write('this program append a p-value column in the input file and then output to stdout\n')
    sys.stderr.write('usage: ' + sys.argv[0] + ' file data1 data2 exp  # data1/data2 are 0-based columns\n')
    sys.stderr.write('for example: ' + sys.argv[0] + ' file.txt 0 1 0.5 \n')
    sys.stderr.write('             ' + sys.argv[0] + ' file2.txt 1 2 0.5 \n')
    sys.exit(-1)

f = open(sys.argv[1], 'r')
c1 = int(sys.argv[2])
c2 = int(sys.argv[3])
exp = float(sys.argv[4])

fields = readFields(f)

while fields != None:
    fn = len(fields) 
    for i in range(fn):
        sys.stdout.write(fields[i]+'\t');
    pValue1 = scipy.stats.binom.sf(int(fields[c1])-1, int(fields[c1])+int(fields[c2]), exp)
    pValue2 = scipy.stats.binom_test(int(fields[c1]), int(fields[c1])+int(fields[c2]), exp)
    sys.stdout.write(str(pValue1)+'\t')
    sys.stdout.write(str(pValue2))
    print
    fields = readFields(f)

f.close()   


