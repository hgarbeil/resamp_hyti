/*
 * InRadImage.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: harold
 */

#include "InRadImage.h"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std ;

InRadImage::InRadImage() {
	// TODO Auto-generated constructor stub
	radData = NULL ;

}

InRadImage::~InRadImage() {
	// TODO Auto-generated destructor stub
	if (radData) delete []radData ;
}

void InRadImage::setFileNames (char *infile, char *infile_hdr) {
	strcpy (filename, infile) ;
	strcpy (hdr_filename, infile_hdr) ;
}


/*
int InRadImage::WriteHdrFile(char *filename, int ns, int nl, int nb, int dt, int bform)
{

	// the filename is not appended with the .hdr suffix.
	// therefore, the program should check to make sure it has been passed with a .hdr
	// suffix



	char buf [240], *token ;
	int  len, len1 ;
/*
	token = strstr (filename, ".hdr") ;
	len = strlen (filename) ;
	len1 = strlen (token) ;
	if (len1 <4) return (-1) ;
	ofstream ofil (filename, ios::out) ;
	if (ofil.bad() )
		return (-1) ;
	sprintf (buf, "samples = %5d", ns) ;
	ofil << buf << endl ;

	sprintf (buf, "lines = %d", nl) ;
	ofil << buf << endl ;

	sprintf (buf, "bands = %d", nb) ;
	ofil << buf << endl ;

	dt = me2envi_dt [dt] ;
	sprintf (buf, "data type = %1d", dt) ;
	ofil << buf << endl ;

	switch (bform) {
	case '0' :
			strcpy (buf, "interleave = bsq") ;
			ofil << buf << endl ;
			break ;
	case '1' :
			strcpy (buf, "interleave = bil") ;
			ofil << buf << endl ;
			break ;
	case '2' :
			strcpy (buf, "interleave = bip") ;
			ofil << buf << endl ;
			break ;
	}


	return (0) ;
}
*/

void  InRadImage::ReadHeader()
{

	int  dt ;
	char linedat [440], *token, *newtoken ;

	ifstream ifil (hdr_filename, ios::in) ;



	while (!ifil.eof()){

		ifil.getline (linedat, 440, '\n') ;
		if (strlen (linedat) < 2 && ifil.eof()) break ;
		token = strtok (linedat, "=") ;
			// find the issues of concern

		if (!strncmp (token, "samp", 4)) {
			token = strtok (NULL, "=") ;
			ns = atoi (token) ;
		}

		if (!strncmp (token, "line", 4)) {
			token = strtok (NULL, "=") ;
			nl = atoi (token) ;
		}
// map info token
		if (!strncmp (token, "map", 3)) {
			token = strtok (NULL, "=,") ;
			token = strtok (NULL, "=,") ;
			token = strtok (NULL, "=,") ;
			token = strtok (NULL, "=,") ;
			// needs to be in geographic , assume this is so
			start_lon = atof (token) ;
			token = strtok (NULL, ",") ;
			start_lat = atof (token) ;
			token = strtok (NULL, ",") ;
			ll_space = atof (token) ;


		}


		//TRACE ("%s\n", linedat) ;
	}
	ifil.close() ;



	return  ;
}


int InRadImage::ReadData (){
	radData = new float [ns * nl] ;
  	FILE *fin = fopen(filename, "r") ;
	if (fin == NULL) return (-1) ;
 	fread ((char *)&radData[0], 4, ns * nl, fin) ;
	fclose (fin) ;
	return (1) ;

}


void InRadImage::GetXY (float lon, float lat, float *xy){

	float x = (lon - start_lon) / ll_space ;
	float y = (start_lat - lat) / ll_space ;
	*(xy) 	= x ;
	*(xy+1)	= y ;
	return ;

}

float InRadImage::getRad (int x, int y){
	if (radData) {
		return (radData[y*ns+x]) ;
	}
	else return (-1.) ;
}

float InRadImage::getClosestPixel (float lon, float lat, float *olon, float *olat) {
	int 	ix, iy ;
	float x, y ;

	x = (lon - start_lon) / ll_space ;
	y = (start_lat - lat) / ll_space ;

	// round to nearest int
	ix = (int) (x+0.5) ;
	iy = (int) (y+0.5) ;

	if (ix < 0 || iy < 0 || ix >= ns || iy >= nl) {
		return (-1.) ;
	}

	*olon = start_lon + ix * ll_space ;
	*olat = start_lat - iy * ll_space ;

	return (getRad (ix, iy))  ;

}

