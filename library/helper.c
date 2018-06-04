/*=================================================================
 * --- helper.c - version 1.0 ---
 *
 * This helper library contains three functions: DeserializeVar and
 * SerializeVar are used to flatten MATLAB variables, even structs.
 * SetTimeStamp is a function that is used for the timing options
 * used in sendvar and receivar. This function will only be compiled
 * when explicitely specified (-DTIMINGS).
 *
 * Copyright 2013 Jeroen Engelberts, SURFsara
 *	
 *=================================================================*/

#include "helper.h"

mxArray*
DeserializeVar(const char *varin, unsigned long buflen)        
{
    char filename[2048];
    char* tmpdir;
    mxArray *varout;
    MATFile* FPm;
    FILE *FPr;

/* Check where a small file can be saved:
 * 1. /dev/shm, if not try:
 * 2. ${TMPDIR}, if set but not possible -> error; if not set, try:
 * 3. /tmp, if not possible -> error */
    strcpy(filename,"/dev/shm/mmpi.XXXXXX");
    if ((mkstemp(filename)==-1) || ((FPr=fopen(filename,"w"))==NULL)) {
        tmpdir=getenv("TMPDIR");
        if (tmpdir!=NULL) {
            strcpy(filename,tmpdir);
            strcat(filename,"mmpi.XXXXXX");
            if ((mkstemp(filename)==-1) || ((FPr=fopen(filename,"w"))==NULL)) {
                mexErrMsgTxt("Cannot create temporary file!");
            }
        } else {
            strcpy(filename,"/tmp/mmpi.XXXXXX");
            if ((mkstemp(filename)==-1) || ((FPr=fopen(filename,"w"))==NULL)) {
                mexErrMsgTxt("Cannot create temporary file!");
            }
        }
    }
/* Write flat data to temporary file and close it. */ 
    fwrite(varin, buflen, 1, FPr);
    fclose(FPr);

/* Read temporary file as MATLAB.mat file */
    FPm = matOpen(filename,"r");
	if (FPm==NULL){
        mexErrMsgTxt("Cannot open temporary file!");
    }   
    varout=matGetVariable(FPm,"temp");
    if (varout==NULL){
        mexErrMsgTxt("Cannot write to temporary file!");
    }
    if (matClose(FPm)!=0){
        mexErrMsgTxt("Cannot close temporary file!");
    }
    remove(filename);
    return varout;
}

char*
SerializeVar(const mxArray *varin, unsigned long *buflen)        
{
    char *buf, *tmpdir;
    char filename[2048];
    MATFile* FPm;
    FILE *FPr;

/* Check where a small file can be saved:
 * 1. /dev/shm, if not try:
 * 2. ${TMPDIR}, if set but not possible -> error; if not set, try:
 * 3. /tmp, if not possible -> error */
    strcpy(filename,"/dev/shm/mmpi.XXXXXX");
    if ((mkstemp(filename)==-1) || ((FPm=matOpen(filename,"w"))==NULL)) {
        tmpdir=getenv("TMPDIR");
        if (tmpdir!=NULL) {
            strcpy(filename,tmpdir);
            strcat(filename,"mmpi.XXXXXX");
            if ((mkstemp(filename)==-1) || ((FPm=matOpen(filename,"w"))==NULL)) {
                mexErrMsgTxt("Cannot create temporary file!");
            }
        } else {
            strcpy(filename,"/tmp/mmpi.XXXXXX");
            if ((mkstemp(filename)==-1) || ((FPm=matOpen(filename,"w"))==NULL)) {
                mexErrMsgTxt("Cannot create temporary file!");
            }
        }
    }
/* Write input variable to MATLAB.mat temporary file */
    if (matPutVariable(FPm,"temp",varin)!=0){
        mexErrMsgTxt("Cannot write to temporary file!");
    }
    if (matClose(FPm)!=0){
        mexErrMsgTxt("Cannot close temporary file!");
    }
    
/* Read temporary file as flat file */
    FPr = fopen(filename,"rb");
	if (FPr==NULL){
        mexErrMsgTxt("Cannot open raw temporary file!");
    }
    fseek(FPr, 0, SEEK_END);
	*buflen=ftell(FPr);
	fseek(FPr, 0, SEEK_SET);
    buf=(char*)malloc((*buflen)+1);
	if (buf==NULL) {
        mexErrMsgTxt("Cannot allocate buffer memory!");
    }
    fread(buf, *buflen, 1, FPr);
    fclose(FPr);
    remove(filename);
    return buf;
}

