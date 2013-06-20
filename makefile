# Set up compiler
CC=g++
# Compiler options:
# -c - compile and assemble only
# -Wall - assemble all files
CLIBS=-lpodofo -lfreetype -lfontconfig -ljpeg -lz -lssl -lidn -lcrypto
SOURCES=pdf-spot-disabler.cpp
EXECUTABLE=pdf-spot-disabler
TESTFILE=test.pdf

all:
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(CLIBS)

test:
	./$(EXECUTABLE) $(TESTFILE)

clean:
	rm -rf *o $(EXECUTABLE)