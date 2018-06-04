/*=================================================================
 * 
 * --- bcastvar 1.0 - completely in C ---
 *
 * This function is used to broadcast a MATLAB variable amongst
 * *all*  workers. 
 *
 * It takes a MATLAB variable as input on the worker that has the
 * original. All other workers will receive the variable as output.
 *
 * Example:
 * 
 * if (mpirank==0) then
 *   a=rand(5,5);
 *   bcastvar(0,a);
 * else
 *   a=bcastvar;
 * end
 *
 * Copyright 2013 Jeroen Engelberts, SURFsara
 *	
 *=================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include "mex.h"
#include "mat.h"
#include "helper.h"

/* mexFunction is the gateway routine for the MEX-file. */ 
void
mexFunction( int nlhs, mxArray *plhs[],
             int nrhs, const mxArray *prhs[])
{
    int myself,root,usempi;
    char *mpibuffer;
    unsigned long mpibuffersize;
    double *dum;
    mxArray *myselfstruct;
    
/* Check the number of input parameters. Should be 1 (for receivers) or 2 (for the broadcaster). */
    if (nrhs>2) {
        mexErrMsgTxt("Please pass the root of the broadcast (and only mandatory for root the variable).");
    }

/* Obtain the mpirank of the worker (myself) */
    myselfstruct=mexGetVariable("base","mpirank");
    if(myselfstruct==NULL) {
        mexErrMsgTxt("mpirank is not defined in base workspace.");
    }
    dum=mxGetPr(myselfstruct);
    myself=(int)dum[0];
    
/* Obtain the mpirank of the broadcaster */
    dum=mxGetPr(prhs[0]);
    root=(int)dum[0];

/* Check whether MPI is initialized. If not -> error */
    MPI_Initialized(&usempi);
    if (!usempi) {
        mexErrMsgTxt("MPI is not initialized.");
    }

/* Check whether the worker is the broadcaster */    
    if (myself==root) {
        /* Yes, the worker is the broadcaster (root) */
        if (nrhs<2) {
            mexErrMsgTxt("For root, a second parameter is required."); 
        }
        /* Serialize the variable into mpibuffer (and calculate mpibuffersize) */
        mpibuffer=SerializeVar(prhs[1],&mpibuffersize);
        /* Broadcast the mpibuffersize to all workers */
        MPI_Bcast(&mpibuffersize,1,MPI_UNSIGNED_LONG,root,MPI_COMM_WORLD);
        /* Broadcast the mpibuffer to all workers */
        MPI_Bcast(mpibuffer,mpibuffersize,MPI_BYTE,root,MPI_COMM_WORLD);
    } else {
        /* No, this worker will receive serialized data (is not root) */
        /* Receive the mpibuffersize from root */
        MPI_Bcast(&mpibuffersize,1,MPI_UNSIGNED_LONG,root,MPI_COMM_WORLD);
        /* Allocate enough memory to receive the serialized data */
        mpibuffer=malloc(mpibuffersize);
        /* Receive the mpibuffer from root */
        MPI_Bcast(mpibuffer,mpibuffersize,MPI_BYTE,root,MPI_COMM_WORLD);
    }
    /* Deserialize the mpibuffer into the first left hand side variable */ 
    plhs[0]=DeserializeVar(mpibuffer,mpibuffersize);
    /* Free all buffers that are no longer required */
    free(mpibuffer);
    mxDestroyArray(myselfstruct);
}
