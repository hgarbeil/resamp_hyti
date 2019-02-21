/*
 *  * To change this license header, choose License Headers
 *  in Project Properties.
 *   * To change this template file, choose Tools |
 *   Templates
 *    * and open the template in the editor.
 *     */

/* 
 *  * File:   viirs_hdf.h
 *   * Author: harold
 *    *
 *     * Created on July 13, 2018, 8:48 AM
 *      */

#ifndef HDF5_H
#define HDF5_H


#include "H5Cpp.h"
using namespace H5 ;

class InHdf5 {
public :

	int ns, nl, nb ;
	H5File *h5f ;
	InHdf5 (char *) ;
	~InHdf5 () ;
	void read_lines(int startl, int nl, float *fdat) ;
	void get_image_size (int *dsize) ;
	void read_band (int, float *) ;
	void read_geom (float *, float *) ;
	void read_wavelengths (float *) ;
	void get_geom_extent (float *llarr) ;
} ;
#endif 
