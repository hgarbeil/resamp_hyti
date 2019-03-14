#ifndef UTILS_1
#define UTILS_1
#include <stdio.h>
#include <math.h>

void bb_rad_to_temp_arr (float *, float *, int, float) ;
void bb_temp_to_rad_arr (float *, float *, int, float) ;

float bb2temp (float rad, float wave) ;
float bb2rad (float temp, float wave) ;


void get_BIP_data (float *outdat, char *dfile, int ns, int nl, int startband, int nbsub, int nb) ;	


#endif
