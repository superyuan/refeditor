#! /usr/bin/env python
import sys
from bioshared import *

''' the program starts here '''
''' usage: program [small_file] [large_file] '''
CHRNAME=0
POSITION=1
GENENAME=11

smap = {}
sset = set()
sf = open(sys.argv[1], 'r') # smaller file, usually the file contains only imprinting SNPs
lf = open(sys.argv[2], 'r') # larger file containing all SNPs

s1 = freadline(sf)
while len(s1) > 0:
	s1fields = s1.split('\t')
	smap[s1fields[CHRNAME]+'p'+s1fields[POSITION]] = s1fields;
	sset.add(s1fields[GENENAME]);
	s1 = freadline(sf)

s2 = freadline(lf)
while len(s2) > 0:
	s2fields = s2.split('\t')
	if s2fields[12] in sset: # Gene name in small file.
		sys.stdout.write(s2fields[0]+'\t'+s2fields[1]+'\t'+s2fields[2]+'\t' \
		+s2fields[3]+'\t'+s2fields[4]+'\t'+s2fields[5]+'\t'+s2fields[6]+'\t'\
		#+s2fields[7]+'\t'+s2fields[8]+'\t'+s2fields[9]+'\t'
		+s2fields[10]+'\t'+s2fields[11]+'\t'+s2fields[12]+'\t'+s2fields[13]+'\t')
		key = s2fields[CHRNAME]+'p'+s2fields[POSITION]
		if key in smap: # chr name and position match
			s1fields = smap[key]
			sys.stdout.write(s1fields[3]+'\t'+s1fields[4]+'\t'+s1fields[7]+'\t'+s1fields[8]+'\t')
		print	
	s2 = freadline(lf)	
	
sf.close()	
lf.close()	
