#include "Hyti_sim.h"
#include "math.h"
#include <iostream>


Hyti_sim::Hyti_sim () {

	nwaves = 25 ;
	wn_start = 930 ;
	wn_space = 13 ;
	grndres = 90. ;
	wn_wavebin = 0L ;
	wl_wavebin = 0L ;
	calc_wavebins () ;

}

Hyti_sim::~Hyti_sim() {
	if (wn_wavebin) 
		delete[] wn_wavebin ;
	if (wl_wavebin) 
		delete[] wl_wavebin ;
}


void Hyti_sim::calc_wavebins () {
	int i ;
	wn_wavebin = new float [nwaves] ;
	wl_wavebin = new float [nwaves] ;

	for (i=0; i<25; i++) {
		wn_wavebin[i] = wn_start + i * wn_space ;
		wl_wavebin[i] = 10000./ wn_wavebin[i] ;
		cout << i << " : " << wn_wavebin[i] << " \t" << wl_wavebin[i] << endl ;
	}

}

void Hyti_sim::find_bins (float *newwaves, vector<int> *wavebin, vector<float> *wavedist, int number_waves){
	int i, binnum ;
	float dist, totval, min_wn, max_wn, wval, cent_bin ;
	float totdist[nwaves] ;

	min_wn = wn_start - wn_space/2. ;
	max_wn = wn_start + (nwaves -1) * wn_space + wn_space/2 ;

	// given a wave file check to see which bin it fits into
	for (i=0; i<nwaves; i++) {
		wavebin[i].clear() ;
		totdist[i] = 0. ;
	}


	for (i=0; i<number_waves; i++) {
		wval = 10000./newwaves[i] ;
		if (wval < min_wn) continue ;
		if (wval > max_wn) continue ;
		binnum = int ((wval - wn_start) / wn_space + 0.5) ;
		cent_bin = wn_start + binnum * wn_space ;
		dist = (6.5 - sqrt((cent_bin-wval)*(cent_bin - wval))) ;
		
		wavebin[binnum].push_back(i) ;
		wavedist[binnum].push_back(dist) ;
		totdist[binnum] += dist ;
		
	}
	int count ;
	for (i=0; i<nwaves; i++) {
		count = wavebin[i].size() ;
		totval = totdist[i] ;
		cout << i << " : " << count <<  endl ;
		for (int is=0 ; is<count; is++) {
			wavedist[i][is] = wavedist[i][is] / totval ;
			cout << wavebin[i].at(is) << "  " << wavedist[i].at(is) << endl ;
			
		}


	}

}


void Hyti_sim::get_geom (float center_lat, float center_lon, float *ymeters_deg, float *xmeters_deg) {
	double eccsq, radius ;
	
	// for the earth
	// earth
	eccsq = .006694380 ;
	radius = 6378137. ;
	
	
	// convert lat to radius
	double center_lat_rad = center_lat / 57.29577951 ;

	double denom2 = pow (1. - eccsq * pow(sin (center_lat_rad), 2),1.5) ;
	double denom2_x = pow (1. - eccsq * pow(sin (center_lat_rad), 2),0.5) ;

	*ymeters_deg = (M_PI / 180. )  * radius * (1. - eccsq) / denom2 ; 
	*xmeters_deg = (M_PI / 180) * radius * cos (center_lat_rad) / denom2_x ;
	
	return ;

}	


