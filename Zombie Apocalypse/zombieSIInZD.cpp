/*
 * Program Name: zombieSIInZD.cpp
 * Summary: This program uses multithreading and parallel calculations
 *	    to simulate the progress of a zombie outbreak on a human population.
 *          The program uses a modified SIR epidemiology model to make these
 *          predictions, here called a SIInZD model for:
 *              (S)usceptible
 *              (I)mune
 *              (In)fected
 *              (Z)ombies
 *              (D)ead
 *          This model was originally designed by Austin Hourigan in his excellent
 *          video "The SCIENCE! Behind the Zombie Apocalypse"
 *          URL: https://www.youtube.com/watch?v=VdO4_C9LGFA
 * Programmer: Sean B. Higgins
 * Start Date: September 15, 2023
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#include "zombieSIInZD.h"


// The main loop of the program.
int main(int argc, char* argv[]) {	
    // First parameter is the initial population.
    if (argc >= 2)
        int Susceptible = atoi(argv[2]);
    // The second parameter, if one is provided, is the fraction
    // of people who are infected.
    if (argc >= 3) {
        int Infected = Susceptible * atof(argv[3])
        Susceptible -= Infected;
    }

    printf("Num Susceptible: %d\n", Susceptible);
    printf("Num Infected: %d\n", Infected);
    printf("Percentage of Infected: %4.2f\n", float(Infected/Susceptible));

    omp_set_num_threads(NUMT);	// same as # of sections
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Susceptible();
        }

        #pragma omp section
        {
            Infected();
        }
	
        #pragma omp section
        {
            Immune();
        }

        #pragma omp section
        {
            Zombies();
        }

        #pragma omp section
        {
            Dead();
        }

        #pragma omp section
        {
            Watcher();
        }

    }   // implied barrier -- all functions must return in order
        // to allow any of them to get past here
}
