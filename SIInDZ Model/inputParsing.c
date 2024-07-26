/*
 * inputParsing.c - A basic file to hold some very useful functions for input
 *                  parsing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

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
        fprintf(stderr, "strtol: The provided value of %ld is invalid\n", *long_val);
        exit(EXIT_FAILURE);
    }
    
    // If everything looks good however, we can store the
    // provided converted long into the provided long pointer.
}

// parseInt: This is a wrapper function for the parseLong function that includes
//	     a check to make sure that the provided value will fit into an integer
//	     value.
//  Input:
//	int* int_val: A pointer to the variable where we will store the converted
//		      str value.
//	char* str:    The string that holds the numeric data to convert to an
//		      integer.
void parseInt(int* int_val, char* str) {
    
    long temp = 0;  // A long integer to store the value that is converted from str.
    
    parseLong(&temp, str);

    // Now check to see if the long value temp is within the maximum int value.
    if (temp > INT_MAX) {
	fprintf(stderr, "parseInt: The provided value of %ld is too large to store as an integer\n", temp);
	exit(EXIT_FAILURE);
    }

    // If the temp value is smaller than INT_MAX, we can store it in the provided
    // integer value.
    *int_val = (int)temp;

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
        fprintf(stderr, "strtod: The provided value of %f is invalid.\n", *double_val);
        exit(EXIT_FAILURE);
    }

    // If everything looks good however, we can store the
    // provided converted double into the provided double pointer.
}

