#REFER = mm9.fa

CC = gcc
CXX = g++

CFLAGS = -Wall -Werror -O2
#CFLAGS = -g -Wall -Werror

DB = imprinting
USERNAME = mysql

allSNPs_GENENAMES = allSNPs.Gatm allSNPs.H13 allSNPs.Blcap allSNPs.GNAS allSNPs.Calcr\
 allSNPs.Tfpi2 allSNPs.Casd1 allSNPs.Ppp1r9a allSNPs.Pon3 allSNPs.Pon2 allSNPs.Asb4\
 allSNPs.Dlx5 allSNPs.Copg2 allSNPs.Klf14 allSNPs.Zim2 allSNPs.Zim3 allSNPs.Ube3a\
 allSNPs.Ampd3 allSNPs.H19 allSNPs.Th allSNPs.Ascl2 allSNPs.Tspan32 allSNPs.Cd81\
 allSNPs.Tssc4 allSNPs.Kcnq1 

#all: commonMatrix.txt $(allSNPs_GENENAMES) allSNPs.count.intron_exon_NA allSNPs.around allSNPs.PValue
TARGETS= only.file1 join1file join2files join3files maskSNP DiploidConstructor MappingConvertor readsAllele genotypext compareSamTrue haploid2diploid generateReads join2sortedfiles expand.genotypext join5files union.files overlap.files rotate sort.chroms cut.fastq


all: $(TARGETS)

install: $(TARGETS)
	cp -rf $(TARGETS) ~/bin/


#we MUST set this rule to keep all intermediate files!!!
.SECONDARY:


$(REFER).fai: $(REFER)
	samtools faidx $^
%.bam: %.bam.input $(REFER).fai
	samtools view -bt $(REFER).fai $< > $@~  &&  mv $@~ $@
%.sorted.bam: %.bam
	samtools sort $^ $(patsubst %.bam,%.tmp,$@) && mv $(patsubst %.bam,%.tmp.bam,$@) $@
%.pileup.snp: %.pileup.my
	awk '{if ($$6+$$7+$$8+$$9>$$10) print $$0}' $^ > $@~ && mv $@~ $@ 
#%.pileup.snp: %.pileup.all
#	awk '{if(toupper($$3) != toupper($$4)) print $$0}' $^ > $@~ && mv $@~ $@
%.pileup.my: %.sam.quality $(REFER)
	pileup $^ -sam > $@~ && mv $@~ $@
#%.pileup.all: $(REFER) %.sorted.bam
#	samtools pileup -cf $^ > $@~  &&  mv $@~ $@
%.sam: $(REFER) %.sai %.fq
	bwa samse $^ > $@~ && mv $@~ $@
%.sai: %.fq $(REFER) $(REFER).bwt
	bwa aln $(REFER) $<  > $@~ && mv $@~ $@
%.bwt: %
	bwa index -a bwtsw $<


genotypesCoverage: genotypesCoverage.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
mergeGenotypesFrom2Alleles: mergeGenotypesFrom2Alleles.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
alterref: alterref.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
cut.fastq: cut.fastq.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
sort.chroms: sort.chroms.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
genotypext: genotypext.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
maskSNP: maskSNP.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
pileup: pileup.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
join.fasta.hapmap: join.fasta.hapmap.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
tmp: tmp.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
GenomeMapperToSam: GenomeMapperToSam.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
MappingConvertor: MappingConvertor.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
readsAllele: readsAllele.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
alnSNPsInfo: alnSNPsInfo.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -g -o $@ $^
fastakit: fastakit.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
compareSamTrue: compareSamTrue.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
DiploidConstructor: DiploidConstructor.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^
generateReads: generateReads.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^
haploid2diploid: haploid2diploid.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
only.file1: only.file1.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
join2files: join2files.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
join2sortedfiles: join2sortedfiles.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
join1file: join1file.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
join3files: join3files.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
join5files: join5files.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
union.files: union.files.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
overlap.files: overlap.files.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
expand.genotypext: expand.genotypext.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
rotate: rotate.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
rotate_high_file: rotate_high_file.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
rotate_wide_file: rotate_wide_file.cpp CLineFields.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 

allSNPs.PValue: allSNPs.OL.DiffS1S2 ../addPValue.py
	../addPValue.py $< > $@~  &&  mv $@~ $@

allSNPs.around: ../lookupAround.py allSNPs.OL.DiffS1S2.filter.GeneName.NoNA allSNPs.GeneName
	../lookupAround.py allSNPs.OL.DiffS1S2.filter.GeneName.NoNA allSNPs.GeneName > $@~ && mv $@~ $@

allSNPs.count.intron_exon_NA: ../countKeyLines.py allSNPs.GeneName
	../countKeyLines.py allSNPs.GeneName intron > $@~ 
	../countKeyLines.py allSNPs.GeneName exon >> $@~
	../countKeyLines.py allSNPs.GeneName NA >> $@~  &&  mv $@~ $@

allSNPs.OL.DiffS1S2.filter.GeneName.NoNA: allSNPs.OL.DiffS1S2.filter.GeneName
	grep -v -i NA $^ > $@~  &&  mv $@~ $@

