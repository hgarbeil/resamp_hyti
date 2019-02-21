// FileOperations.cpp: implementation of the CFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "FileOperations.h"
#include <fstream>
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileOperations::CFileOperations()
{
	envi2me_dt [0] = 0 ;
	envi2me_dt [1] = 0 ;
	envi2me_dt [2] = 1 ;
	envi2me_dt [3] = 2 ;
	envi2me_dt [4] = 3 ;
	envi2me_dt [5] = 3 ;
	envi2me_dt [6] = 4 ;
	me2envi_dt [0] = 0 ;
	me2envi_dt [1] = 2 ;
	me2envi_dt [2] = 3 ;
	me2envi_dt [3] = 4 ;
	me2envi_dt [4] = 6 ;
	map_info_flag = true ;
}

CFileOperations::~CFileOperations()
{

}

void CFileOperations::SetMapInfo (char *m){
	strcpy (mapInfo, m) ;
	map_info_flag = true ;
}


void CFileOperations::AppendWavelengths (char *filename, vector<float>waves)
{
	char ostr[240] ;
	int i, nwaves ;

	nwaves = waves.size() ;

	FILE *fout = fopen (filename, "a+") ;
	strcpy (ostr,"wavelength = {") ;
	fputs (ostr, fout) ;

	for (i=0; i<nwaves-1; i++) {
		sprintf (ostr, "%f,", waves.at(i)) ;
		fputs(ostr, fout) ; 
	}
	
	sprintf (ostr, "%f}", waves.at(nwaves-1)) ;
	fputs (ostr, fout) ;
	fclose (fout) ;
}

void CFileOperations::AppendFilenames (char *filename, vector<char *>nms) {
	int i, nnames ;
	char ostr [240] ;
	FILE *fout = fopen (filename, "a+") ;

	strcpy (ostr,"band names = {") ;
	fputs (ostr, fout) ;
	nnames = nms.size() ;

	for (i=0; i<nnames-1; i++) {
		sprintf (ostr, "%s,", nms.at(i)) ;
		fputs (ostr, fout) ;
	}

	sprintf (ostr, "%f}", nms.at(nnames-1)) ;
	fputs (ostr, fout) ;
	fclose (fout) ;
}





int CFileOperations::WriteHdrFile(char *filename, int ns, int nl, int nb, int dt, int bform)
{

	// the filename is not appended with the .hdr suffix.
	// therefore, the program should check to make sure it has been passed with a .hdr
	// suffix



	char buf [240], mapfilestr[420], *token ;
	int  len, len1 ;

	strcpy (mapfilestr, 
"map info = {Geographic Lat/Lon, 1.0000, 1.0000, -156.07722144, 19.93694053, 2.7778000000e-004, 2.7778000000e-004,WGS-84, units =Degrees}") ;
	token = strstr (filename, ".hdr") ;
	len = strlen (filename) ;
	len1 = strlen (token) ;
	if (len1 <4) return (-1) ;
	ofstream ofil (filename, ios::out) ;
	if (ofil.bad() )
		return (-1) ;
	sprintf (buf, "ENVI\r\ndescription = {\r\nOutput from getRad}") ;
	ofil << buf << endl ;
	sprintf (buf, "samples = %5d", ns) ;
	ofil << buf << endl ;

	sprintf (buf, "lines = %d", nl) ;
	ofil << buf << endl ;

	sprintf (buf, "bands = %d", nb) ;
	ofil << buf << endl ;

	//dt = me2envi_dt [dt] ;
	sprintf (buf, "data type = %1d", dt) ;
	ofil << buf << endl ;

	switch (bform) {
	case 0 :
			strcpy (buf, "interleave = bsq") ;
			ofil << buf << endl ;
			break ;
	case 1 :
			strcpy (buf, "interleave = bil") ;
			ofil << buf << endl ;
			break ;
	case 2 :
			strcpy (buf, "interleave = bip") ;
			ofil << buf << endl ;
			break ;
	}
	ofil << buf << endl ;

	if (map_info_flag) {
		ofil << mapfilestr << endl ;
	}
}



