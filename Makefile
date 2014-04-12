#REFER = mm9.fa

CC = gcc
CXX = g++

CFLAGS = -Wall -Werror -O2
#CFLAGS = -g -Wall -Werror

TARGETS= DiploidConstructor MappingConvertor

all: $(TARGETS)

#we MUST set this rule to keep all intermediate files!!!
.SECONDARY:

MappingConvertor: MappingConvertor.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^

DiploidConstructor: DiploidConstructor.cpp CLineFields1.cpp
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	find . -maxdepth 1 -type l -exec rm -rf {} \;
	rm -rf *~ 
	awk '{if( ($$1 != "all:") && ($$1 != "clean:") && (substr($$1,1,1)!="#") && (substr($$1,1,1)!="$$") && (substr($$1,length($$1),1)==":") ) { targetname=substr($$1,1, length($$1)-1); sub("%","*", targetname); cmd="rm -rf "targetname; system(cmd); }}' ${MAKEFILE_LIST} 

