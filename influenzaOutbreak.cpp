/*
 * Program Name: influenzaModel.cpp
 * Summary: This program uses multithreading and parallel calculations
 *			to simulate the progress of a flu outbreak on a human population
 *                      using the SIR epidemiology model.
 * Programmer: Sean B. Higgins
 * Start Date: September 15, 2023
 */

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// print debugging messages?
#ifndef DEBUG
#define DEBUG	false
#endif

// setting the number of threads:
#ifndef NUMT
#define NUMT	4
#endif


// Define the start and end year for the simulation.
#ifndef START_YEAR
#define START_YEAR	2023
#endif

#ifndef END_YEAR
#define END_YEAR	2029
#endif

//#define CSV

// Define other values to be used that might be adjusted
// for the flu outbreak. The current values are equivalent
// to those in the United States of America.

// Year and month for the simulation to keep track of.
int NowYear = START_YEAR;	// [START_YEAR, END_YEAR]
int NowMonth = 0;			// [0, 11]

// Starting number of susceptible people, infected people, and recovered people.
int CurrentInfected = 10;
int CurrentSusceptible = 175000 - CurrentInfected;
int	CurrentRecovered = 0;

// Transfer rates for the zombie outbreak model.
// Rate of infection for the common cold.
float InfectionRate = 0.4;
// Rate of recovery for the common cold.
float RecoveryRate = 0.04;


// For use with calculating the Carrying Capacity
float Sqr( float x )
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
	int nextSusceptible;
	
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
	int nextInfected;
	
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
	int nextRecovered = CurrentRecovered;
	
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
			
			fprintf(stderr, "%2d, %d, %d, %d\n",
					printMonth, CurrentSusceptible, CurrentInfected, CurrentRecovered);
		#else
			fprintf(stderr, "Year %4d, Month %2d - Susceptible: %d, Infected: %d, Recovered: %d\n",
					NowYear, NowMonth+1, CurrentSusceptible, CurrentInfected, CurrentRecovered);
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

// The main loop of the program.
int main(int argc, char* argv[]) {
        // Gather the initial values from the command-line
        if (argc >= 2) {
            CurrentSusceptible = atoi(argv[1]);
            CurrentInfected = atoi(argv[2]);
        }
        if (argc >= 4) {
            
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
