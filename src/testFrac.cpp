#include <stdio.h>
#include <math.h>

int main (int argc, char *argv[])  
{


        	double ydist, bterm, denom, x,val, acoef, bcoef, ccoef ;
		for (x=0.; x<1200.; x+= 10.){
		ydist = x ;
		ydist /= 1000. ;
        
        	acoef = 1.0111061 ;
        	bcoef = 469.75024 ;
        	ccoef = 9.3393436 ;
        	bterm = bcoef * pow (ydist, ccoef) ;

        	val =  (1./(acoef+bterm)) ;
		printf ("%f\t%f\r\n", x, val) ;
		}
}
