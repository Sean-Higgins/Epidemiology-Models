/*
 * SIInDZ.h - A header file for the zombieSIInDZ.cpp program
 *            to store the declarations and definitions of the most
 *            important funcations and variables for the program.
 */

// print debugging messages?
//#define DEBUG

// setting the number of threads:
#ifndef NUMT
#define NUMT    6
#endif


// Define the start and end year for the simulation.
#ifndef START_YEAR
#define START_YEAR  2023
#endif

#ifndef END_YEAR
#define END_YEAR    2025
#endif

//#define CSV

// Since the below variables are shared between zombieSIInDZ.cpp and SIInDZ.c,
// we need to declare them as extern variable so that they can be used in all the
// files that make up this program. The variables are declared here in the header
// file, are assigned their default values in the SIInDZ.c file, and can be accessed
// and changed in the zombieSIInDZ.cpp file.

// Year and month for the simulation to keep track of.
extern int NowYear;	    // [START_YEAR, END_YEAR]
extern int NowMonth;	// [0, 11]

// Starting number of susceptible people, infected people, and recovered people.
extern long CurrentSusceptible;
extern long CurrentImmune;
extern long CurrentInfected;
extern long CurrentZombies;
extern long CurrentDead;


// Transfer rates for the zombie outbreak model.
// Rate of infection for the zombie virus.
extern double InfectionRate;
// Rate of zombification for infected individuals
extern double ZombieRate;
// Rate of Susceptible, Infected, and Immune individuals being killed by Zombies
extern double DeathRate;
// Rate of Zombies being killed by Susceptible, Infected, and Immune individuals
extern double ZombieDeathRate;



// parseLong: This function serves as a wrapper for the function strtol(),
//            ensuring that the provided character string str produces
//            a long integer that meets our criteria.
//  Input:
//      long* long_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      long integer
void parseLong(long* long_val, char* str);


// parseDouble: This function serves as a wrapper for the function strtod(),
//              ensuring that the provided character string str produces
//              a double that meets our criteria.
//  Input:
//      double* double_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      double
void parseDouble(double* double_val, char* str);


// Function prototypes for the functions stored in the SIInDZ.c file.
void Susceptible();
void Infected();
void Immune();
void Zombies();
void Dead();
void Watcher();
