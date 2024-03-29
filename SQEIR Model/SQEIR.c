/*
 * SQEIR.c - This function stores all of the differential functions that are
 *	     used with the functional multiprocessing method to produce our SQEIR model.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SQEIR.h"

// Define other values to be used that might be adjusted
// for the flu outbreak. The current values are equivalent
// to those in the United States of America.

// These variables are initialized and assigned their values in the influenzaSIR.cpp
// program, but we still need them to be within the scope of the SIR.c file.
// Thus, they are declared here, but they are not initialized.

// Year and month for the simulation to keep track of.
int NowYear = START_YEAR;	// [START_YEAR, END_YEAR]
int NowMonth = 0;			// [0, 11]

// Starting number of susceptible people, quarantined people, exposed people, infected people, and recovered people.
long CurrentSusceptible = 175000;
long CurrentQuarantined = 0;
long CurrentExposed = 0;
long CurrentInfected = 10;
long CurrentRecovered = 0;

// Transfer rates for the SIR model.
// Rate of infection for the common cold.
double InfectionRate = 0.4;
// Rate of recovery for the common cold.
double RecoveryRate = 0.04;


/* Susceptible: This function is executed by a thread in parallel with the
 *              Infected(), Recovered(), and Watcher() functions. It serves
 *              to calculate the next value of the Susceptible population,
 *              as the Susceptible become Infected.
 */
void Susceptible() {
    long nextSusceptible;

    while( NowYear < END_YEAR ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextSusceptible = CurrentSusceptible;

    	// Subtract the number of new infections based on the current number
        // of infected individuals.
        // IMPORTANT: While it is mathematically correct to find the change in
        //            the Susceptible population by multiplying
        //            CurrentSusceptible *
    	nextSusceptible -= round(CurrentSusceptible * InfectionRate);
	
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

/* Quarantined: This function is executed by a thread in parallel with the
 *              Infected(), Recovered(), and Watcher() functions. It serves
 *              to calculate the next value of the Susceptible population,
 *              as the Susceptible become Infected.
 */
void Quarantined() {
    long nextQuarantined;

    while( NowYear < END_YEAR ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextQuarantined = CurrentQuarantined;

    	// Subtract the number of new infections based on the current number
        // of infected individuals.
        // IMPORTANT: While it is mathematically correct to find the change in
        //            the Quarantined population by multiplying
        //            CurrentQuarantined *
    	nextQuarantined -= round(CurrentQuarantined * InfectionRate);
	
        // We can't have a negative population
    	if( nextQuarantined < 0 )
            nextQuarantined = 0;
		
    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentQuarantined = nextQuarantined;

    	// DoneAssigning barrier:
    	#pragma omp barrier		

    	// DonePrinting barrier:
    	#pragma omp barrier		
    }
}

/* Exposed: This function is executed by a thread in parallel with the
 *          Infected(), Recovered(), and Watcher() functions. It serves
 *          to calculate the next value of the Susceptible population,
 *          as the Susceptible become Infected.
 */
void Exposed() {
    long nextExposed;

    while( NowYear < END_YEAR ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextExposed = CurrentExposed;

    	// Subtract the number of new infections based on the current number
        // of infected individuals.
        // IMPORTANT: While it is mathematically correct to find the change in
        //            the Exposed population by multiplying
        //            CurrentExposed *
    	nextExposed -= round(CurrentExposed * InfectionRate);
	
        // We can't have a negative population
    	if( nextExposed < 0 )
            nextExposed = 0;
		
    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentExposed = nextExposed;

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
    long nextInfected;
	
    while( NowYear < END_YEAR ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextInfected = CurrentInfected;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextInfected += round(CurrentSusceptible * InfectionRate);
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
    long nextRecovered = CurrentRecovered;

    while( NowYear < END_YEAR ) {
	// Compute a temporary next-value for the number of recovered individuals
        // based on the current number of infected individuals.

    	nextRecovered += round(CurrentInfected * RecoveryRate);
		
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

        fprintf(stderr, "%2d, %ld, %ld, %ld\n",
                printMonth, CurrentSusceptible, CurrentInfected, CurrentRecovered);

#else
        fprintf(stderr, "Year %4d, Month %2d - Susceptible: %6ld, Infected: %6ld, Recovered: %6ld\n",
                NowYear, NowMonth+1, CurrentSusceptible, CurrentInfected, CurrentRecovered);
#endif

#ifdef DEBUG
        fprintf(stderr, "Total Population: %6d\n",
                CurrentSusceptible + CurrentInfected + CurrentRecovered);
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
