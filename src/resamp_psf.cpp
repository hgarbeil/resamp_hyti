//============================================================================
// Name        : resamp_psf.cpp
// Author      : Harold Garbeil
// Version     :
// Copyright   : HIGP/SOEST/UHM  	12 / 1/ 08
// Description : resample radiance image to new grid using input
// psf...
//============================================================================

#include <iostream>
using namespace std;

//#include "flowmask.h"
//#include "outfilegeom.h"
#include "InRadImage.h"
#include "OutResamp.h"
#include "FileOperations.h"


int main(int argc, char *argv[]) {

	char infile [420], outfile [420], header [424], mapinfo [420] ;
	int i, j, is, js, sensor ;
	int ns_out, nl_out ;

	float xdist, ydist, start_lat_out, start_lon_out, out_lat, out_lon, out_space ;
	float end_lat, end_lon ;
	float in_lat, in_lon ;

	double totweight ;


	
	if (argc < 4) {
		cout << "Usage : resamp_psf inRadiance outresampfile outspacing sensor (0-modis, 1-TM, 2 - 60m HySperies?)" << endl ;
		exit (-1) ;
	}
	strcpy (infile, *++argv) ;
	strcpy (outfile, *++argv) ;
	out_space = atof (*++argv) ;
	sensor = atoi (*++argv) ;

	strcpy (header, infile) ;
	strcat (header, ".hdr") ;

	// Input Radiance Image Class
	InRadImage *inrad_class = new InRadImage () ;
	inrad_class->setFileNames (infile, header) ;
	inrad_class->ReadHeader () ;
	CFileOperations *cfo = new CFileOperations () ;
	cfo->GetMapInfo (header, mapinfo) ;
	// open image file - that is the radiance file in non-Modis spatial resolution
	//float *raddata_in = new float [inrad_class->ns * inrad_class->nl] ;
	inrad_class->ReadData () ;
	// Output Class
	OutResamp *ores = new OutResamp () ;
	start_lat_out = inrad_class->start_lat  ;
	start_lon_out = inrad_class->start_lon  ;
	end_lat = inrad_class->start_lat - inrad_class->ll_space * inrad_class->nl ;
	end_lon = inrad_class->start_lon + inrad_class->ll_space * inrad_class->ns ;
	//end_lat -= 2000. ;
	//end_lon += 2000. ;

	ns_out = (int) ((end_lon - start_lon_out) / out_space) + 1 ;
	nl_out = (int) ((start_lat_out - end_lat) / out_space) + 1 ;




	ores->init (start_lon_out, start_lat_out, out_space, ns_out, nl_out) ;
	strcpy (ores->outfile, outfile) ;
	ores->setSensor (sensor) ;
	//ns_out = ores->getWidth () ;
	//nl_out = ores->getHeight ();
	//ores->getGeometry (&start_lon_out, &start_lat_out, &out_space) ;
	ores->setInRadImage (inrad_class) ;
	// go through the output file grid, looking at the flowmaskfile, calculate
	// the weights for all hotspots within the mindist for x and y, total these
	// weights, normalize....
	ores->testFracs() ;
	ores->getRadiance () ;

	ores->writeOutputRadiance () ;
	cfo->SetMapInfo (mapinfo) ;
	strcpy (header, outfile) ;
	strcat (header, ".hdr") ;
	cfo->WriteHdrFile (header, ns_out, nl_out, 1, 4, 0, .000545455) ;
	

}
