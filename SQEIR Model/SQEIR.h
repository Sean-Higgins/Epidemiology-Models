/*
 * SQEIR.h - A header file for the SQEIR.cpp program to store
 *	         the declarations and definitions of the most
 *           important funcations and variables for the program.
 */

// print debugging messages?
//#define DEBUG

// setting the number of threads:
#ifndef NUMT
#define NUMT    6
#endif


// Define the start and end year for the simulation.
#ifndef NUM_YEARS
#define NUM_YEARS   2
#endif

//#define CSV

// Since the below variables are shared between influenzaSIR.cpp and SIR.c,
// we need to declare them as extern variable so that they can be used in all the
// files that make up this program. The variables are declared here in the header
// file, are assigned their default values in the SIR.c file, and can be accessed
// and changed in the influenzaSIR.cpp file.

// Year and month for the simulation to keep track of.
extern int NowYear;	    // [START_YEAR, END_YEAR]
extern int NumYears;
extern int NowMonth;	// [0, 11]

// Starting population of each group in the SQEIR epidemic model.
extern long CurrentSusceptible;
extern long CurrentQuarantined;
extern long CurrentExposed;
extern long CurrentInfected;
extern long CurrentRecovered;

extern double InfectionRate;
extern double RecoveryRate;

// parseLong: This function serves as a wrapper for the function strtol(),
//            ensuring that the provided character string str produces
//            a long integer that meets our criteria.
//  Input:
//      long* long_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      long integer
void parseLong(long* long_val, char* str);


// parseInt: This is a wrapper function for the parseLong function that includes
//	     a check to make sure that the provided value will fit into an integer
//	     value.
//  Input:
//	int* int_val: A pointer to the variable where we will store the converted
//		      str value.
//	char* str:    The string that holds the numeric data to convert to an
//		      integer.
void parseInt(int* int_val, char* str);


// parseDouble: This function serves as a wrapper for the function strtod(),
//              ensuring that the provided character string str produces
//              a double that meets our criteria.
//  Input:
//      double* double_val: A pointer to the variable where we will be storing our
//                      converted str.
//      char* str:      The string that holds the numeric data to convert to a
//                      double
void parseDouble(double* double_val, char* str);


// Function prototypes for the functions stored in the SQEIR.c file.
void Susceptible();
void Quarantined();
void Exposed();
void Infected();
void Recovered();
void Watcher();
