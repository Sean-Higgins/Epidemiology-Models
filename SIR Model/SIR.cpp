/*
 * Program Name: SIR.cpp
 * Summary: This program uses multithreading and parallel calculations
 *          to simulate the progress of a flu outbreak on a human population
 *          using the SIR epidemiology model.
 *              (S)usceptible
 *              (I)nfected
 *              (R)ecovered
 * Programmer: Sean B. Higgins
 * Start Date: September 15, 2023
 */

// Implementing a matrix-vector multpiplication method to perform the calculations
// for this epidemiological model.

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Extra header files for OpenCL implementation.
#include <string.h>
#include <ctime>
#include <sys/time.h>
#include <sys/source.h>

#include "SIR.h"
#include "cl.h"
#include "cl_platform.h"

// The matrix-width and the number of work-items per work group:
// Note: The matrices are actually MATWxMATW and work-group sizes
//       are LOCALSIZExLOCALSIZE.

#ifndef MATW
#define MATW    3   // Number of variables to calculate
#endif

#ifndef LOCALSIZE
#define LOCALSIZE   8
#endif

// OpenCL Objects:
cl_platform_id      Platform;
cl_device_id        Device;
cl_kernel           Kernel;
cl_program          Program;
cl_context          Context;
cl_command_queue    CmdQueue;

// Do we want to output the values in csv format?
//#define CSV

// Prepare the host matrix and vectors for calculating the linear system of equations:
// Ax = b.

/*
 * dS/dt = -(InfectionRate)*Susceptible
 * dI/dt = InfectionRate*Susceptible - RecoveryRate*Infected
 * dR/dt = RecoveryRate*Infected
 * 
 * This system of linear equations can be converted to a matrix-vector multiplication:
 * [-InfectionRate      0                   0][ Susceptible ]   [dS/dt]
 * [InfectionRate       -RecoveryRate       0][ Infected    ] = [dI/dt]
 * [0                   RecoveryRate        0][ Recovered   ]   [dR/dt]
 * 
 * Once the change in population has been calculated, it 
 */

double          hA[MATW][MATW];     // This matrix holds the linear equations for the population change
long            hx[MATW];           // Holds population values for run n
long            hb[MATW];           // Holds population values for run n+1

// Open the OpenCL files that have the function for matrix-vector multiplication
// and vector-addition.
const char *    CL_FILE_NAME_1 = {"MatrixVectorMult.cl"};
const char *    CL_FILE_NAME_2 = {"VectorAdd.cl"};

// Function prototypes.
void            SelectOpenclDevice();
char *          Vendor(cl_uint);
char *          Type(cl_device_type);
void            Wait(cl_command_queue);

