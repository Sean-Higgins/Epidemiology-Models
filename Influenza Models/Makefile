# This makefile handles the compilation of the epidemiology models for
# influenza.

# Compile the debug version of the code
debug: influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.h
    g++ -DDEBUG -g -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o influenzaDEBUG

# Compile the program without any debugging features.
release: influenzaSIR.cpp inputParsing.c SIR.c influenzaSIR.handles
    g++ -fopenmp -lm influenzaSIR.cpp inputParsing.c SIR.c -o influenza

# Clean up and remove any previously generated executable files, if they exist
clean:
    rm -f influenza influenzaDEBUG
