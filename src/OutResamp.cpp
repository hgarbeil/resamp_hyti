/*
 * OutResamp.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: harold
 */

#include "OutResamp.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#define  Deg2Meters 110000.
using namespace std ;


OutResamp::OutResamp() {
	// TODO Auto-generated constructor stub
	inRad = 0L ;
	outRad = 0L ;
	sensor = 0 ; // default is modis

}

OutResamp::~OutResamp() {
	// TODO Auto-generated destructor stub
	if (outRad) delete [] outRad ;
}

void OutResamp::init (float lon, float lat, float space, int s, int l) {
	ns = s ;
	nl = l ;
	start_lat = lat ;
	start_lon = lon ;
	gridspace = space ;
	outRad = new float [ns * nl] ;
	for (int i=0; i<ns *nl; i++) outRad[i]=-1.f ;

}

void OutResamp::setSensor (int s) {
	sensor = s ;
}


int OutResamp::getWidth() {
	return (ns) ;
}

int OutResamp::getHeight() {
	return (nl) ;
}

void OutResamp::setInRadImage (InRadImage *iri) {
	this->inRad = iri ;
}

void OutResamp::getGeometry (float *ll){
	ll[0] = start_lat ;
	ll[1] = start_lon ;
	ll[2] = gridspace ;
}

void OutResamp::getGeometry (float *lon, float *lat, float *space){
	*lon = start_lon ;
	*lat = start_lat ;
	*space = gridspace ;
}

void OutResamp::testFracs () {

	float distval, xfrac, yfrac ;
	FILE *fout = fopen ("/home/harold/testfracs.txt", "w") ;
	for (int i=-60; i< 60; i++) {
		distval = (i * 3) ;
		xfrac = calcXFrac (fabs(distval)) ;
		yfrac = calcYFrac (fabs(distval)) ;
		fprintf (fout, "%f %f %f\r\n", distval, xfrac, yfrac) ;
	}
	fclose (fout) ;

}

void OutResamp::getRadiance () {

	int i, j, is, js, i_in, j_in, i_out, j_out, numx_in, numy_in ;
	float  	srchdist, inlat, inlon, xdist, xfrac, ydist, yfrac ;
	float   limitval, limit=1200. ;
	double totalRad, totalweight, radval ;

	float lat, lon, latstart, lonstart, newlat, newlon ;
	lat=0.f ;
	lon=0.f ;
	// search +/- 2km

	srchdist = 4. * gridspace ;



		numx_in =(int)( srchdist / (inRad->ll_space)) ;
		numy_in = (int) (srchdist / (inRad->ll_space)) ;


	for (is=0; is<nl; is++) {
	if (!(is%10)) cout << "Working on line # : " << is << endl ;

	for (js=0; js<ns; js++) {

	getLatLon (js, is, &lon, &lat) ;
	getLatLon (js-2, is-2, &lonstart, &latstart) ;
	totalRad = 0.f ;


	// for MODIS - the x distance and y distance of influence is
	// within 2 km
	totalweight = 0. ;

	if (sensor ==0) limit = 1200. ;
	if (sensor ==1) limit = 45. ;
	if (sensor ==2) limit = 90. ;
	for (i=0; i<numy_in; i++) {
		inlat = latstart - i * inRad->ll_space ;
		// find closest pixel in inRad to this and then get
		// the lat and lon of that pixel

		//ydist = fabs(lat - inlat) * Deg2Meters ;
		//yfrac = calcYFrac (yfrac) ;
		for (j=0; j<numx_in; j++) {
			inlon = lonstart + j * inRad->ll_space ;
			
			radval = inRad->getClosestPixel (inlon, inlat, &newlon, &newlat) ;
			if (radval < 0) continue ;
			xdist = fabs(lon - newlon) * Deg2Meters  ;
			ydist = fabs(lat - newlat) * Deg2Meters ;
			if (xdist > limit || ydist > limit) continue ;
			
			xfrac = calcXFrac (xdist) ;
			yfrac = calcYFrac (ydist) ;
			totalweight += (xfrac * yfrac) ;
			totalRad += (xfrac * yfrac * radval) ;
		}
	}
	totalRad /= totalweight ;
	outRad [is*ns+js] = totalRad ;
	}}


}


void OutResamp::writeOutputRadiance () {
	FILE *fout = fopen (outfile, "w") ;
	fwrite (outRad, 4, ns * nl, fout) ;
	fclose (fout) ;
}


double OutResamp::calcYFrac (float ydist) {
	

        float acoef, bcoef, ccoef ;
        double bterm, denom ;
		if (sensor==0) {
			ydist /= 1000. ;
        	if (ydist>1.2) return (0.) ;
        
        	acoef = 1.0111061 ;
        	bcoef = 469.75024 ;
        	ccoef = 9.3393436 ;
        	bterm = bcoef * pow (ydist, ccoef) ;

        	return (1./(acoef+bterm)) ;
		}
		else return (calcTMFrac(ydist)) ;

}



double OutResamp::calcXFrac (float xdist) {
	
	if (sensor==0){
		xdist /= 1000. ;
		if (xdist < .1) return (1.0) ;
        if (xdist > 1.1) return (0.0) ;
        
        return (- xdist + 1.1) ;
	}
	else return (calcTMFrac (xdist)) ;
}

double OutResamp::calcTMFrac (float xval) {

	float xdist ;
	xdist= xval ;
	if (sensor == 2) xdist = xval / 2. ;
	double fval ;
	double a =  0.49089062 ;
	double b =  0.52159306 ;
	double c =  0.093935698 ;
	double d = 0.074249528 ;
	if (xdist > 35.) return 0. ;


	fval = a + b * cos (c * xdist + d) ;
	if (fval <0.) fval = 0. ;


	return (fval) ;
}



int OutResamp::getLatLon (int x, int y, float *lon, float *lat) {

	*lat = start_lat - y * gridspace ;
	*lon = start_lon + x * gridspace ;
	return (0) ;
}
