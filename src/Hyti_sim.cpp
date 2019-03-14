#include "Hyti_sim.h"
#include "math.h"
#include <iostream>
#include <random>
#include <chrono>
#include <string>


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


/***
 * This routine reads the performance model output of polynomial coefficients 
 * which relates temperature to the NEdT for a lens of 25C
 ***/
void Hyti_sim::load_noise (float *indat, float *outdat, float *noise, int bandnum, int npix) {

	int i ;
	float noisemax, tval, nval, *allcoefs = new float [4*25], *coefs ;
	double total = 0. ;
	unsigned seed = chrono::system_clock::now().time_since_epoch().count() ;

	default_random_engine generator(seed);

	FILE *fcoefs = fopen ("/hbeta/harold/workdir/resamp_hyti/PerfModel/outcoefs","r") ;
	fread (allcoefs, 4, 4 * 25, fcoefs) ;
	fclose (fcoefs) ;
	coefs = allcoefs + 4 * bandnum ;
	normal_distribution <double> distribution (0., 1.) ;
	//uniform_real_distribution<double>distribution (-1.,1.) ;

	for (i=0; i<npix; i++) {
		tval = indat[i] ;
		noisemax = coefs[0] + coefs[1] * tval + coefs[2] * tval * tval + 
			coefs [3] * tval * tval * tval ;
		//noisemax *=  ((bandnum+1) * .0093 + .6677) ;
		noisemax *= .70 * (1. - bandnum/24.) ;
		nval = distribution (generator) * noisemax   ;
		noise[i] = nval ;
		outdat[i] = tval + nval ;
		total+= noisemax ;
	}
	total /= npix ;
	cout << "For band : " << bandnum << "   Ave noise max : " << total << endl ;  

}
		


		



