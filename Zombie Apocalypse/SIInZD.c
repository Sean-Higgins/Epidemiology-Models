/*
 * SIInZD.c - This file stores all of the differential functions that are used
 *	      with the functional multiprocessing method to produce our SIInDZ model.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "zombieSIInZD.h"

// Define other values to be used that might be adjusted
// for the flu outbreak. The current values are equivalent
// to those in the United States of America.

// These variables are initialized and assigned their values in the zombieSIInZD.cpp
// program, but we still need them to be within the scope of the SIInZD.c file.
// Thus, they are declared here, but they are not initialized.

// Year and month for the simulation to keep track of.
int NowYear = START_YEAR;	// [START_YEAR, END_YEAR]
int NowMonth = 0;			// [0, 11]

// Starting number of susceptible people, infected people, and recovered people.
long CurrentSusceptible = 175000;
long CurrentImmune = 100;
long CurrentInfected = 10;
long CurrentZombies = 10;
long CurrentDead = 0;


// Transfer rates for the zombie outbreak model.
// Rate of infection for the zombie virus.
double InfectionRate = 0.4;
// Rate of zombification for infected individuals
double ZombieRate = 0.04;
// Rate of Susceptible, Infected, and Immune individuals being killed by Zombies
double DeathRate = 0.25;
// Rate of zombies being killed by Susceptible, Infected, and the Immune
double ZombieDeathRate = 0.5;


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
    	nextSusceptible -= round(CurrentSusceptible * (InfectionRate + DeathRate));
	
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
 *	     will be for the next generation of the simulation. This depends on
 *	     the number of susceptible individuals available to be infected, as
 *	     well as the number of infected individuals that have recovered.
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
        nextInfected -= round(CurrentInfected * (ZombieRate + DeathRate));

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

/* Immune: This function calculates the number of immune individuals there
 *	   will be for the next generation of the simulation. This depends on
 *	   the number of zombies available to kill the immune population.
 */
void Immune() {
    long nextImmune;
	
    while( NowYear < END_YEAR ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextImmune = CurrentImmune;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextImmune -= round(CurrentImmune * DeathRate);

    	// We still cannot have a negative population
	if (nextImmune < 0)
            nextImmune = 0;

    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentImmune = nextImmune;

    	// DoneAssigning barrier:
    	#pragma omp barrier

    	// DonePrinting barrier:
    	#pragma omp barrier
    }
}

/* Zombies: This function calculates the number of zombies there
 *	    will be for the next generation of the simulation. This depends on
 *	    the number of Susceptible, Infected, and Immune individuals there are
 *	    who are killing zombies. It also depends on the number of Infected
 *	    individuals who turn into zombies.
 */
void Zombies() {
    long nextZombies;
	
    while( NowYear < END_YEAR ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextZombies = CurrentZombies;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextZombies += round(CurrentInfected * ZombieRate);
        nextZombies -= round(CurrentZombies * ZombieDeathRate);

    	// We still cannot have a negative population
	if (nextZombies < 0)
            nextZombies = 0;

    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentZombies = nextZombies;

    	// DoneAssigning barrier:
    	#pragma omp barrier

    	// DonePrinting barrier:
    	#pragma omp barrier
    }
}

/* Dead: This function calculates the number of individuals that have died
 *	 for the next generation of the simulation. This depends on
 *	 the number of zombies available to kill the various populations, as
 *	 well as the various other populations killing the zombies.
 */
void Dead() {
    long nextDead;
	
    while( NowYear < END_YEAR ) {
        // compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextDead = CurrentDead;

    	// Add the new number of infected indivuduals and subtract
        // the number of recovered individuals.
        nextDead += round(CurrentSusceptible * DeathRate);
        nextDead += round(CurrentInfected * DeathRate);
	nextDead += round(CurrentImmune * DeathRate);
	nextDead += round(CurrentZombies * ZombieDeathRate);

    	// We still cannot have a negative population
	if (nextDead < 0)
            nextDead = 0;

    	// DoneComputing barrier:
    	#pragma omp barrier
    	CurrentDead = nextDead;

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

	fprintf(stderr, "%2d, %ld, %ld, %ld, %ld, %ld\n", printMonth, CurrentSusceptible,
		CurrentImmune, CurrentInfected, CurrentZombies, CurrentDead);
#else
	fprintf(stderr, "Year %4d, Month %2d - Susceptible: %6ld, Immune: %6ld, Infected: %6ld, Zombies: %6ld, Dead: %6ld\n",
		NowYear, NowMonth+1, CurrentSusceptible, CurrentImmune, CurrentInfected,
		CurrentZombies, CurrentDead);
#endif

	// For debugging, print the total population as we go.
#ifdef DEBUG
	fprintf(stderr, "Total Population: %6ld\n",
		CurrentSusceptible + CurrentImmune + CurrentInfected + CurrentZombies + CurrentDead);
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
