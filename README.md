Citation
==============

If you use this software, please cite this paper:

Shuai Y, Zhaohui Q: *Read-mapping using personalized diploid reference genome for RNA sequencing data reduced bias for detecting allele-specific expression.* In _Bioinformatics and Biomedicine Workshops (BIBMW), 2012 IEEE International Conference on; 4-7 Oct. 2012._ 2012: 718-724.

You can download citation here (Right click -> Save as...):
* [diploid_mapping.ris](https://github.com/superyuan/refeditor/blob/master/diploid_mapping.ris) (compatible with EndNote, Reference Manager, ProCite, RefWorks)
* [diploid_mapping.bibtex](https://github.com/superyuan/refeditor/blob/master/diploid_mapping.bibtex) (compatible with BibDesk, LaTeX)

Contacts
==============
Shuai Yuan,  Email: [shuaiyuan@emory.edu]

----

 
Downloads
==============
Latest Version
--------------
Please use right panel to download source code.

Old Versions
--------------
Not provided.

Minimum System Requirement:
==============
CPU 2.0G Hz

Memory 8 GB

Hard disk 50 GB free space

Linux 64-bit or MacOS 64-bit, gcc, Read Mapping Tools (e.g. BWA or Bowtie).

 
Introduction
==============

This software package is to improve read mapping accuracy for diploid individuals.  

It has two major programs: DiploidConstructor and MappingConvertor.  

DiploidConstructor converts a haploid reference genome to a diploid reference genome according to the genotypes and sequencing read length.  

MappingConvertor converts the SAM file created from being mapped to a diploid reference genome to being mapped to a haploid reference genome according to a dipmap file.  

DiploidConstructor should be executed before calling read mapping tools (BWA, Bowtie etc.).  

MappingConvertor should be executed after calling read mapping tools.

 
Build & Install
==============

   Download and uncompress this software, and then:
   
    cd refeditor
    make

then you can copy compiled executables to anywhere in your PATH variable ( e.g  ~/bin/ ).

 
Run
==============
DiploidConstructor
--------------
Usage: 

    DiploidConstructor -r haploid.fa -g genotypes -l length -o diploid.fa

Convert haploid reference genome to diploid reference genome according to the genotypes and sequencing read length

Example: 

    ./DiploidConstructor -r hg19.fa -g known.genotypes -l 36 -o diploid.fa

Parameters (mandatory): 

    -r    haploid reference genome file 

    -g    genotypes file

    -l    read length

    -o    output diploid reference genome file 

Parameters (optional): 

    -d    maximal deletion in a read and mapped to alternative alleles [default=0]

    -s    sex of the individual [default="m"]

MappingConvertor
--------------
Usage:

    MappingConvertor -i input.sam -m dipmap -o output.sam

Convert the sam file created from being mapped to a diploid reference genome to being mapped to a haploid reference genome according to a dipmap file.

Example: 

    ./MappingConvertor -i input.sam -m hg19.di.fa.dipmap -o output.sam

Parameters (mandatory): 

    -i    input SAM file 

    -m    dipmap file created by DiploidConstructor

    -o    output SAM file


An example
==============

Read mapping using RefEditor
--------------

1. Compile 

        cd refeditor
        make

2. Create genotypes file for target individual

        ./vcf2genotypes test.vcf NA19238 > test.genotypes


3. Create customized reference genome

        ./DiploidConstructor -r hg19test.fa -g test.genotypes -l 36 -o hg19test.di.fa

    That will create two files: hg19test.di.fa and hg19test.di.fa.dipmap

4. Map the reads against the diploid reference genome using BWA (Version: 0.7.5a-r405)

    Create index for reference genome

        bwa index -a bwtsw hg19test.di.fa

    Perform alignment

        bwa aln hg19test.di.fa test.fastq > test.sai
        bwa samse -n 16 hg19test.di.fa test.sai test.fastq > test.sam

 
5. Convert intermediate SAM file to final result SAM file

        ./MappingConvertor -i test.sam -m hg19test.di.fa.dipmap -o test.final.sam


Done!  Now the reads in the test.fastq file are mapped successfully and stored in test.final.sam file. 

 
Read mapping using traditional method without RefEditor
--------------
1. Map the reads against the haploid reference genome using BWA (Version: 0.7.5a-r405)

   1.1 Create index for reference genome

    bwa index -a bwtsw hg19test.fa

    1.2 Make alignment

    bwa aln hg19test.fa test.fastq > test.ha.sai
    bwa samse -n 16 hg19test.fa test.ha.sai test.fastq > test.ha.sam


Done!  Now the mapping results are stored in test.ha.sam file.  By viewing this file, you will find the mapping quality is 0 for all reads.  That's because traditional method does not take into account the genotypes information

 
Conclusion
--------------
Using RefEditor for read mapping can improve mapping accuracy and downstream analysis.
 
----

Revisions
==============
v1.3_r17
--------------
  Add test.vcf and vcf2genotypes 

v1.2_r16
--------------
  Update the example 

v1.1_r13
--------------
  Add files that are needed in the example
  Create release tags automatically

v1.0_r7
--------------
  First working release