// The main loop of the program.
int main(int argc, char* argv[]) {
#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not enabled! Exiting program!\n");
    return 1;
#endif

    // Attempt to open the OpenCL kernel programs. There's no point continuing if we can't
    FILE *fp1;
    FILE *fp2;

#ifdef WIN32
	errno_t err1 = fopen_s( &fp1, CL_FILE_NAME_1, "r" );
	errno_t err2 = fopen_s( &fp2, CL_FILE_NAME_2, "r" );
	if ( err1 != 0 || err2 != 0)
#else
	fp1 = fopen( CL_FILE_NAME_1, "r" );
	fp2 = fopen( CL_FILE_NAME_2, "r" );
	if ( fp1 == NULL || fp2 == NULL)
#endif
	{
		fprintf( stderr, "Cannot open OpenCL source file '%s' or '%s'\n", CL_FILE_NAME_1, CL_FILE_NAME_2 );
		return 1;
	}

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
                    fprintf(stderr, "Usage: %s [-s susceptible] [-i infected] [-b rate-of-infection] [-g rate-of-recovery]\n",
                        argv[0]);
                    exit(EXIT_FAILURE);
 
                }
                
                switch (c) {
                    case 's':   // -s: Initial susceptible value. Value must be a
                                // whole positive integer.
                        parseLong(&CurrentSusceptible, str);
                        break;
                    case 'i':   // -i: Initial infected value. Value must be a whole
                                // positive integer.
                        parseLong(&CurrentInfected, str);
                        break;
                    case 'b':   // -b: Rate of infection (from susceptible to
                                //      infected). Must be a non-negative value.
                        parseDouble(&InfectionRate, str);
                        break;
                    case 'g':   // -g: Rate of recovery (from infected to recovered).
                                // Must be a non-negative value.
                        parseDouble(&RecoveryRate, str);
                        break;
                    default:    // If the provided argument flag is invalid, print a
                                // message explaining the flag is invalid. Then print
                                // the usage message.
                        fprintf(stderr, "Error: The provided flag of \"%s\" is invalid.\n", argv[--i]);
                        fprintf(stderr, "Usage: %s [-s susceptible] [-i infected] [-b rate-of-infection] [-g rate-of-recovery]\n",
                                 argv[0]);
                        exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Returned status from OpenCL calls -- Test against CL_SUCCESS
    cl_int status;

    // Get the platform id and the device id:
    SelectOpenclDevice();   // Sets the global variables Platform and Device

    // 2. Allocate the host memory buffers:
    // Already done. The host memory buffers were created as global variables instead of on the heap
    // so that we could allocate them as a 2D array

    // Once the user-provided values have been parsed (if they were
    // provided), we need to subtract the number of infected from the
    // beginning population of CurrentSusceptible
    CurrentSusceptible -= CurrentInfected;

    // IMPORTANT: Before we begin our calculations, we need to print out the initial
    // population values.
#ifdef CSV
    // Calculate the current month number for graphing purposes.
    int yearDiff = NowYear - START_YEAR;
    int addMonths = 12*yearDiff;
    int printMonth = NowMonth+addMonths;

    fprintf(stderr, "%2d, %ld, %ld, %ld\n", printMonth, CurrentSusceptible, CurrentInfected, CurrentRecovered);
#else
    fprintf(stderr, "Year %4d, Month %2d - Susceptible: %6ld, Infected: %6ld, Recovered: %6ld\n",
            NowYear, NowMonth+1, CurrentSusceptible, CurrentInfected, CurrentRecovered);
#endif

    // For debugging, print the total population as we go.
#ifdef DEBUG
    fprintf(stderr, "Total Population: %6ld\n",
            CurrentSusceptible + CurrentInfected + CurrentRecovered);
#endif

    // Increment to the next month to begin our calculations.
    NowMonth++;

    // Initialize the input matrices and vectors.
    /*
     * This system of linear equations can be converted to a matrix-vector multiplication:
     * [-InfectionRate      0                   0][ Susceptible ]   [dS/dt]
     * [InfectionRate       -RecoveryRate       0][ Infected    ] = [dI/dt]
     * [0                   RecoveryRate        0][ Recovered   ]   [dR/dt]
     */

    // hA: Rates and linear system of equations
    // Row 0: dS/dt
    hA[0][0] = -InfectionRate;
    
    // Row 1: dI/dt
    hA[1][0] = InfectionRate;
    hA[1][1] = -RecoveryRate;
    
    // Row 2: dR/dt
    hA[2][1] = RecoveryRate;

    // hx: Population values
    hx[0] = CurrentSusceptible;
    hx[1] = CurrentInfected;
    hx[2] = 0;    // No-one has recovered at the beginning of the simulation.

    // 3. Create an OpenCL Context
    Context = clCreateContext(NULL, 1, &Device, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateContext failed\n");
    
    // 4. Create an OpenCL command queue:
    CmdQueue = clCreateCommandQueue( Context, Device, 0, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateCommandQueue failed\n" );
    
    // 5. Allocate the GPU device memory buffers for the A matrix and the x and b vectors
    size_t aSize = MATW * MATW * sizeof(float);
    size_t xSize = MATW * sizeof(long);
    int mw = MATW;
    size_t mwSize = sizeof(mw);
    size_t bSize = MATW * sizeof(long);

    // Allocating device memory for the A matrix
    cl_mem dA = clCreateBuffer(Context, CL_MEM_READ_ONLY, aSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed for dA (1)\n");
    
    // Allocating device memory for the x vector
    cl_mem dx = clCreateBuffer(Context, CL_MEM_READ_ONLY, xSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed for dx (1)\n");
    
    // Allocating device memory for the A matrix width/x vector height
    cl_mem dMW = clCreateBuffer(Context, CL_MEM_READ_ONLY, mwSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed for dMW (1)\n");
    
    // Allocating device memory for the b vector
    cl_mem db = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, bSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed for dc (1)\n");
    

    // 6. Enqueue the 3 commands to write the data from the host buffers to the device buffers:

    // Enqueue the data from matrix A to the device
    status = clEnqueueWriteBuffer(CmdQueue, dA, CL_FALSE, 0, aSize, hA, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed for matrix A (1)\n");
    
    // Enqueue the data from vector x to the device
    status = clEnqueueWriteBuffer(CmdQueue, dx, CL_FALSE, 0, xSize, hx, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed for vector x (1)\n");

    // Enqueue the matrix width/vector height data dMW to the device
    status = clEnqueueWriteBuffer(CmdQueue, dMW, CL_FALSE, 0, mwSize, hMW, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed for matrix width/vector height mw (1)\n");
    
    Wait(CmdQueue);

    // This code is for the MatrixVectorMult and VectorAdd GPU parallelization functions.
    // 7. Read the kernel code from the MatrixVectorMult.cl and VectorAdd.cl files...

    // Read MatrixVectorMult.cl for MatrixVectorMult
    fseek(fp1, 0, SEEK_END);
    size_t fileSize = ftell(fp1);
    fseek(fp1, 0, SEEK_SET);
    char *clProgramTextMatVecMult = new char[ fileSize+1 ];     // Leave room for '\0'
    size_t n = fread( clProgramTextMatVecMult, 1, fileSize, fp1);
    clProgramTextMatVecMult[fileSize] = '\0';
    fclose(fp1);
    if (n != fileSize)
        fprintf(stderr, "Expected to read %d bytes from '%s' -- actually read %d.\n", fileSize, CL_FILE_NAME_1, n);

    // Read VectorAdd.cl for VectorAdd
    fseek(fp2, 0, SEEK_END);
    fileSize = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);
    char *clProgramTextVecAdd = new char[ fileSize+1 ];     // Leave room for '\0'
    n = fread( clProgramTextVecAdd, 1, fileSize, fp2);
    clProgramTextVecAdd[fileSize] = '\0';
    fclose(fp2);
    if (n != fileSize)
        fprintf(stderr, "Expected to read %d bytes from '%s' -- actually read %d.\n", fileSize, CL_FILE_NAME_2, n);

    // ... and create the kernel program:

    char *strings[2];
    strings[0] = clProgramTextMatVecMult;   // Add both th MatrixVectorMult and VectorAdd kernels to the list of string pointers
    strings[1] = clProgramTextVecAdd;       // for use with creating the program.
    Program = clCreateProgramWithSource(Context, 2, (const char **)strings, NULL, &status); //IMPORTANT: Multiple kernel .cl files
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateProgramWithSource failed\n");
    delete [ ] clProgramTextMatVecMult;
    delete [ ] clProgramTextVecAdd;


    // 8. Compile and link the kernel code:
    char *options = { (char*)"" };
    status = clBuildProgram(Program, 1, &Device, options, NULL, NULL);
    if (status != CL_SUCCESS) {
        size_t size;
        clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
        cl_char *log = new cl_char[size];
        clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, size, log, NULL);
        fprintf(stderr, "clBuildProgram failed:\n%s\n", log);
        delete [ ] log;
    }

    
    // 9. Create the kernel object (for MatrixVectorMult):

    // Create a Kernel for the MatrixMult function.
    Kernel = clCreateKernel(Program, "MatrixVectorMult", &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateKernel failed for MatrixVectorMult\n");
    
    // 10. Setup the arguments to the kernel object:

    // Passing the dA matrix arguments to the kernal object.
    status = clSetKernelArg(Kernel, 0, sizeof(cl_mem), &dA);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dA (%d)\n", status);
    

    // Passing the dx vector arguments to the kernal object.
    status = clSetKernelArg(Kernel, 1, sizeof(cl_mem), &dx);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dx (%d)\n", status);

    // Passing the dMW matrix width/vector height arguments to the kernal object.
    status = clSetKernelArg(Kernel, 2, sizeof(cl_mem), &dMW);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dMW (%d)\n", status);

    // Passing the db vector arguments to the kernal object.
    status = clSetKernelArg(Kernel, 3, sizeof(cl_mem), &db);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dA (%d)\n", status);

    // 11. Enqueue the kernel object for execution
    size_t globalWorkSize[3] = {MATW,  MATW,   1};
    size_t localWorkSize[3]  = {LOCALSIZE, LOCALSIZE, 1};

#ifndef CSV
	fprintf( stderr, "MatrixVectorMult\n");
	fprintf( stderr, "Number of Work Groups = %5d x %5d\n", MATW/LOCALSIZE, MATW/LOCALSIZE );
#endif

    Wait(CmdQueue);

    double time0 = omp_get_wtime();

    status = clEnqueueNDRangeKernel(CmdQueue, Kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueNDRangeKernel failed: %d\n", status);
    
    Wait(CmdQueue);
    double time1 = omp_get_wtime();

    // 12. Read the results buffer back from the device to the host:
    status = clEnqueueReadBuffer(CmdQueue, db, CL_FALSE, 0, bSize, hb, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueReadBuffer failed\n");
    
    Wait (CmdQueue);

#ifdef CSV
	fprintf( stderr, "%8d , %6d , %10.2lf, %12.2f\n",
		MATW*MATW, LOCALSIZE*LOCALSIZE, (double)MATW*(double)MATW*(double)MATW/(time1-time0)/1000000000., hb[MATW-1] );
#else
	fprintf( stderr, "Matrix Vector Multiplication Results");								// For MatrixMult, dC[MATW-1][MATW-1] = 2.0
	fprintf( stderr, "Matrix Size: %6d x %6d , Vector Size: %6d x 1, Work Elements: %4d x %4d , GigaMultsPerSecond: %10.2lf, db[%6d][0] = %12.2f\n",
		MATW, MATW, MATW, LOCALSIZE, LOCALSIZE, (double)MATW*(double)MATW*(double)MATW/(time1-time0)/1000000000., MATW-1, hb[MATW-1] );
#endif


    // 9. Create the kernel object (for VectorAdd):

    // Create a Kernel for the VectorAdd function.
    Kernel = clCreateKernel(Program, "VectorAdd", &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateKernel failed for VectorAdd\n");
    
    // 10. Setup the arguments to the kernel object:

    // Passing the dx vector arguments to the kernal object.
    status = clSetKernelArg(Kernel, 0, sizeof(cl_mem), &du);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dx (%d)\n", status);
    

    // Passing the db vector arguments to the kernal object.
    status = clSetKernelArg(Kernel, 1, sizeof(cl_mem), &dv);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for db (%d)\n", status);

    // Passing the dVR vector height arguments to the kernal object.
    status = clSetKernelArg(Kernel, 2, sizeof(cl_mem), &dVR);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dMW (%d)\n", status);

    // Passing the dx vector (return) arguments to the kernal object.
    status = clSetKernelArg(Kernel, 3, sizeof(cl_mem), &dw);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed for dx (%d)\n", status);

    // 11. Enqueue the kernel object for execution
    size_t globalWorkSize[3] = {MATW,  MATW,   1};
    size_t localWorkSize[3]  = {LOCALSIZE, LOCALSIZE, 1};

#ifndef CSV
	fprintf( stderr, "VectorAdd\n");
	fprintf( stderr, "Number of Work Groups = %5d x %5d\n", MATW/LOCALSIZE, MATW/LOCALSIZE );
#endif

    Wait(CmdQueue);

    double time0 = omp_get_wtime();

    status = clEnqueueNDRangeKernel(CmdQueue, Kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueNDRangeKernel failed: %d\n", status);
    
    Wait(CmdQueue);
    double time1 = omp_get_wtime();

    // 12. Read the results buffer back from the device to the host:
    status = clEnqueueReadBuffer(CmdQueue, dw, CL_FALSE, 0, bSize, hx, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueReadBuffer failed\n");
    
    Wait (CmdQueue);

#ifdef CSV
	fprintf( stderr, "%8d , %6d , %10.2lf, %12.2f\n",
		MATW*MATW, LOCALSIZE*LOCALSIZE, (double)MATW*(double)MATW*(double)MATW/(time1-time0)/1000000000., hb[MATW-1] );
#else
	fprintf( stderr, "Vector Addition Results\n");
	fprintf( stderr, "Vector Size: %6d x 1, Work Elements: %4d x %4d , GigaMultsPerSecond: %10.2lf, dw[%6d][0] = %12.2f\n",
		MATW, LOCALSIZE, LOCALSIZE, (double)MATW*(double)MATW*(double)MATW/(time1-time0)/1000000000., MATW-1, hx[MATW-1] );
#endif





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
