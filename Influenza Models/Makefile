# This makefile handles the compilation of the epidemiology models for
# influenza.

.PHONY: all clean prep debug release

EXE := influenza

# Debug directory and executable
DBGDIR := debug
DBGEXE := $(DBGDIR)/$(EXE)

# Release directory and executable
RELDIR := release
RELEXE := $(RELDIR)/$(EXE)

all: prep debug release


# Clean up and remove any previously generated executable files, if they exist
clean:
	rm -rf $(DBGDIR) $(RELDIR)

# Prepare the debug and release directories
prep:
	mkdir -p $(DBGDIR) $(RELDIR)


# Compile the debug version of the code
debug: prep influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.h
	g++ -DDEBUG -g -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o $(DBGEXE)


# Compile the program without any debugging features.
release: prep influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.h
	g++ -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o $(RELEXE)

