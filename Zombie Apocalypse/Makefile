# This makefile handles the compilation of the epidemiology models for
# influenza.

.PHONY: all clean prep debug release

EXE := zombieOutbreak

# Debug directory and executable
DBGDIR := debug
DBGEXE := $(DBGDIR)/$(EXE)

RELDIR := release
RELEXE := $(RELDIR)/$(EXE)

all: prep debug release

# Clean up any previous builds
clean:
	rm -rf debug/ release/

# Prepare by creating the debug and release directories
prep:
	mkdir -p $(DBGDIR) $(RELDIR)


# Compile the debug version of the code
debug: prep zombieSIInZD.cpp inputParsing.c SIInZD.c zombieSIInZD.h
	g++ -DDEBUG -g -fopenmp -lm zombieSIInZD.cpp inputParsing.c SIInZD.c -o $(DBGEXE)


# Compile the release version of the program without any debugging features.
release: prep zombieSIInZD.cpp inputParsing.c SIInZD.c zombieSIInZD.h
	g++ -fopenmp -lm zombieSIInZD.cpp inputParsing.c SIInZD.c -o $(RELEXE)
