# This makefile handles the compilation of the epidemiology models for
# COVID-19.

.PHONY: all clean prep debug release

EXE := COVID

# Debug directory and executable
DBGDIR := debug
DBGEXE := $(DBGDIR)/$(EXE)

RELDIR := release
RELEXE := $(RELDIR)/$(EXE)

all: prep debug release

# Clean up any previous builds
clean:
	rm -rf $(DBGDIR) $(RELDIR)

# Prepare by creating the debug and release directories
prep:
	mkdir -p $(DBGDIR) $(RELDIR)


# Compile the debug version of the code
debug: prep SQEIR.cpp inputParsing.c SQEIR.c SQEIR.h
	g++ -DDEBUG -g -fopenmp -lm SQEIR.cpp inputParsing.c SQEIR.c -o $(DBGEXE)


# Compile the release version of the program without any debugging features.
release: prep SQEIR.cpp inputParsing.c SQEIR.c SQEIR.h
	g++ -fopenmp -lm SQEIR.cpp inputParsing.c SQEIR.c -o $(RELEXE)
