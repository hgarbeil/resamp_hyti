#include "Hyti_sim.h"
#include "InHdf5.h"
#include "FileOperations.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std ;


int main (int argc,char *argv[]) {
	char infile[420], outfile[420], llfile [420], hdrfile[420]  ;
	int i, nl = 23396, ns=512 ;
	int nl_out, ns_out, npix_out ;
	int insize[3] ;
	float llarr[4], clat, clon, slat, slon, xsize_deg, ysize_deg ;
	float ydist, xdist, totdist, x_scalefac, y_scalefac ;
	float *outarr, *outlatlon ;
	vector<float>outwaves ;
	long npix ;
	CFileOperations *cfo  = new CFileOperations() ;
	cfo->map_info_flag= false ;

	strcpy (infile, *++argv) ;
	strcpy (outfile, *++argv) ;
	

	//strcpy (infile, "../data/HyTES-1542143562717b/HyTES-1542143562717b/20180220t200932_HawaiiHI_L1.hdf5") ;
	//strcpy (infile, "../data/HyTES-1541795246784a/HyTES-1541795246784a/20180206t222742_HawaiiHI_L1.hdf5") ;
	//strcpy (infile, "/hbeta/harold/workdir/resamp_hyti/data/HyTES-1542465703919b/20180203t063508_HawaiiHI_L1.hdf5") ;
	InHdf5 *in_hdf = new InHdf5(infile) ;
	//InHdf5 *in_hdf = new InHdf5(infile) ;
	in_hdf->get_image_size (insize) ;
	nl = insize[2] ;
	ns = insize[1] ;
	npix = long(ns) * nl ;

	Hyti_sim *hsim = new Hyti_sim () ;

	float *ploc = new float [nl * ns * 2] ;
	in_hdf->read_geom (ploc, llarr) ;

	clat = ploc[nl/2*ns*2+ns/2] ;
	clon = ploc[nl/2*ns*2+ns/2+1] ;
	cout << "Center lat and lon " << clat<< "  "<< clon << endl ;
	hsim->get_geom(clat, clon, &ysize_deg, &xsize_deg) ;
	// get output wavelength vector to write to header
	for (i=24; i>=0; i--) {
		outwaves.push_back (hsim->wl_wavebin[i]) ;
	}

	float lat, lon ;
	lat = ploc[0] ;
	lon = ploc[1] ;
	cout << "0\t0\t" << lat << "\t"<< lon << endl ;
	lat = ploc[(ns-1) * 2] ;
	lon = ploc[(ns-1) * 2 + 1] ;
	cout << "0\t" << ns << "\t"<< lat << "\t"<< lon << endl ;
	lat = ploc[(nl-1) * ns * 2 ] ;
	lon = ploc[(nl-1) * ns * 2 + 1] ;
	cout << nl << "\t" <<  "0\t"<< lat << "\t"<< lon << endl ;


	

	
	// Calculate the input pixel size from the top line distances
	ydist = fabs(ploc[0] - ploc[(ns-1) * 2]) ;
	xdist = fabs(ploc[0+1] - ploc[(ns-1) * 2 + 1]) ;
	ydist *= ysize_deg ;
	xdist *= xsize_deg ;
	totdist = sqrt (xdist * xdist + ydist * ydist) ;
	ns_out = totdist / 70. + 1 ;
	x_scalefac = float(ns) / ns_out ;


	ydist = fabs(ploc[0] - ploc[(nl-1) * ns * 2]) ;
	xdist = fabs(ploc[0+1] - ploc[(nl-1) * ns*2 + 1]) ;
	
	ydist *= ysize_deg ;
	xdist *= xsize_deg ;
	totdist = sqrt (xdist * xdist + ydist * ydist) ;
	nl_out = totdist / 70. + 1 ;
	y_scalefac = float (nl) / nl_out ;
	npix_out = ns_out * nl_out ;
	outarr = new float [npix_out *2] ;

	cout << "Output lines is " << nl_out << " samples is : " << ns_out << endl ;

	// create an array to calc the new lat lon arrays and make the
	// third band do the count of orig pixels that comprise an output
	// pixel
	outlatlon = new float [npix_out * 3] ;
	//xsize_deg = 70. / xsize_deg ;
	//ysize_deg = 70. / ysize_deg ;
	
	cout << "meters per deg is : " << ysize_deg << "  "<< xsize_deg << endl ;
	cout << "Output lines and samps : " << nl_out<< " " << ns_out << endl ;

	float *waves = new float [256] ;
	// decide which bin the actual data band belongs to and 
	// then place that band number in wbin
	// and then vcalc its wave dist and place that in wdist
	vector <int> *wbin = new vector<int> [25] ;
	vector <float> *wdist = new vector<float> [25] ;
	//
	// hytes has 256 wavelength bands packed in an array, we read
	// the center scene column, which gives a 1d 256 element array
	in_hdf->read_wavelengths(waves) ;
	int *wavebins = new int [25] ;
	float *bandtotal = new float [npix] ;
	hsim->find_bins (waves, wbin, wdist, 256) ;


	

	int j, ib, nimages, bnum, snum ; ;
	float xloc, yloc ;
	int ixloc, iyloc ; 
	// initialize latlon array
	for (i=0; i<npix_out*3; i++)outlatlon[i]=0. ; 

	FILE *fout = fopen (outfile, "wb") ; 
	// for each band do this...
	
	float *fdatfull = new float [npix*256] ;
	in_hdf->dumpfull (fdatfull) ;
	fwrite (fdatfull, 4, npix * 256, fout) ;
	fclose (fout) ;
	return(0) ;

	fclose (fout) ;

	delete [] outlatlon ;	
	delete [] fdatfull ;
	delete [] bandtotal ;
	delete [] waves ;
	delete in_hdf ;
	delete hsim ;
	

}

