#include <iostream>
#include "Utils.h"
#include "FileOperations.h"
using namespace std ;

// program to dump out a specific band in both original radiance
// and in temperature
// for now, make assumption that file is bsq
int main (int argc, char *argv[]) {
	char infile [240], outfile[240], hdrfile[240], tempfile[240] ;
	int i, bnum, ns, nl, nb, npix ;
	float *inrad, *temparr, wlen ;
	CFileOperations *cfo = new CFileOperations () ;


	if (argc < 3) {
		cout << "Useage : dump_band infile outfile bnum " << endl ;
		exit(-1) ;
	}

	strcpy (infile, *++argv) ;
	strcpy (hdrfile, infile) ;
	strcat (hdrfile, ".hdr") ;
	strcpy (tempfile, infile) ;
	strcat (tempfile, "_temp") ;
	strcpy (outfile, *++argv) ;
	bnum = atoi (*++argv) ;

	// read in header and get relevant stuffs
	cfo->ReadEnviHeader (hdrfile) ;
	ns = cfo->samps ;
	nl = cfo->lines ;
	npix = ns * nl ;
	wlen = cfo->waves[bnum] ;
	cout << "NSamps : " << ns << endl ;
	cout << "NLines : " << nl << endl ;
	cout << "WLen   : " << wlen << endl ;

	// alloc arrays 
	inrad = new float [npix] ;
	temparr = new float [npix] ;

	// get the float radiance data -> into inrad
	FILE *fin = fopen (infile, "r") ;
	fseek (fin, npix * long(bnum) * 4, SEEK_SET) ;
	fread (inrad, 4, npix, fin) ;
	fclose (fin) ;

	// convert to temp -> into temparr
	for (i=0; i<npix; i++) {
		temparr[i] = bb2temp (inrad[i], wlen) ;
	}


	FILE *fout = fopen (outfile, "w") ;
	fwrite (inrad, 4, npix, fout) ;
	fclose (fout) ;
	cfo->WriteHeaderFile (hdrfile, ns, nl, 1) ;
	fout = fopen (tempfile, "w") ;
	fwrite (temparr, 4, npix, fout) ;
	fclose (fout) ;
	strcat (tempfile, ".hdr") ;
	cfo->WriteHeaderFile (tempfile, ns, nl, 1) ;
	
	delete [] inrad ;
	delete [] temparr ;
}

