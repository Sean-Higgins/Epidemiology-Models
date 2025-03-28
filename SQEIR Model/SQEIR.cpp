/*
 * Program Name: SQEIR.cpp
 * Summary: This program uses multithreading and parallel calculations
 *          to simulate the progress of any bacterial or viral outbreak
 *          on a human population, making sure to take into account
 *          the incubation state of the desease, high transmission potential,
 *          and quarantine measures to reduce the spread of the disease. Taking
 *          these extra requirements into consideration, we can thus derive a new
 *          epidemiological model: the SQEIR model. The relevent components are as
 *          listed below:
 *              (S)usceptible
 *              (Q)uarantined
 *              (E)xposed
 *              (I)nfected
 *              (R)ecovered
 *          This model was first introduced, as far as I am aware, as a way to model
 *          the spread of the COVID-19 virus in the research paper "SQEIR: An epidemic
 *          virus spread analysis and prediction model"
 *          URL: https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9364756/
 * Programmer: Sean B. Higgins
 * Start Date: March 30, 2024
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#include "SQEIR.h"


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
        int c;
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
                    fprintf(stderr, "Usage: %s [-s susceptible] [-q quarantined] [-e exposed] [-i infected] [-b rate-of-infection] [-g rate-of-recovery]\n",
                        argv[0]);
                    exit(EXIT_FAILURE);
 
                }
                
                switch (c) {
		    // Initial Population Values:
                    case 's':   // -s: Initial susceptible value. Value must be a
                                // whole positive integer.
                        parseLong(&CurrentSusceptible, str);
                        break;
		    case 'q':	// -q: Initial quarantined value. Value must be a
				// whole positive integer.
			parseLong(&CurrentQuarantined, str);
			break;
		    case 'e':	// -e: Initial exposed value. Value must be a whole
				// positive integer.
			parseLong(&CurrentExposed, str);
			break;
                    case 'i':   // -i: Initial infected value. Value must be a whole
                                // positive integer.
                        parseLong(&CurrentInfected, str);
                        break;

		    // Initial rates.
                    case 'b':   // -b: Rate of infection (from susceptible to
                                //      infected). Must be a non-negative value.
                        parseDouble(&InfectionRate, str);
                        break;
                    case 'g':   // -g: Rate of recovery (from infected to recovered).
                                // Must be a non-negative value.
                        parseDouble(&RecoveryRate, str);
                        break;
		    case 'y':	// -y: Years
			parseInt(&NumYears, str);
			break;
                    default:    // If the provided argument flag is invalid, print a
                                // message explaining the flag is invalid. Then print
                                // the usage message.
                        fprintf(stderr, "Error: The provided flag of \"%s\" is invalid.\n", argv[--i]);
                        fprintf(stderr, "Usage: %s [-s susceptible] [-q quarantined] [-e exposed] [-i infected] [-b rate-of-infection] [-g rate-of-recovery]\n",
                                 argv[0]);
                        exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Once the user-provided values have been parsed (if they were
    // provided), we need to subtract the number of infected from the
    // beginning population of CurrentSusceptible
    CurrentSusceptible -= CurrentInfected;

    // IMPORTANT: Before we begin our calculations, we need to print out the initial
    // population values of each of the groups, Susceptible, Quarantined, Exposed,
    // Infected, and Recovered (SQEIR).

#ifdef CSV
        // Calculate the current month number for graphing purposes.
        int yearDiff = NowYear - START_YEAR;
        int addMonths = 12*NowYear;
        int printMonth = NowMonth+addMonths;

        fprintf(stderr, "%2d, %ld, %ld, %ld, %ld, %ld\n",
                printMonth, CurrentSusceptible, CurrentQuarantined, CurrentExposed, CurrentInfected, CurrentRecovered);

#else
        fprintf(stderr, "Year %4d, Month %2d - Susceptible: %6ld, Quarantiened: %6ld, Exposed: %6ld, Infected: %6ld, Recovered: %6ld\n",
                NowYear, NowMonth+1, CurrentSusceptible, CurrentQuarantined, CurrentExposed, CurrentInfected, CurrentRecovered);
#endif

    // For debugging, print the total population to ensure that no individual is lost in the calculations due to rounding.
#ifdef DEBUG
    fprintf(stderr, "Total Population: %6ld\n",
            CurrentSusceptible + CurrentInfected + CurrentRecovered);
#endif

    // Increment to the next month to begin our calculations.
    NowMonth++;

    omp_set_num_threads(NUMT);	// same as # of sections
    #pragma omp parallel sections
    {
    
        #pragma omp section
        {
            Susceptible();
        }

        #pragma omp section
        {
            Quarantined();
        }

        #pragma omp section
        {
            Exposed();
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
