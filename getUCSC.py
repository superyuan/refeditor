#! /usr/bin/env python
import sys
import urllib2

#print sys.argv[0]
#print sys.argv[1]
#print 

ucscID = sys.argv[1]

url1 = 'http://genome.ucsc.edu/cgi-bin/hgTracks?clade=mammal&org=Mouse&db=mm9&position=' + \
    ucscID + '&hgt.suggest=&pix=800&Submit=submit&hgsid=191029117'

response = urllib2.urlopen(url1);
data = response.read()
start = data.find("&c=")
end = data.find('&db=', start)
condition = data[start:end]
url2= 'http://genome.ucsc.edu/cgi-bin/hgc?hgsid=190338791&g=htcDnaNearGene&i=' + \
  ucscID + condition+'&o=knownGene&boolshad.hgSeq.promoter=0&hgSeq.promoterSize=1000&hgSeq.utrExon5=on&boolshad.hgSeq.utrExon5=0&hgSeq.cdsExon=on&boolshad.hgSeq.cdsExon=0&hgSeq.utrExon3=on&boolshad.hgSeq.utrExon3=0&hgSeq.intron=on&boolshad.hgSeq.intron=0&boolshad.hgSeq.downstream=0&hgSeq.downstreamSize=1000&hgSeq.granularity=gene&hgSeq.padding5=10&hgSeq.padding3=10&boolshad.hgSeq.splitCDSUTR=0&hgSeq.casing=exon&boolshad.hgSeq.maskRepeats=0&hgSeq.repMasking=lower&submit=submit'
#print url2
response = urllib2.urlopen(url2);
data = response.read()
data = data.replace('\n<PRE>\n', '').replace('</PRE>', '')
#print data
#output to the corresponding file.
fout = open(ucscID, "w")
fout.write(data)
fout.close()

sys.exit(1)

