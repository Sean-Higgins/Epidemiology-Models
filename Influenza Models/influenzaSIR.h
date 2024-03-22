/*
 * influenzaSIR.h - A header file for the influenzaSIR.cpp program
 *                  to store declarations and definitions for the program.
 */




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
