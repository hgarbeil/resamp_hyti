#include "Utils.h"

#include <iostream>

using namespace std ;


int main (int argc, char *argv[]) {
	int i ;
	float nval, R0, R1, R2, Tval = 20 ;
	float waves [25] ;
	float *allcoefs= new float[4*25], *coefs ;
	

	FILE *fcoefs = fopen ("/hbeta/harold/workdir/resamp_hyti/PerfModel/outcoefs","r") ;
	fread (allcoefs, 4, 100, fcoefs) ;
	fclose (fcoefs) ;

	for (i=0; i<25; i++) {
		waves[24-i] = 10000. / (930.+i*13) ;
	}

	for (i=0; i<25; i++) {
		
		coefs = &allcoefs[ i * 4] ; 
		nval = coefs[0] + coefs[1] * Tval + coefs[2] * Tval * Tval + coefs[3] * Tval * Tval * Tval ;
		R0 = bb2rad(Tval, waves[i]) ;
		R1 = bb2rad(Tval - nval, waves[i]) ;
		R2 = bb2rad(Tval + nval, waves[i]) ;
		cout << i << " " << waves[i] << " " << nval<< " " << R0 << " " << R1 << " "<< R2 << endl ;
	}
}


	
