# Set up compiler
CC=g++
# Compiler options:
# -c - compile and assemble only
# -Wall - assemble all files
CLIBS=-lfreetype -lfontconfig -ljpeg -lz -lssl -lidn -lcrypto
SOURCES=pdf-spot-disabler.cpp getopt/getopt_pp.cpp
EXECUTABLE=pdf-spot-disabler
TESTFILE=test.pdf
OUTFILE=processed.pdf

all:
	$(CC) $(SOURCES) /usr/local/lib/libpodofo.a -o $(EXECUTABLE) $(CLIBS)

test:
	./$(EXECUTABLE) $(TESTFILE) $(OUTFILE)

clean:
	rm -rf *o $(EXECUTABLE)