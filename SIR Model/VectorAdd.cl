#define IN
#define OUT

// An OpenCL function designed to quickly add two vectors together: u + v = w
kernel void VectorAdd(IN global const long *du, IN global const long *dv, IN global int *dVR, OUT global long *dw) {
    // [du] is a vector of dimensions dVR by 1 column
    // [dv] is a vector of dimensions dVR by 1 column
    // [dw] is a vector of dimensions dVR by 1 column

    int vr = *dVR;
    int wrow = get_global_id(0);
    //int wcol = get_global_id(1);

    int index = wrow * vr;

    dw[index] = du[index] + dv[index];
}