#define IN
#define OUT

// A OpenCL function designed to multiply a maxtrix by a vector: Ax = b
// A: A matrix with m rows and n columns.
// x: A vector with n rows and 1 column.
// b: The resultant vector with m rows and 1 column.

kernel void MatrixVectorMult(IN global const long *dA, IN global const long *dx, IN global int *dMW, OUT global long *db) {
    int mw = *dMW;
    int brow = get_global_id(0);
    int bcol = get_global_id(1);

    int aindex = brow * mw;         // a[i][0]
    int xindex = bcol;              // x[0][j]
    int bindex = brow * mw + bcol;  // b[i][j]

    long bij = 0.0;

    for (int k = 0; k < mw; k++) {
        bij += dA[aindex] * dx[xindex];
        aindex++;
        xindex += mw;
    }
    
    db[bindex] = bij;
}