// FileOperations.h: interface for the CFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CFO
#define CFO


#include <vector>
#include <string.h>
#include <stdlib.h>

using namespace std ;
class CFileOperations
{
public:
	int WriteHdrFile(char *fname, int ns,int nl, int nb, int dt, int bform) ;
	int WriteHdrFile(char *fname, int ns,int nl, int nb, int dt, int bform, float space);
	void AppendWavelengths (char *fname, vector<float>wv) ;
	void AppendFilenames (char *filename, vector<char *>nms) ;
	int ReadEnviHeader (char *fname) ;
	int GetMapInfo (char *fname, char *mapinfo) ;
	void SetMapInfo (char *) ;
	void Get_BIP_Band (char *, float *dat, int band) ;
	CFileOperations();
	virtual ~CFileOperations();

	char filename [420], mapInfo[1024] ;
	float *waves ;
	int me2envi_dt [7] ;
	int envi2me_dt [7] ;
	int samps ;
	int lines ;
	int bands ;
	int dtype ;
	int bform ;
	bool map_info_flag ;

};

#endif

