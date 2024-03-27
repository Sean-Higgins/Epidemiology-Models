# This makefile handles the compilation of the epidemiology models for
# influenza.

.PHONY: all clean prep debug release test

EXE := influenza

# Debug directory and executable
DBGDIR := debug
DBGEXE := $(DBGDIR)/$(EXE)
debug: CFLAGS += -DDEBUG -g

RELDIR := release
RELEXE := $(RELDIR)/$(EXE)
release: CFLAGS += -03

all: debug release

clean:
    rm -rf debug/ release/

prep:
    @mkdir -p $(DBGDIR) $(RELDIR)

debug: prep $(DBGEXE)

# Compile the debug version of the code
debug: influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.h
    @g++ -DDEBUG -g -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o influenza

# Compile the program without any debugging features.
release: influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.handles
    @g++ -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o influenza