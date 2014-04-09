#! /usr/bin/env python
import sys
from bioshared import *

totalLineNum = 0;
keyLineNum = 0;
f = open(sys.argv[1], 'r')
line = freadline(f)
while len(line) > 0:
	totalLineNum += 1
	if str.find(line, sys.argv[2])>=0:
		keyLineNum += 1
	line = freadline(f)	
f.close()

print "file: " + sys.argv[1] 
print "total lines:" + str(totalLineNum)
print "key word (" + sys.argv[2] + ") lines:" + str(keyLineNum) +"("+ str(float(keyLineNum*100)/float(totalLineNum)) + "%)"
print
#print "key word lines: " + str(float(keyLineNum*100)/float(totalLineNum)) + "%"

