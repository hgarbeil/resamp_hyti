#include <math.h>
#include <stdio.h>
#include "Utils.h"

void bb_rad_to_temp_arr (float *rad, float *arr, int npix, float wave) 
{
	int i ;
	float val; 
	for (i=0; i<npix; i++) {
		arr[i] = bb2temp(rad[i], wave) ;
	}

	

}

void bb_temp_to_rad_arr (float *temparr, float *rad, int npix, float wave) {
	int i ;
	
	for (i=0; i<npix; i++) {
		rad[i]= bb2rad (temparr[i], wave) ;
	}

}





float bb2temp (float rad, float wave){
    double k, k2, k1, emiss=1., val ;
    double h, c ;

    k = 1.38066e-23 ;
	c = 2.99793E8 ;
	h = 6.626068E-34 ;
	//k1 = 119.104E7 ;
	k1 = 119.1066E7 ;

	// convert microns to meters
	wave = wave / 1000000. ;
	
	// convert rad to m-3 rather than m-2 microns
	rad = rad * 1E6 ;

	k2 = h * c / (k * wave) ;
	k1 = 2. * h * c * c / pow(wave,5.) ;
	val = log ((emiss * k1)/(rad)+1.) ;
	val = k2 / val - 273.15 ;

	return (float(val)) ;
}



float bb2rad (float temp, float wave){
    double val, tval ;

    tval = temp + 273.15 ;
    val = 1.191066E8/ pow(wave,5) / (exp(1.4387752E4/(wave*tval))-1.) ;
	return (float) val ;

}


void get_BIP_data (float *outdat, char *dfile, int ns, int nl, int startband, int nbsub, int nb) {

	int i, j, ib ;
	float *fullline = new float [ns * nb] ;

	FILE *fin = fopen (dfile, "rb") ;
	for (i=0; i<nl; i++) {
		fread (fullline, 4, ns * nb, fin) ;
		for (ib=0; ib<nbsub; ib++) {
			for (j=0; j<ns; j++) 
				outdat[i * nbsub * ns + j * nbsub +  ib] = fullline[j*nb+startband+ib] ;
		}
	}
	fclose (fin) ;
	delete [] fullline ;
}
	



