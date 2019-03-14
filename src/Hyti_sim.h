#include <iostream>
#include <vector>
using namespace std ;


class Hyti_sim {
public :	
	Hyti_sim () ;
	~Hyti_sim () ;
	void calc_wavebins () ;
	void find_bins (float *newwaves, vector<int> *wavebin, vector<float> *wd, int number_waves) ;
	void get_geom (float cent_lat, float cent_lon, float *ymeters_deg, float *xmeters_deg) ;
	void load_noise (float *indat, float *outdat, float *noise, int bandnum, int npix) ;
	int nwaves ;
	float *wn_wavebin, *wl_wavebin ;
	float grndres, wn_space, wn_start ;

} ;
