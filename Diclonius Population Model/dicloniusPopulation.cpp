/*
 * Program Name: dicloniusPopulation.cpp
 * Summary: This program uses multithreading and parallel calculations
 *			to simulate the growth of the Diclonius population from the anime
 *			and manga series Elfen Lied.
 * Programmer: Sean B. Higgins
 * Start Date: September 26, 2023
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
#define NUMT	5
#endif

//#define CSV

// Define other values to be used that might be adjusted
// for the Diclonius population's growth. The current values are
// equivalent to those in Japan.

// Year and month for the simulation to keep track of.
int StartYear = 2005;
int NowYear = StartYear;    // [NowYear, EndYear]
int NowMonth = 0;	    // [0, 11]
int EndYear = 2025;

// Starting number of uninfected humans, active carriers of the
// Vector Virus, Diclonius Queens and Silphalets.
int CurrentHumans =     173000;
int CurrentCarriers = 	10;
int CurrentQueens = 	1;
int CurrentSilphalets =	35;

// Transfer rates for the Diclonius population model.
// 
const float INFECTION_RATE = 0.4;
// For simplicities sake, we will use the same birth-rate for all
// of our population groups. This might be subject to change.
// This birthrate is based on Japan's average birthrate for the year
// 2023. 7.013 per 1000 people.
const float BIRTH_RATE_HUMANS = 7.013;
const float BIRTH_RATE_SILPHALETS = 7.013;
const float BIRTH_RATE_QUEENS = 7.013;

// This function calculates how many new uninfected human children
// there will be for the next generation of the simulation. This rate of
// population increase is proportional to the current population of
// uninfected humans. We also need to calculate how many people have become
// infected with the Vector Virus, thus making them Carriers.
void Humans() {
    int nextHumans;

    while( NowYear < EndYear ) {
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
    	nextHumans = CurrentHumans;

    	// Calculate the population of uninfected humans.
    	// Subtract the number of new infections based on the current number
    	// of infected individuals.
    	// Add the number of new uninfected human babies based on the current
    	// population of uninfected humans.
    	nextHumans += (CurrentHumans/1000 * BIRTH_RATE_HUMANS/12) - (CurrentQueens + CurrentSilphalets) * INFECTION_RATE;

	// We can't have a negative population
	if( nextHumans < 0 )
            nextHumans = 0;
		
	// DoneComputing barrier:
    	#pragma omp barrier
		
    	// Store the newly calculated population value after all the parallel functions
    	// have finished their own calculatons.
    	CurrentHumans = nextHumans;

    	// DoneAssigning barrier:
    	#pragma omp barrier	

    	// DonePrinting barrier:
    	#pragma omp barrier
    }
}

// This function calculates the number of infected individuals there will be
// for the next generation of the simulation. This depends on the number of
// uninfected humans available to be infected, as well as the number of
// Queens and Silphalets around to spread the Vector Virus.
void Carriers() {
    int nextCarriers;

    while( NowYear < EndYear ) {
    	// compute a temporary next-value for this quantity
    	// based on the current state of the simulation:
    	nextCarriers = CurrentCarriers;

        // Add the new number of infected indivuduals and subtract
    	// the number of recovered individuals.
    	nextCarriers += (CurrentQueens + CurrentSilphalets) * INFECTION_RATE;

    	// DoneComputing barrier:
    	#pragma omp barrier
	
    	// Store the newly calculated value for the next generation of the simulation.
    	CurrentCarriers = nextCarriers;

    	// DoneAssigning barrier:
    	#pragma omp barrier	

    	// DonePrinting barrier:
    	#pragma omp barrier
    }
}


// This function calculates the number of Diclonius Queens there are.
// The population of Diclonius queens depends on the number of current Queens.
// Also, Queens will only birth Queens, not uninfected humans, carriers, or Silphalets.
// Since the Diclonius race is all female, the birth of the next generation depends on
// the current Queen population, as well as the current male population (uninfected humans
// and carriers).
void Queens() {
    int nextQueens = CurrentQueens;
	
    while( NowYear < EndYear ) {
    	// Compute a temporary next-value for the number of recovered individuals
    	// based on the current number of infected individuals.
		
    	nextQueens += ((CurrentQueens + CurrentHumans/2 + CurrentCarriers/2)/1000) * BIRTH_RATE_QUEENS/12;

    	// DoneComputing barrier: Save the calculated variables to the global variables.
    	#pragma omp barrier
		
    	CurrentQueens = nextQueens;

    	// DoneAssigning barrier:
    	#pragma omp barrier

    	// DonePrinting barrier:
    	#pragma omp barrier		
    }
}


// This function calculates the number of Diclonius Silphalets there are.
// Silphalets are sterile, so the population of Silphalets depends on the number of carriers
// and uninfected humans.
void Silphalets() {
    int nextSilphalets = CurrentSilphalets;
	
    while( NowYear < EndYear ) {
    	// Compute a temporary next-value for the number of recovered individuals
    	// based on the current number of infected individuals.
		
    	nextSilphalets += ((CurrentHumans + CurrentCarriers)/1000) * BIRTH_RATE_SILPHALETS/12;
		
    	// DoneComputing barrier: Save the calculated variables to the global variables.
    	#pragma omp barrier
		
    	CurrentSilphalets = nextSilphalets;

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

    while( NowYear < EndYear ) {
    	// DoneComputing barrier:
    	#pragma omp barrier

    	// DoneAssigning barrier:
    	#pragma omp barrier
		
    	// Print the current values for the simulation.
    	#ifdef CSV
            // Calculate the current month number for graphing purposes.
            int yearDiff = NowYear - StartYear;
            int addMonths = 12*yearDiff;
            int printMonth = NowMonth+addMonths;

            fprintf(stderr, "%2d, %d, %d, %d, %d\n",
                printMonth, CurrentHumans, CurrentCarriers, CurrentQueens, CurrentSilphalets);
        #else
            fprintf(stderr, "Year %4d, Month %2d - Uninfected Humans: %d, Carriers: %d, Diclonius Queens: %d, Silphalets: %d\n",
                NowYear, NowMonth+1, CurrentHumans, CurrentCarriers, CurrentQueens, CurrentSilphalets);
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
    // Accept input values from the command line for this simulation.
    if (argc == 5) {
        CurrentHumans = atoi(argv[1]);
        CurrentCarriers = atoi(argv[2]);
        CurrentQueens = atoi(argv[3]);
        CurrentSilphalets = atoi(argv[4]);
    } else {
        fprintf(stderr, "No command-line arguments provided for %s.\nRunning the program using default values:\n\
            \tHumans = %d\n\
            \tCarriers = %d\n\
            \tQueens = %d\n\
            \tSilphalets = %d\n\n",\
            CurrentHumans, CurrentCarriers, CurrentQueens, CurrentSilphalets);
    }

    omp_set_num_threads(NUMT);	// same as # of sections
    #pragma omp parallel sections
    {
    	#pragma omp section
	{
            Humans();
        }

    	#pragma omp section
    	{
            Carriers();
        }
	
    	#pragma omp section
    	{
            Queens();
    	}

    	#pragma omp section
    	{
	    Silphalets();
    	}

        #pragma omp section
    	{
    	    Watcher();
    	}

    }   // implied barrier -- all functions must return in order
	// to allow any of them to get past here
}