int CFileOperations::WriteHdrFile(char *filename, int ns, int nl, int nb, int dt, int bform, float space)
{

	// the filename is not appended with the .hdr suffix.
	// therefore, the program should check to make sure it has been passed with a .hdr
	// suffix



	char buf [240], mapfilestr[420], *token ;
	int  len, len1 ;



	sprintf (mapfilestr, 
"map info = {Geographic Lat/Lon, 1.0000, 1.0000, -156.07722144, 19.93694053, %f, %f,WGS-84, units =Degrees}", space,
space) ;
	token = strstr (filename, ".hdr") ;
	len = strlen (filename) ;
	len1 = strlen (token) ;
	if (len1 <4) return (-1) ;
	ofstream ofil (filename, ios::out) ;
	if (ofil.bad() )
		return (-1) ;
	sprintf (buf, "ENVI\r\ndescription = {\r\nOutput from getRad}") ;
	ofil << buf << endl ;
	sprintf (buf, "samples = %5d", ns) ;
	ofil << buf << endl ;

	sprintf (buf, "lines = %d", nl) ;
	ofil << buf << endl ;

	sprintf (buf, "bands = %d", nb) ;
	ofil << buf << endl ;

	//dt = me2envi_dt [dt] ;
	sprintf (buf, "data type = %1d", dt) ;
	ofil << buf << endl ;

	switch (bform) {
	case 0 :
			strcpy (buf, "interleave = bsq") ;
			ofil << buf << endl ;
			break ;
	case 1 :
			strcpy (buf, "interleave = bil") ;
			ofil << buf << endl ;
			break ;
	case 2 :
			strcpy (buf, "interleave = bip") ;
			ofil << buf << endl ;
			break ;
	}
	ofil << buf << endl ;

//	if (map_info_flag) {
//		ofil << mapfilestr << endl ;
//	}

	
	return (0) ;
}


int CFileOperations::ReadEnviHeader(char *pathname)
{

	int  dt ;
	char linedat [440], *token, *newtoken ;

	ifstream ifil (pathname, ios::in) ;
	newtoken = strstr(pathname,".hdr") ;


	strncpy (filename, pathname, newtoken-pathname) ;
	strcpy (filename + (newtoken-pathname), "") ;


	while (!ifil.eof()){

		ifil.getline (linedat, 440, '\n') ;
		if (strlen (linedat) < 2 && ifil.eof()) break ;
		if (strlen (linedat)< 2) continue ;
		token = strtok (linedat, "=") ;
		if (token == 0x0) continue ;
			// find the issues of concern

		if (!strncmp (token, "samp", 4)) {
			token = strtok (NULL, "=") ;
			samps = atoi (token) ;
		}

		if (!strncmp (token, "line", 4)) {
			token = strtok (NULL, "=") ;
			lines = atoi (token) ;
		}

		if (!strncmp (token, "bands", 5)) {
			token = strtok (NULL, "=") ;
			bands = atoi (token) ;
		}

		if (!strncmp (token, "data", 4)) {
			token = strtok (NULL, "=") ;
			dt = atoi (token) ;
			dtype = envi2me_dt [dt] ;
		}
		if (!strncmp (token, "inte", 4)) {
			token = strtok (NULL, "=") ;
			if (!strncmp(token, "bsq",3)) bform = 0 ;
			if (!strncmp(token, "bil",3)) bform = 1 ;
			if (!strncmp(token, "bip",3)) bform = 2 ;
		}



		//TRACE ("%s\n", linedat) ;
	}
	ifil.close() ;



	return (0) ;
}


int  CFileOperations::GetMapInfo(char *pathname, char *mapinfo)
{

	int  dt ;
	char linedat [440], *token, *newtoken ;

	ifstream ifil (pathname, ios::in) ;
	newtoken = strstr(pathname,".hdr") ;


	strncpy (filename, pathname, newtoken-pathname) ;
	strcpy (filename + (newtoken-pathname), "") ;


	while (!ifil.eof()){

		ifil.getline (linedat, 440, '\n') ;
		if (strlen (linedat) < 2 && ifil.eof()) break ;
		if (!strncmp (linedat, "map", 3)){
			strcpy (mapinfo, linedat) ;
			return  (1);
		}


	}
	ifil.close() ;



	return (0) ;
}


