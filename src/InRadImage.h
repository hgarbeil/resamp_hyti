/*
 * InRadImage.h
 *
 *  Created on: Oct 31, 2008
 *      Author: harold
 */

#ifndef INRADIMAGE_H_
#define INRADIMAGE_H_

class InRadImage {
public:
	InRadImage();
	void 	setFileNames (char *infile, char *infile_hdr) ;
	void 	GetXY (float lon, float lat, float *xy) ;
	float  	getClosestPixel (float, float , float *, float *) ;
	float	getRad (int, int) ;
	virtual ~InRadImage();
	char filename [420], hdr_filename [420] ;
	int  ns, nl, dtype ;
	float ll_space ;
	float start_lat, start_lon ;
	float 	*radData ;
	void  	ReadHeader () ;
	int 	ReadData () ;
};

#endif /* INRADIMAGE_H_ */
