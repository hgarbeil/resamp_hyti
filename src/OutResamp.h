/*
 * OutResamp.h
 *
 *  Created on: Nov 3, 2008
 *      Author: harold
 */

#ifndef OUTRESAMP_H_
#define OUTRESAMP_H_
#include "InRadImage.h"

class OutResamp {
public:
	OutResamp();
	virtual ~OutResamp();
	void init (float lon0, float lat0, float gridspace, int ns, int nl) ;
	int getWidth () ;
	int getHeight () ;
	void getGeometry (float *) ;
	void getGeometry (float *, float *, float *) ;
	void getRadiance () ;
	double calcTMFrac (float x) ;
	void setSensor (int s) ;
	void writeOutputRadiance () ;
	void setInRadImage (InRadImage *iri) ;
	void testFracs () ;
	int 	getLatLon (int, int, float*, float *) ;
	double  calcXFrac (float ) ;
	double  calcYFrac (float) ;
	char	outfile [420] ;
 	int nl, ns, sensor ;
	float start_lat, start_lon, gridspace ;
	float *outRad ;
	InRadImage *inRad ;
};

#endif /* OUTRESAMP_H_ */
