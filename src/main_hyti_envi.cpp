#include "Hyti_sim.h"
#include "InHdf5.h"
#include "Utils.h"
#include "FileOperations.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std ;


// this is the main procedure when the hytes file is provided as an
// envi file rather than an hdf5 file
// uses the CFileOperation class to read header and data....


int main (int argc,char *argv[]) {
	char infile[420], outfile[420], llfile [420], hdrfile[420]  ;
	char inhdrfile [420], outhdrfile[420], outllfile[420] ;
	char outtempfile [420], outtemphdrfile[420], outtnoisefile[420] ;
	char outrnoisefile [420], outrnoisehdrfile[420] ;
	char l1_file[420] ;
	int i, nl = 23396, ns=512, nbands=256 ;
	int nl_out, ns_out, npix_out ;
	int insize[3] ;
	float llarr[4], clat, clon, slat, slon, xsize_deg, ysize_deg ;
	float *inlat, *inlon, wavelen ;
	float bfrac ;
	float ydist, xdist, totdist, x_scalefac, y_scalefac ;
	float *outarr, *outlatlon, *outarr_temp, *outarr_tnoise, *outarr_rad, *tmparr ;
	vector<float>outwaves ;
	long npix ;
	CFileOperations *cfo  = new CFileOperations() ;
	cfo->map_info_flag= false ;

	strcpy (l1_file, *++argv) ;
	strcpy (llfile, *++argv) ;
	strcpy (outfile, *++argv) ;
	strcpy (outllfile, outfile) ;
	strcat (outllfile, "_ll") ;
	strcpy (outtempfile, outfile) ;
	strcat (outtempfile, "_temp") ;
	strcpy (outrnoisefile, outfile) ;
	strcat (outrnoisefile, "_rnoise") ;
	strcpy (outtnoisefile, outfile) ;
	strcat (outtnoisefile, "_tnoise") ;
	strcpy (outrnoisehdrfile, outrnoisefile) ;
	strcat (outrnoisehdrfile, ".hdr") ;
	strcpy (inhdrfile, l1_file) ;
	strcat (inhdrfile, ".hdr") ;
	strcpy (outhdrfile, outfile) ;
	strcat (outhdrfile, ".hdr") ;
	strcpy (outtemphdrfile, outtempfile) ;
	strcat (outtemphdrfile, ".hdr") ;
	


	cfo->ReadEnviHeader (inhdrfile) ;
	nl = cfo->lines ;
	ns = cfo->samps ;
	npix = long(ns) * nl ;

// read in the lat lon which is in the geo.dat file
// bip with 1st two bands being lat and lon, second alt
// which we ill ignore
	tmparr = new float [3] ;

	float *ploc = new float [nl * ns * 2] ;
	
	FILE *fll = fopen (llfile, "r") ;
	for (i=0; i<ns * nl; i++) {
		fread (tmparr, 4, 3, fll) ;
		ploc[i*2] = tmparr[0] ;
		ploc[i*2+1] = tmparr[1] ;
	}
	fclose (fll) ;
	delete [] tmparr ;



	Hyti_sim *hsim = new Hyti_sim () ;

	//in_hdf->read_geom (ploc, llarr) ;

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
	outarr_temp = new float [npix_out] ;
	outarr_tnoise = new float [npix_out] ;
	outarr_rad = new float [npix_out] ;

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
	//in_hdf->read_wavelengths(waves) ;
	int *wavebins = new int [25] ;
	float *bandtotal = new float [npix] ;
	hsim->find_bins (cfo->waves, wbin, wdist, 256) ;


	// make this array plenty big to hold multiple bands of bip data for full
	// scene, not sure if it can get this much memory, it is alot!!!!
	float *fdatfull = new float [npix*256L] ;
	FILE *finfull = fopen (l1_file, "rb") ;
	cout << "Memory allocated and data rad in..." << endl;
	

	int j, ib, minband, nbsub, nimages, bnum, snum ; ;
	float xloc, yloc ;
	int ixloc, iyloc ; 
	// initialize latlon array
	for (i=0; i<npix_out*3; i++)outlatlon[i]=0. ; 

	FILE *fout = fopen (outfile, "wb") ; 
	FILE *fout_temp = fopen (outtempfile, "wb") ; 
	FILE *fout_tnoise = fopen (outtnoisefile, "wb") ; 
	FILE *fout_rnoise = fopen (outrnoisefile, "wb") ; 
	FILE *fout_ll = fopen (outllfile, "wb") ; 
	// for each band do this...
	float *mynoise = new float [npix_out] ;

	for (int ibandout=24; ibandout>=0; ibandout--) {

	for (i=0; i<npix; i++) bandtotal[i] = 0. ;
	for (i=0; i<npix_out*2; i++)outarr[i]=0. ; 
	minband = wbin[ibandout].at(0) ;
	nbsub = wbin[ibandout].size() ;
	//get_BIP_data (fdatfull, l1_file, ns, nl, minband, nbsub, nbands) ; 
	for (ib=0; ib < wbin[ibandout].size(); ib++) {
		bnum = wbin[ibandout].at(ib) ;
		bfrac = wdist[ibandout].at(ib) ;
		cout << "Processing band " << bnum << endl ;
		// read in the band from the data file which is now bsq
		fseek (finfull, long(bnum) * 4L * npix, SEEK_SET) ;
		fread (fdatfull, 4, npix, finfull) ;
		//in_hdf->read_band (bnum, fdatfull) ;
		for (i=0; i<npix; i++) {
			bandtotal[i] += bfrac * fdatfull[i] ;
			
			
			//	bandtotal[i*ns+j] += wdist[ibandout].at(ib) * fdatfull[i*ns*256+j*nbsub+ib] ;
			//}
		}
	}

	for (i=0; i<nl; i++) {
		yloc = (i / y_scalefac) ;
		for (j=0; j<ns; j++) {
			xloc = j / x_scalefac ;
			if (xloc < 0) continue ;
			if (yloc < 0) continue ;
			if (xloc >= ns_out) continue ;
			if (yloc >= nl_out) continue ;
			ixloc = int (xloc) ;
			iyloc = int (yloc) ;
			snum = iyloc * ns_out + ixloc ;
			outarr[snum] += bandtotal[i*ns+j] ;
			outarr[snum+npix_out] += 1 ;
		}
		}
		for (i=0;i<npix_out; i++) {
			if (outarr[i+npix_out]>.9) 
				outarr[i] /= outarr[i+npix_out] ;
			outarr_temp [i] = bb2temp (outarr[i],outwaves.at(ib)) ;

		}
		fwrite (outarr, 4, npix_out, fout) ;
		hsim->load_noise (outarr_temp, outarr_tnoise, mynoise, 24-ibandout, npix_out) ;
		// now that we have a new temp image -> convert to a noise radiance
		// image
		fwrite (outarr_temp, 4, npix_out, fout_temp) ;
		bb_temp_to_rad_arr (outarr_tnoise, outarr_rad, npix_out, outwaves.at(ib)) ;
		fwrite (outarr_tnoise, 4, npix_out, fout_tnoise) ;
		fwrite (outarr_rad, 4, npix_out, fout_rnoise) ;
	}
	// fill in the lat lon array, already initialized to zero
	for (i=0; i<nl; i++) {
		yloc = (i / y_scalefac) ;
		for (j=0; j<ns; j++) {
			xloc = j / x_scalefac ;
			if (xloc < 0) continue ;
			if (yloc < 0) continue ;
			if (xloc >= ns_out) continue ;
			if (yloc >= nl_out) continue ;
			ixloc = int (xloc) ;
			iyloc = int (yloc) ;
			//lat
			outlatlon [iyloc * ns_out + ixloc] += 
				ploc[i*ns*2+j*2] ;
			// lon goes into second bsq band
			outlatlon [ns_out * nl_out + iyloc * ns_out + ixloc] += 
				ploc[i*ns*2+j*2 + 1] ;
			// count goes into third bsq band
			outlatlon [2 * ns_out * nl_out + iyloc * ns_out + ixloc] += 1 ; 
		}
	}
	// now get the avg loc by dividing by the count in the third band
	// of the array
	for (i=0; i<npix_out; i++) {
		outlatlon [i] = outlatlon[i] / outlatlon[npix_out*2+i] ;
		outlatlon [npix_out + i] = outlatlon[npix_out +  i] / outlatlon[npix_out*2+i] ;
	}

	
	fwrite (outlatlon, 4, npix_out * 2, fout_ll) ;
	cfo->WriteHdrFile (outhdrfile, ns_out,nl_out, 25, 4, 0) ; 
	cfo->AppendWavelengths(outhdrfile, outwaves) ;
	// write header for latlon
	strcpy (hdrfile, outllfile) ;
	strcat (hdrfile, ".hdr") ;
	cfo->WriteHdrFile (hdrfile, ns_out,nl_out, 2, 4, 0) ; 
	cfo->WriteHdrFile (outtemphdrfile, ns_out, nl_out, 25, 4, 0) ;
	cfo->AppendWavelengths(outtemphdrfile, outwaves) ;
	cfo->WriteHdrFile (outrnoisehdrfile, ns_out, nl_out, 25, 4, 0) ;
	cfo->AppendWavelengths(outrnoisehdrfile, outwaves) ;
	strcpy (hdrfile, outtnoisefile) ;
	strcat (hdrfile, ".hdr") ;
	cfo->WriteHdrFile (hdrfile, ns_out,nl_out, 25, 4, 0) ; 
	cfo->AppendWavelengths(hdrfile, outwaves) ;
		

cout << "Number of lines and samps in output " << nl_out<< " " << ns_out << endl ;


	// once all bands have been compiled into a new band, then geom it
	// out
	
		
		
	
	
//	for (int i=0; i<256 ; i++ ) 
//		cout << i << "  : " <<waves[i] << endl ;
	//in_hdf->read_lines (100, 1024, fdat) ;
	//in_hdf->read_band (220, fdat) ;
	fclose (fout) ;
	fclose (fout_ll) ;
	fclose (fout_temp) ;
	fclose (fout_tnoise) ;
	fclose (finfull) ;

	delete [] outlatlon ;	
	delete [] fdatfull ;
	delete [] bandtotal ;
	delete [] waves ;
	//delete in_hdf ;
	delete hsim ;
	delete [] outarr ;
	delete [] outarr_temp ;
	delete [] outarr_tnoise ;
	delete [] outarr_rad ;
	delete [] mynoise ;
	

}

