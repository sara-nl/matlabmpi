#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mex.h"
#include "mat.h"

mxArray* DeserializeVar(const char*, unsigned long);   
char* SerializeVar(const mxArray*, unsigned long*);
#ifdef TIMINGS
int SetTimeStamp(uint8_T);
#endif