# matlabmpi

This is matlabmpi! It combines the power of MATLAB (within one computer), with the power of MPI (within, or even beyond one computer). Currently, (only) three MPI functions have been implemented:

* **MPI_Send** in C / **sendvar** in MATLAB
* **MPI_Recv** in C / **receivevar** in MATLAB
* **MPI_Bcast** in C / **bcastvar** in MATLAB

##Requirements
To run and build matlabmpi, you will need:

* A C compiler (GCC will do)
* An MPI library (OpenMPI will do)
* MATLAB
* The MATLAB Compiler (for building)
* The MATLAB Compiler Runtime Environment

##Build

**NB** Make sure that *mcc*, *mex*, *gcc* and *mpirun* are in your *PATH*, and that the MPI header and library can be found by the compiler/linker.

Inside the main directory, type:

`make`

To run, please use the functionality of *mpirun*. For example, on eight cores:

`mpirun -n 8 ./matlabprog`

##Example
The example included uses all three implemented MPI functions. What it does:

1. On the master process, it creates a vector [1,2,3,4,5,6,7,8,9]
2. It then broadcasts this vector to all slave processes
3. It then divides the work, *i.e.* it sends (unique) multiplication factor to all slaves
4. Each slave process multiple the vector with its (unique) multiplication factor
5. Each slave sends back his result to the master
6. The master assembles all vectors into a matrix and prints that on screen

Modifyable in this example are:

* matlabmain.m
* f1.m

*matlabmain.m* contains the main structure of the MPI program. Modify it to your needs. *f1.m* is an arbitrary function that you wish to run on all processes. It should, of course, be replaced by a (very) time consuming compute function that does part of the work for the master.

