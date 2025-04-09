/*
 * SIR.c - This function stores all of the differential functions that are
 *         used with the functional multiprocessing method to produce our SIR model.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SIR.h"

// Define other values to be used that might be adjusted
// for the flu outbreak. The current values are equivalent
// to those in the United States of America.

// These variables are initialized and assigned their values in the influenzaSIR.cpp
// program, but we still need them to be within the scope of the SIR.c file.
// Thus, they are declared here, but they are not initialized.

// Year and month for the simulation to keep track of.
/*
int NowYear = 0;			// [0, Num_Years]
int NumYears = NUM_YEARS;
int NowMonth = 0;			// [0, 11]
*/
int NowDays = 0;			// [0, 364]
int MaxDays = 100;			// The maximum number of days to run the simulation for.

// Starting number of susceptible people, infected people, and recovered people.
long CurrentSusceptible = 10000;
long CurrentInfected = 10;
long CurrentRecovered = 0;

/*
double CurrentSusceptible = 10000.0;
double CurrentInfected = 10.0;
double CurrentRecovered = 0.0;
*/

// Transfer rates for the SIR model.
// Rate of infection for the common cold.
double InfectionRate = 0.0001;
// Rate of recovery for the common cold.
double RecoveryRate = 0.1;


/* Susceptible: This function is executed by a thread in parallel with the
 *              Infected(), Recovered(), and Watcher() functions. It serves
 *              to calculate the next value of the Susceptible population,
 *              as the Susceptible become Infected.
 */
void Susceptible() {
    //long nextSusceptible;
	double nextSusceptible;

    while( NowDays < MaxDays ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextSusceptible = CurrentSusceptible;

    	// Subtract the number of new infections based on the current number
        // of infected individuals.
        // IMPORTANT: While it is mathematically correct to find the change in
        //            the Susceptible population by multiplying
        //            CurrentSusceptible *
    	nextSusceptible -= round(InfectionRate * CurrentSusceptible * CurrentInfected);
	
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

/* Infected: This function calculates the number of infected individuals there
 *			 will be for the next generation of the simulation. This depends on
 *			 the number of susceptible individuals available to be infected, as
 *			 well as the number of infected individuals that have recovered.
 */
void Infected() {
    //long nextInfected;
	double nextInfected;
	
    while( NowDays < MaxDays ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextInfected = CurrentInfected;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextInfected += round(InfectionRate * CurrentSusceptible * CurrentInfected);
        nextInfected -= round(CurrentInfected * RecoveryRate);

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


/* Recovered: This function calculates the number of individuals who have
 *			  recovered from their flu infection. This depends on the number
 *			  of people who are currently infected.
 */
void Recovered() {
    //long nextRecovered;
	double nextRecovered;

    while( NowDays < MaxDays ) {
		// Compute a temporary next-value for the number of recovered individuals
        // based on the current number of infected individuals.

    	nextRecovered += round(CurrentInfected * RecoveryRate);

		// We still cannot have a negative population
		if (nextRecovered < 0)
			nextRecovered = 0;
		
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

// Watcher: This function adjusts the global variables for the program.
void Watcher() {
    while( NowDays < MaxDays ) {

	// DoneComputing barrier:
	#pragma omp barrier

	// DoneAssigning barrier:
	#pragma omp barrier

	// Print the current values for the simulation.
#ifdef CSV
        // Calculate the current month number for graphing purposes.
        //int addMonths = 12*NowYear;
        //int printMonth = NowMonth+addMonths;

        fprintf(stderr, "%4d, %6ld, %6ld, %6ld\n",
                NowDays, CurrentSusceptible, CurrentInfected, CurrentRecovered);

#else
        fprintf(stderr, "Day %4d - Susceptible: %6ld, Infected: %6ld, Recovered: %6ld\n",
                NowDays+1, CurrentSusceptible, CurrentInfected, CurrentRecovered);
#endif

#ifdef DEBUG
        fprintf(stderr, "Total Population: %6ld\n",
                CurrentSusceptible + CurrentInfected + CurrentRecovered);
#endif

		// Compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		/*
		tempYear = NowYear;
		tempMonth = NowMonth + 1;

		if (tempMonth > 11) {
			tempMonth = 0;
			tempYear++;
		}
		*/

		// Store the new environment variables for the simulation.
		//NowMonth = tempMonth;
		//NowYear = tempYear;

		// Move the calculation to the next day.
		NowDays++;

	// DonePrinting barrier:
	#pragma omp barrier
    }
}
