# SID_Models
A repository to store all of my epidemiological predictive models, including a basic SIR model and it's modifications. This repository also has some other fun models, including the SIInZD model for zombie outbreaks, and a model to predict the population growth of the Diclonious race from Elfen Lied. These models are written in C++ and use OpenMP.
When compiling these models, we need to use a C++ compiler, and we also need to be sure to link the Math and OpenMP libraries as well. On Linux, the compilation command will look like this:
    g++ -lm -fopenmp proj.c -o proj