#ifdef TIMINGS
/* Special function for the eSiBayes project - for analysis of timings */
int
SetTimeStamp(uint8_T code)
{
    mxArray *timing, *fcounter, *fstarttime, *fnumber1[1], *fctime[1], *intimer, *outtimer;
    mxArray *incodes, *outcodes;
    uint32_T *counter;
    double *ctime, timer, *pintimer, *pouttimer;
    uint64_T *starttime, *number1;
    int nelem, fieldnumber;
    uint8_T *pincodes, *poutcodes;
            
    /* retrieve timing variable from base workspace */
    timing=mexGetVariable("base","timing");
    if (timing==NULL) {
        mexWarnMsgTxt("No timing results present.");
        return 23;
    }
    /* Increase counter */
    fcounter=mxGetField(timing,0,"counter");
    counter=(uint32_T*) mxGetPr(fcounter);
    (*counter)=(*counter)+1;
    /* Obtain time relative to starttime (as double in secs) */
    fstarttime=mxGetField(timing,0,"starttime");
    starttime=(uint64_T*) mxGetPr(fstarttime);
    fnumber1[0]=mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
    number1=(uint64_T*) mxGetPr(fnumber1[0]);
    *number1=1;
    if(mexCallMATLAB(1,fctime,1,fnumber1,"toc")!=0) {
        mexErrMsgTxt("cannot call toc from mex.");
    }
    ctime=mxGetPr(fctime[0]);
    /* Timing only works on Linux - 1000000 
     * On Mac OSX, this should be - 1000000000 */
    timer=*ctime-(double)*starttime/1000000;
    /* Get timer field from timings array */
    intimer=mxGetField(timing,0,"timer");
    /* Get pointer to current timer values */
    pintimer=mxGetPr(intimer);
    /* Obtain number of timer values */
    nelem=mxGetNumberOfElements(intimer);
    /* Create a new array for timer value (nelem+1) */
    outtimer=mxCreateDoubleMatrix(1,nelem+1,mxREAL);
    /* Get pointer to new timer value array */
    pouttimer=mxGetPr(outtimer);
    /* Copy old values to new array */
    memcpy(pouttimer,pintimer,nelem*sizeof(double));
    /* Add the current timer */
    pouttimer[nelem]=timer;
    /* Remove old timer array */
    fieldnumber=mxGetFieldNumber(timing,"timer");
    mxRemoveField(timing,fieldnumber);
    /* Add new timer array */
    mxAddField(timing,"timer");
    fieldnumber=mxGetFieldNumber(timing,"timer");
    mxSetFieldByNumber(timing,0,fieldnumber,outtimer);
    /* Get code field from timings array */
    incodes=mxGetField(timing,0,"code");
    /* Get pointer to current code values */
    pincodes=(uint8_T*)mxGetPr(incodes);
    /* Create a new array for code value (nelem+1) */
    outcodes=mxCreateNumericMatrix(1,nelem+1,mxUINT8_CLASS,mxREAL);
    /* Get pointer to new code value array */
    poutcodes=(uint8_T*)mxGetPr(outcodes);
    /* Copy old values to new array */
    memcpy(poutcodes,pincodes,nelem*sizeof(uint8_T));
    /* Add the current code */
    poutcodes[nelem]=code;
    /* Remove old code array */
    fieldnumber=mxGetFieldNumber(timing,"code");
    mxRemoveField(timing,fieldnumber);
    /* Add new code array */
    mxAddField(timing,"code");
    fieldnumber=mxGetFieldNumber(timing,"code");
    mxSetFieldByNumber(timing,0,fieldnumber,outcodes);
    /* Write back timing structure */
    if (mexPutVariable("base","timing",timing)!=0) {
        mexErrMsgTxt("Timing can not be written back.");
    }
    mxDestroyArray(fnumber1[0]);
    mxDestroyArray(intimer);
    mxDestroyArray(incodes);
    mxDestroyArray(timing);
    return 0;
}
#endif