allSNPs.OL.DiffS1S2.filter.GeneName: ../addGeneName.py allSNPs.OL.DiffS1S2.filter ../mm9genes.txt
	../addGeneName.py allSNPs.OL.DiffS1S2.filter 0 ../mm9genes.txt > $@~  &&  mv $@~ $@

allSNPs.OL.DiffS1S2.filter: allSNPs.OL.DiffS1S2
	awk '{if (($$8>=0.8)&&($$9>=0.8)) print $$0}' $^ > $@~  &&  mv $@~ $@

allSNPs.OL.DiffS1S2: allSNPs.OL.rsNo
	awk '{if (($$4 != $$5)&&($$4 != "NoMajor")&&($$5 != "NoMajor")&&($$4 != "NoInfo")&&($$5 != "NoInfo")) print $$0}' $^ > $@~  &&  mv $@~ $@
	
$(allSNPs_GENENAMES): allSNPs.GeneName
	-grep -i '\<$(patsubst allSNPs.%,%,$@)_' allSNPs.GeneName > $@

%.pileup.filter: ../%.pileup
	samtools.pl varFilter -d $(threshold) -D 999999 $^ > $@~  && mv $@~ $@

commonMatrix.txt: ../commonMutation.py s1.pileup.filter s2.pileup.filter s3.pileup.filter s4.pileup.filter
	../commonMutation.py s1.pileup.filter s2.pileup.filter s3.pileup.filter s4.pileup.filter > $@~  &&  mv $@~ $@

allSNPs.OL.rsNo: allSNPs.OL ../snp_chr.sort.txt ../addRSNo.py
	../addRSNo.py allSNPs.OL ../snp_chr.sort.txt > $@~  &&  mv $@~ $@

allSNPs.OL: ../mergeSNPOL.py allSNPs.txt 
	../mergeSNPOL.py allSNPs.txt > $@~  &&  mv $@~ $@

mergeSNP: mergeSNP.cpp
	$(CXX) $(CFLAGS) -o $@ $^ 
allSNPs.NoSNP.txt: ../mergeSNP s1.pileup.filter s2.pileup.filter s3.pileup.filter s4.pileup.filter
	../mergeSNP s1.pileup.filter s2.pileup.filter s3.pileup.filter s4.pileup.filter > $@~  &&  mv $@~ $@

allSNPs.txt: ../lookupBack.py allSNPs.NoSNP.txt ../s1.pileup.all ../s2.pileup.all ../s3.pileup.all ../s4.pileup.all
	../lookupBack.py allSNPs.NoSNP.txt ../s1.pileup.all ../s2.pileup.all ../s3.pileup.all ../s4.pileup.all > $@~  &&  mv $@~ $@

allSNPs.sorted.txt: allSNPs.txt
	sort -rg -k 10 $^ > $@~  &&  mv $@~ $@

allSNPs.rsNo.txt: allSNPs.txt ../snp_chr.sort.txt ../addRSNo.py
	../addRSNo.py allSNPs.txt ../snp_chr.sort.txt > $@~  &&  mv $@~ $@

allSNPs.GeneName: ../addGeneName.py allSNPs.rsNo.txt ../mm9genes.txt
	../addGeneName.py allSNPs.rsNo.txt 0 ../mm9genes.txt > $@~  &&  mv $@~ $@

html_file_head:
	echo '<html> <head> <title> $(patsubst %.html,%,$(HTMLFILE)) </title> </head> <body>' > $(HTMLFILE)~
$(HTMLFILE): html_file_head Paternal_Imprinting  Maternal_Imprinting Other_Imprinting
	echo '</body> </html>' >> $(HTMLFILE)~
	mv $(HTMLFILE)~ $@
Paternal_Imprinting: Paternal_Imprinting_head $(patsubst %,%.gif,$(PAT_IMPRINT))
Paternal_Imprinting_head:
	echo '<p align="center"><b><font size="5">Paternal Imprinting:</font></b></p>' >> $(HTMLFILE)~ 
Maternal_Imprinting: Maternal_Imprinting_head $(patsubst %,%.gif,$(MAT_IMPRINT))
Maternal_Imprinting_head:
	echo '<p align="center"><b><font size="5">Maternal Imprinting:</font></b></p>' >> $(HTMLFILE)~
Other_Imprinting: Other_Imprinting_head $(patsubst %,%.gif,$(OTHER_IMPRINT))
Other_Imprinting_head:
	echo '<p align="center"><b><font size="5">Other Imprinting:</font></b></p>' >> $(HTMLFILE)~
%.gif:	%.draw
	-gnuplot < $^
	echo '<p>'$@ >> $(HTMLFILE)~; \
	head -n 1 $(patsubst %.gif,%.SNP.plot,$@) | awk '{printf(" (%s)", $$1)}' >> $(HTMLFILE)~; \
	echo '</p>' >> $(HTMLFILE)~; \
	echo '<p> <img border="0" src="'$@'" </p>' >> $(HTMLFILE)~; \

