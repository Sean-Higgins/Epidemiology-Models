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
    // Gather the initial values (if any) from the command-line
    // If command-line arguments are provided, they are parsed in with
    // the appropriate flags:
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
                    fprintf(stderr, "Usage: %s [-s susceptible] [-I immune] [-i infected] [-z zombies] [-a infection-rate] [-b zombification-rate] [-g death-rate] [-d zombie-death-rate]\n",
                        argv[0]);
                    exit(EXIT_FAILURE);
 
                }
                
                switch (c) {
                    case 's':   // -s: Initial susceptible value. Value must be a
                                // whole positive integer.
                        parseLong(&CurrentSusceptible, str);
                        break;
		    case 'I':	// -I: Initial immune value. Value must be a whole
				// positive integer.
			parseLong(&CurrentImmune, str);
			break;
		    case 'i':   // -i: Initial infected value. Value must be a whole
                                // positive integer.
                        parseLong(&CurrentInfected, str);
                        break;
		    case 'z':   // -z: Initial zombie value. Value must be a whole
                                // positive integer.
                        parseLong(&CurrentZombies, str);
                        break;
		    case 'a':   // -a: Rate of infection (from susceptible to
                                //      infected). Must be a non-negative value.
                        parseDouble(&InfectionRate, str);
                        break;
		    case 'b':	// -b: Rate of zombification (from infected to
				// zombie). Must be a non-negative value.
			parseDouble(&ZombieRate, str);
			break;
                    case 'g':   // -g: Death rate (from
				// susceptible/infected/immune to Dead).
                                // Must be a non-negative value.
                        parseDouble(&DeathRate, str);
                        break;
		    case 'd':	// -d: Zombie death rate (from Zombie to Dead)
				// Must be a non-negative value
			parseDouble(&ZombieDeathRate, str);
			break;
                    default:    // If the provided argument flag is invalid, print a
                                // message explaining the flag is invalid. Then print
                                // the usage message.
                        fprintf(stderr, "Error: The provided flag of \"%s\" is invalid.\n", argv[--i]);
			fprintf(stderr, "Usage: %s [-s susceptible] [-I immune] [-i infected] [-z zombies] [-a rate-of-infection] [-b rate-of-zombification] [-g death-rate]\n",
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
    CurrentSusceptible -= CurrentImmune;
    CurrentSusceptible -= CurrentZombies;

    // IMPORTANT: Before we begin our calculations, we need to print out the initial
    // population values.
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
