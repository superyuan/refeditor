#! /usr/bin/env python

import sys
import copy
from bioshared import *

class Exon:
	''' Define an exon '''
	def __init__(self):
		self.start = 0
		self.end = 0
		self.gene = 0
	
class Gene:
	''' Define a gene '''
	def __init__(self):
		self.name = ''
		self.chr = ''
		self.strand = ''
		self.start = 0
		self.end = 0
		self.cdsStart = 0
		self.cdsEnd = 0
		self.exonCount = 0
		self.exonStarts = 0
		self.exonEnds = 0
		self.id = 0
		self.name2 = ''
		self.cdsStartStat = ''
		self.cdsEndStat = ''
		self.exonFrames = []

def sortCompareGenes(gene1, gene2):
	''' compare the genes according to their chromosome/position. '''
	assert(gene1.chr==gene2.chr)
	return gene1.start - gene2.start

def sortCompareGenesByEnd(gene1, gene2):
	''' compare the genes according to their chromosome/position. '''
	assert(gene1.chr==gene2.chr)
	return gene1.end - gene2.end

def inGeneRange(pos, gene):
	assert gene.start<=gene.end
	if pos<gene.start:
		return -1	#pos is before range of gene
	if pos>gene.end:
		return 1;	#pos is after range of gene
	return 0		#pos is in range of gene

def binary_search(p, l, fun, lo=0, hi=None):
	''' binary_search for any list and any item '''
	if hi is None:
		hi = len(l)-1
	if hi< 0:
		return -1;
	mid = (lo+hi)/2	
	while lo <= hi:	
		result = fun(p, l[mid])
		if (result < 0):
			hi = mid-1
		elif (result > 0):
			lo = mid+1
		else:
			return mid #found the matched item.
		mid = (lo+hi)/2
	return -1  #can not find match item.

def outputGeneInfo(pos, gene):
	sys.stdout.write( gene.name+'\t' + gene.name2+'_'+str(pos-gene.start)+'\t') 
	for i in range(gene.exonCount):
		if (gene.exonEnds[i]>=pos):
			if (gene.exonStarts[i] <= pos):
				#it is in the range of an exon(1 based).
				print 'exon'+str(i+1)+'_' + str(pos-gene.exonStarts[i])+'\t'
				return
			else:
				#it is an intron just after the ith exon(1 based).
				print 'intron'+str(i)+'_'+ str(pos-gene.exonEnds[i-1])+'\t'
				return 
	assert(0) # should not reach here!		
		

geneTable = {'chr1':[],'chr2':[],'chr3':[],'chr4':[],'chr5':[],'chr6':[],'chr7':[],'chr8':[],'chr9':[],\
             'chr10':[],'chr11':[],'chr12':[],'chr13':[],'chr14':[],'chr15':[],'chr16':[],\
             'chr17':[],'chr18':[],'chr19':[],'chrX':[],'chrY':[],'chrM':[],\
             'chr1_random':[],'chr2_random':[],'chr3_random':[],'chr4_random':[],'chr5_random':[],\
             'chr6_random':[],'chr7_random':[],'chr8_random':[],'chr9_random':[],'chr10_random':[],\
             'chr11_random':[],'chr12_random':[],'chr13_random':[],'chr14_random':[],'chr15_random':[],\
             'chr16_random':[],'chr17_random':[],'chr18_random':[],'chr19_random':[],\
             'chrX_random':[],'chrY_random':[],'chrM_random':[],'chrUn_random':[],'*':[],'zzzzzz':[]}


fGeneDB = open(sys.argv[3], 'r');
#strGeneDB = freadline(fGeneDB) #this is the file title, need to skip it.
strGeneDB = freadline(fGeneDB)
while len(strGeneDB) > 0:
	strGene = strGeneDB.split('\t');
	gene = Gene(); 
	gene.name = strGene[0];
	gene.chr = strGene[1];
	gene.strand = strGene[2]
	gene.start = int(strGene[3])
	gene.end = int(strGene[4])
	gene.cdsStart = int(strGene[5])
	gene.cdsEnd = int(strGene[6])
	gene.exonCount = int(strGene[7])
	gene.exonStarts = strGene[8].split(',')
	gene.exonEnds = strGene[9].split(',')
	gene.id = strGene[10]
	gene.name2 = strGene[11]
	gene.cdsStartStat = strGene[12]
	gene.cdsEndStat = strGene[13]
	gene.exonFrames = strGene[14].split(',')
	#convert them into integers from strings.
	for i in range(gene.exonCount):
		gene.exonStarts[i] = int(gene.exonStarts[i])
		gene.exonEnds[i] = int(gene.exonEnds[i])
		gene.exonFrames[i] = int(gene.exonFrames[i])	
	geneTable[gene.chr].append(gene)#add this gene into the geneTable.
	strGeneDB = freadline(fGeneDB)
	
#sort every list in the geneTable
for geneList in geneTable.itervalues():
	geneList.sort(sortCompareGenes)
	
geneTableByEnd = copy.deepcopy(geneTable)	
#sort every list in the geneTableByEnd by the end position of every gene.
#we do this because gene regions can overlap with each other. 
for geneList in geneTableByEnd.itervalues():
	geneList.sort(sortCompareGenesByEnd)
	
field = int(sys.argv[2]) #which field is chr, and field+1 is the position.	
if (sys.argv[1] == '-'):
    fInput = sys.stdin
else:
    fInput = open(sys.argv[1], 'r');
strInput = freadline(fInput).strip('\n')
while len(strInput) > 0:
    if (strInput[0] != '@'):  #it is not a comment line.
        sys.stdout.write(strInput) #print out without a return or space.    
        strInput = strInput.split('\t')
        chrname = strInput[field].strip()
        pos = int(strInput[field+1])
        index = binary_search(pos, geneTable[chrname], inGeneRange)
        if index >= 0: #search by start position
            outputGeneInfo(pos, geneTable[chrname][index])
        else:   #search by end position
    	    index = binary_search(pos, geneTableByEnd[chrname], inGeneRange)
    	    if index >= 0:
                outputGeneInfo(pos, geneTableByEnd[chrname][index])
            else:
                print 'NA\tNA\tNA\t'         
    strInput = freadline(fInput).strip('\n')  #read next line

fGeneDB.close()
fInput.close()