%.average.plot: %.SNP.plot
	echo "select chr,AVG($(CHILD1)_RATIO1),AVG($(CHILD1)_RATIO2),AVG($(CHILD2)_RATIO1),AVG($(CHILD2)_RATIO2),name,name2 from tmpt_snp;" | mysql $(DB) -u $(USERNAME) | tail -n +2 > $@~
	mv $@~ $@

%.draw:	%.average.plot %.exon.plot
	echo "set terminal push" > $@
	echo "set terminal gif size 800,400" >>$@
	echo "set yrange [-0.05:1.05]" >> $@
	echo "set xlabel 'position'" >> $@
	echo "set ylabel 'Mat Percent' " >> $@
	echo "set format x '%.9g' " >> $@
	echo "f(x)=0.5" >> $@
	echo "set output '$(patsubst %.draw,%.gif,$@)'" >> $@
	if [ -s $(patsubst %.draw,%.exon.plot,$@) ]; then \
		echo "plot '$(patsubst %.draw,%.exon.plot,$@)' using 8:9:10:11 title '$(patsubst %.draw,%,$@) exon' with vector nohead , '$(patsubst %.draw,%.SNP.plot,$@)' using 2:10 title '$(CHILD1)' with points 3 , '$(patsubst %.draw,%.SNP.plot,$@)' using 2:17 title '$(CHILD2)' with points 10, '$(patsubst %.draw,%.SNP.plot,$@)' using 2:($$""10+$$""17)/2 title 'avg' with linespoints 11, f(x) title 'middle line' " >> $@ ;  \
	else \
		echo "plot '$(patsubst %.draw,%.SNP.plot,$@)' using 2:10 title '$(CHILD1)' with points 3 , '$(patsubst %.draw,%.SNP.plot,$@)' using 2:17 title '$(CHILD2)' with points 10, '$(patsubst %.draw,%.SNP.plot,$@)' using 2:($$""10+$$""17)/2 title 'avg' with linespoints 11, f(x) title 'middle line'">> $@ ; \
	fi
%.SNP.plot:
	echo "drop table if exists tmpt_snp;" | mysql $(DB) -u $(USERNAME)
	echo "create table tmpt_snp like $(TABLE);" | mysql $(DB) -u $(USERNAME)
	echo "ALTER TABLE tmpt_snp ADD PRIMARY KEY (chr, position);" | mysql $(DB) -u $(USERNAME)
	echo "insert ignore into tmpt_snp select * from $(TABLE) where name2='$(patsubst %.SNP.plot,%,$@)';" | mysql $(DB) -u $(USERNAME)
	#echo "create table tmpt_snp as (select * from $(TABLE) where name2='$(patsubst %.SNP.plot,%,$@)');" | mysql $(DB) -u $(USERNAME)
	echo "select * from tmpt_snp;" | mysql $(DB) -u $(USERNAME) | tail -n +2 > $@~
	sort -g -k 2 $@~ > $@
	rm -rf $@~
%.exon.plot: %.SNP.plot
	echo "drop table if exists tmpt_exon3; drop table if exists tmpt_exon2; drop table if exists tmpt_exon1;" | mysql $(DB) -u $(USERNAME)
	echo "create table tmpt_exon1 as (select * from ref_exon where ref_exon.name2='$(patsubst %.exon.plot,%,$@)' and ref_exon.is_exon=1);" | mysql $(DB) -u $(USERNAME)
	echo "create table tmpt_exon2 as (select * from tmpt_exon1 where exists (select 1 from tmpt_snp where tmpt_snp.position>=tmpt_exon1.start));" | mysql $(DB) -u $(USERNAME)
	echo "create table tmpt_exon3 as (select * from tmpt_exon2 where exists (select 1 from tmpt_snp where tmpt_snp.position<=tmpt_exon2.end));" | mysql $(DB) -u $(USERNAME)
	echo "select name, name2, chr, start, end, is_exon, seq_num, start, -0.02, end-start, 0 from tmpt_exon3;"  | mysql $(DB) -u $(USERNAME) | tail -n +2 > $@
	echo "drop table if exists tmpt_exon3; drop table if exists tmpt_exon2; drop table if exists tmpt_exon1; drop table if exists tmpt_snp;" | mysql $(DB) -u $(USERNAME)
#clean:$(SELF_CLEAN)
#	rm -rf *.filter commonMatrix.txt allSNPs.* *.NA *.intron_exon_NA *~
#	rm -rf *.gif *.draw *.plot *.html
#	echo "drop table if exists tmpt_exon3; drop table if exists tmpt_exon2; drop table if exists tmpt_exon1; drop table if exists tmpt_snp;" | mysql $(DB) -u $(USERNAME)
#distclean: clean
#	rm -rf *.fai *.bam *.pileup *.GeneName 

clean:
	find . -maxdepth 1 -type l -exec rm -rf {} \;
	rm -rf *~ 
	awk '{if( ($$1 != "all:") && ($$1 != "clean:") && (substr($$1,1,1)!="#") && (substr($$1,1,1)!="$$") && (substr($$1,length($$1),1)==":") ) { targetname=substr($$1,1, length($$1)-1); sub("%","*", targetname); cmd="rm -rf "targetname; system(cmd); }}' ${MAKEFILE_LIST} 
