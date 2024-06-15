#define IN
#define OUT

// A OpenCL function designed to multiply a maxtrix by a vector: Ax = b
// A: A matrix with m rows and n columns.
// x: A vector with n rows and 1 column.
// b: The resultant vector with m rows and 1 column.

kernel void MatrixVectorMult(IN global const float *dA, IN global const long *dx, IN global int *dMW, OUT global long *db) {
    // [dA] is a matrix of dimensions m rows by dMW columns
    // [dx] is a vector of dimensions dMW rows by 1 column
    // [db] is a vector of dimensions m rows by 1 column
    
    int mw = *dMW;
    int brow = get_global_id(0);
    int bcol = get_global_id(1);

    int aindex = brow * mw;         // a[i][0]
    int xindex = bcol;              // x[0][j]
    int bindex = brow * mw + bcol;  // b[i][j]

    long bij = 0;

    for (int k = 0; k < mw; k++) {
        bij += round(dA[aindex] * dx[xindex]);
        aindex++;
        xindex += mw;
    }
    
    db[bindex] = bij;
}