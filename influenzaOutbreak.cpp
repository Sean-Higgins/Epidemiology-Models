/*
 * Program Name: influenzaModel.cpp
 * Summary: This program uses multithreading and parallel calculations
 *			to simulate the progress of a flu outbreak on a human population
 *                      using the SIR epidemiology model.
 * Programmer: Sean B. Higgins
 * Start Date: September 15, 2023
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// print debugging messages?
#ifndef DEBUG
#define DEBUG   false
#endif

// setting the number of threads:
#ifndef NUMT
#define NUMT    4
#endif


// Define the start and end year for the simulation.
#ifndef START_YEAR
#define START_YEAR  2023
#endif

#ifndef END_YEAR
#define END_YEAR    2024
#endif

//#define CSV

// Define other values to be used that might be adjusted
// for the flu outbreak. The current values are equivalent
// to those in the United States of America.

// Year and month for the simulation to keep track of.
int NowYear = START_YEAR;	// [START_YEAR, END_YEAR]
int NowMonth = 0;			// [0, 11]

// Starting number of susceptible people, infected people, and recovered people.
long CurrentInfected = 10;
long CurrentSusceptible = 175000;   // - CurrentInfected; The CurrentInfected is
                                    // subtracted in main
long CurrentRecovered = 0;

// Transfer rates for the zombie outbreak model.
// Rate of infection for the common cold.
double InfectionRate = 0.4;
// Rate of recovery for the common cold.
double RecoveryRate = 0.04;


// For use with calculating the Carrying Capacity
double Sqr( double x )
{
    return x*x;
}

// Seed the random number function.
unsigned int seed = 0;

float Randf(unsigned int *seedp, float low, float high) {
    float r = (float) rand_r(seedp);
    return (low + r*(high - low) / (float)RAND_MAX);
}


// This function calculates how many (or rather, how few) susceptible humans
// there will be for the next generation of the simulation. This rate of
// decrease is proportional to the current population zombies.
void Susceptible() {
    long nextSusceptible;

    while( NowYear < END_YEAR ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextSusceptible = CurrentSusceptible;

    	// Subtract the number of new infections based on the current number
        // of infected individuals.
    	nextSusceptible -= CurrentInfected * InfectionRate;
	
        // We can't have a negative population
    	if( nextSusceptible < 0 )
            nextSusceptible = 0;
		
    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentSusceptible = nextSusceptible;

    	// DoneAssigning barrier:
    	#pragma omp barrier		

    	// DonePrinting barrier:
    	#pragma omp barrier		
    }
}

// This function calculates the number of infected individuals there will be
// for the next generation of the simulation. This depends on the number of
// susceptible individuals available to be infected, as well as the number of
// infected individuals that have recovered.
void Infected() {
    long nextInfected;
	
    while( NowYear < END_YEAR ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextInfected = CurrentInfected;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextInfected += (CurrentSusceptible * InfectionRate);
        nextInfected -= (CurrentInfected * RecoveryRate);

    	// We still cannot have a negative population
	if (nextInfected < 0)
            nextInfected = 0;

    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentInfected = nextInfected;

    	// DoneAssigning barrier:
    	#pragma omp barrier

    	// DonePrinting barrier:
    	#pragma omp barrier
    }
}


// This function calculates the number of individuals who have recovered from
// their flu infection. This depends on the number of people who are currently
// infected.
void Recovered() {
    long nextRecovered = CurrentRecovered;

    while( NowYear < END_YEAR ) {
	// Compute a temporary next-value for the number of recovered individuals
        // based on the current number of infected individuals.

    	nextRecovered += (CurrentInfected * RecoveryRate);
		
    	// DoneComputing barrier: Save the calculated variables to the
    	// global variables.
    	#pragma omp barrier
    	CurrentRecovered = nextRecovered;

    	// DoneAssigning barrier:
    	#pragma omp barrier

    	// DonePrinting barrier:
    	#pragma omp barrier		
    }
}

// This function adjusts the global variables for the program.
void Watcher() {
    int tempMonth;
    int tempYear;

    while( NowYear < END_YEAR ) {

	// DoneComputing barrier:
	#pragma omp barrier

	// DoneAssigning barrier:
	#pragma omp barrier

	// Print the current values for the simulation.
	#ifdef CSV
	    // Calculate the current month number for graphing purposes.
	    int yearDiff = NowYear - START_YEAR;
            int addMonths = 12*yearDiff;
	    int printMonth = NowMonth+addMonths;

            fprintf(stderr, "%2d, %ld, %ld, %ld\n", printMonth, CurrentSusceptible, CurrentInfected, CurrentRecovered);
	
        #else
            fprintf(stderr, "Year %4d, Month %2d - Susceptible: %6ld, Infected: %6ld, Recovered: %6ld\n", NowYear, NowMonth+1, CurrentSusceptible, CurrentInfected, CurrentRecovered);
        #endif
	
	// Compute a temporary next-value for this quantity
	// based on the current state of the simulation:
        tempYear = NowYear;
	tempMonth = NowMonth + 1;

	if (tempMonth > 11) {
            tempMonth = 0;
	    tempYear++;
        }

        // Store the new environment variables for the simulation.
        NowMonth = tempMonth;
	NowYear = tempYear;

	// DonePrinting barrier:
	#pragma omp barrier
    }
}

// parseLong: This function serves as a wrapper for the function strtol(),
//            ensuring that the provided character string str produces
//            a long integer that meets our criteria.
//  Input:
//      long* long_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      long integer
void parseLong(long* long_val, char* str) {
    errno = 0;      // Set errno to 0 so that when strtol() is called, we can
                    // distinguish between a success and a failure.
    char* endptr;

    // Convert the value in str to a long using strtol()
    *long_val = strtol(str, &endptr, 0);
    
    // Error checking. If errno is not 0, something went wrong
    // with strtol()
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    // The stored value of endptr points to the end of the
    // numeric section of str, so if str == endptr, that means
    // there was never a numeric section to str!
    if (str == endptr) {
        fprintf(stderr, "strtol: No digits provided for the initial value.\n");
        exit(EXIT_FAILURE);
    }

    // Finally, we need to make sure that the provided value is
    // not less than 0.
    if (*long_val < 0) {
        fprintf(stderr, "strtol: The provided value of %ld is invalid\n", long_val);
        exit(EXIT_FAILURE);
    }
    
    // If everything looks good however, we can store the
    // provided converted long into the provided long pointer.
}

// parseDouble: This function serves as a wrapper for the function strtod(),
//              ensuring that the provided character string str produces
//              a double that meets our criteria.
//  Input:
//      double* double_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      double
void parseDouble(double* double_val, char* str) {
    errno = 0;      // Set errno to 0 so that when strtod() is called, we can
                    // distinguish between a success and a failure.
    char* endptr;

    // Convert the value in str to a double using strtod()
    *double_val = strtod(str, &endptr);

    // Error checking. If errno is not 0, then something went
    // wrong with strtod()
    if (errno != 0) {
        perror("strtod");
        exit(EXIT_FAILURE);
    }

    // The stored value of endptr points to the end of the
    // numeric section of str, so if str == endptr, that means
    // there was never a numeric section to str
    if (str == endptr) {
        fprintf(stderr, "strtod: No digits provided for the value.\n");
        exit(EXIT_FAILURE);
    }

    // Finally, we need to make sure that the provided value is
    // not less than 0.
    if (*double_val < 0) {
        fprintf(stderr, "strtod: The provided value of %f is invalid.\n", double_val);
        exit(EXIT_FAILURE);
    }

    // If everything looks good however, we can store the
    // provided converted double into the provided double pointer.
}


// The main loop of the program.
int main(int argc, char* argv[]) {
    // Gather the initial values (if any) from the command-line
    // If command-line arguments are provided, they are parsed in with
    // the appropriate flags:
    //  -
    if (argc >= 2) {
        // Parse all of the provided arguments and their values.
        // IMPORTANT: We also need to handle error cases where a user
        // forgets to inter a value (e.g., ./influenzaOutbreak -s)
        // This leaves argv[argc-1] as the flag, and the final argv[argc] as NULL.

        // Iterate through the argument vector to locate the flags.
        int c, errno;
        char *endptr, *str;
        double double_val;  // Store values from strtod() (i.e., the rates)

        for (int i = 1; i < argc; ++i) {
            // If the current argument is an option, determine which option
            // and store the provided value to the corresponding variable
            if (argv[i][0] == '-') {
                c = argv[i][1]; // Get the option character

                str = argv[++i];    // Store the pointer that holds the number value
                                    // to convert.

                // IMPORTANT: If the user didn't actually provide an argument after
                // one of the argument flags, that will result in str being given a
                // nullptr. If we try to run strtol() or strtod() on a nullptr, we
                // get a segfault. 
                if (str == nullptr) {
                    fprintf(stderr, "Error: No argument was provided for option flag \"%s\".\n", argv[--i]);
                    fprintf(stderr, "Usage: %s [-s susceptible] [-i infected] [-a rate-of-infection] [-r rate-of-recovery]\n",
                        argv[0]);
                    exit(EXIT_FAILURE);
 
                }
                
                switch (c) {
                    case 's':   // -s: Initial susceptible value. Value must be a
                                // whole positive integer.
                        parseLong(&CurrentSusceptible, str);
                        break;
                    case 'i':   // -i: Initial infected value. Value must be a whole
                                // positive integer.
                        parseLong(&CurrentInfected, str);
                        break;
                    case 'a':   // -a: Rate of infection (from susceptible to
                                //      infected). Must be a non-negative value.
                        parseDouble(&InfectionRate, str);
                        break;
                    case 'r':   // -r: Rate of recovery (from infected to recovered).
                                // Must be a non-negative value.
                        parseDouble(&RecoveryRate, str);
                        break;
                    default:    // If the provided argument flag is invalid, print a
                                // message explaining the flag is invalid. Then print
                                // the usage message.
                        fprintf(stderr, "Error: The provided flag of \"%s\" is invalid.\n", argv[--i]);
                        fprintf(stderr, "Usage: %s [-s susceptible] [-i infected] [-a rate-of-infection] [-r rate-of-recovery]\n",
                                 argv[0]);
                        exit(EXIT_FAILURE);
                }

                // Once the user-provided values have been parsed (if they were
                // provided), we need to subtract the number of infected from the
                // beginning population of CurrentSusceptible
                CurrentSusceptible -= CurrentInfected;
            }
        }
    }

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
            Recovered();
	}
		
	#pragma omp section
	{
            Watcher();
	}
		
    }   // implied barrier -- all functions must return in order
	// to allow any of them to get past here
}
