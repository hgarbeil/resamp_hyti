#include "InHdf5.h"
#include <iostream>
#include "Hyti_sim.h" 
#include <vector>
using namespace std ;



InHdf5::InHdf5 (char *infile) {
	hid_t hid ;

	h5f = new H5File (infile, H5F_ACC_RDONLY) ;
	hid = h5f->getId() ;

	cout << "H5 file id is : " << hid << endl ;



}

InHdf5::~InHdf5 () {
	delete h5f ;
}

/***
 * read_wavelengths 
 * gets the wavelength array from the midpoint of the image
 */
void InHdf5::read_wavelengths (float *waves) {
	int i, ndims ;
	hsize_t dims [5] ;
	float *pixloc ;
	DataSet dset ;
	DataSpace dspace ;
	H5T_class_t type_class ;
	string dset_name ("/wave_matrix") ;
	dset = h5f->openDataSet (dset_name) ;
	type_class = dset.getTypeClass() ;

	// for hytes 512 lines by 256 wavelengths
    dspace = dset.getSpace () ;
    ndims = dspace.getSimpleExtentDims (dims, NULL) ;

	nl = dims [0] ;
	ns = dims [1] ;

	// get the middle wavelength 
	hsize_t count_out[] = {1, 256} ;
	hsize_t offset_out[]={256,0} ;
	hsize_t stride[] = {1,1} ;
	hsize_t block[] = {1,1} ;
	
	DataSpace mspace1 (ndims, count_out) ;

	
	dspace.selectHyperslab (H5S_SELECT_SET, count_out, offset_out, stride, block) ;
	dset.read (waves, PredType::NATIVE_FLOAT, mspace1, dspace) ;

}

/*** read_geom 
 * reads the pixel geolocation dataset
 * for hytes this is nlines by 512 samples by 4 
 * and at each location we have lat, lon, alt, range to target
 */
void InHdf5::read_geom (float *ploc, float *mnmx) {
	int i, npts, ndims, samp ;
	hsize_t dims [5] ;
	float *pixloc, minlat, minlon, maxlat, maxlon ;

	minlat=1.E9 ;
	minlon=1.E9 ;
	maxlat=-1.E9 ;
	maxlon=-1.E9 ;

	DataSet dset ;
	DataSpace dspace ;
	H5T_class_t type_class ;
	string dset_name ("/pixel_geolocation") ;
	dset = h5f->openDataSet (dset_name) ;
	type_class = dset.getTypeClass() ;

    dspace = dset.getSpace () ;
    ndims = dspace.getSimpleExtentDims (dims, NULL) ;

	nl = dims [0] ;
	ns = dims [1] ;
	npts = dims [2] ;

	hsize_t count_out[] = {nl, ns, 2} ;
	hsize_t offset_out[]={0,0,0} ;
	hsize_t stride[] = {1,1,1} ;
	hsize_t block[] = {1,1,1} ;
	
	DataSpace mspace1 (ndims, count_out) ;
	dspace.selectHyperslab (H5S_SELECT_SET, count_out, offset_out, stride, block) ;
	dset.read (ploc, PredType::NATIVE_FLOAT, mspace1, dspace) ;
	// check array bounds for min and max lat and lon
	samp = 0 ;
	if(ploc[samp] >maxlat) maxlat=ploc[samp] ;
	if(ploc[samp] <minlat) minlat=ploc[samp] ;
	if(ploc[samp+1] >maxlon) maxlon=ploc[samp+1] ;
	if(ploc[samp+1] <minlon) minlon=ploc[samp+1] ;
	samp = (ns-1)*2  ;
	if(ploc[samp] >maxlat) maxlat=ploc[samp] ;
	if(ploc[samp] <minlat) minlat=ploc[samp] ;
	if(ploc[samp+1] >maxlon) maxlon=ploc[samp+1] ;
	if(ploc[samp+1] <minlon) minlon=ploc[samp+1] ;
	samp = (nl-1)*ns*2  ;
	if(ploc[samp] >maxlat) maxlat=ploc[samp] ;
	if(ploc[samp] <minlat) minlat=ploc[samp] ;
	if(ploc[samp+1] >maxlon) maxlon=ploc[samp+1] ;
	if(ploc[samp+1] <minlon) minlon=ploc[samp+1] ;
	samp = (nl-1)*ns*2+(ns-1)*2  ;
	if(ploc[samp] >maxlat) maxlat=ploc[samp] ;
	if(ploc[samp] <minlat) minlat=ploc[samp] ;
	if(ploc[samp+1] >maxlon) maxlon=ploc[samp+1] ;
	if(ploc[samp+1] <minlon) minlon=ploc[samp+1] ;
	mnmx[0] = minlat ;
	mnmx[1] = maxlat ;
	mnmx[2] = minlon ;
	mnmx[3] = maxlon ;



	dset.close() ;

}

void InHdf5::get_geom_extent (float *llarr) {
	

}


void InHdf5::get_image_size (int *mydims) {
	int i, ndims ;
	hsize_t dims [5] ;
	float *pixloc ;
	DataSet dset ;
	DataSpace dspace ;
	H5T_class_t type_class ;
	string dset_name ("/radiance_data") ;
	dset = h5f->openDataSet (dset_name) ;
	type_class = dset.getTypeClass() ;
	
	dspace = dset.getSpace () ;
	ndims = dspace.getSimpleExtentDims (dims, NULL) ;
	for (i=0; i<ndims; i++) {
		cout << i<< "  " << dims[i] << endl ;
	}
	nl = dims [0] ;
	ns = dims [1] ;
	nb = dims [2] ;
	mydims [0] = nb ;
	mydims [1] = ns ;
	mydims [2] = nl ;
}
	
void InHdf5::read_band (int band, float *banddat) {
	hid_t hid ;
	int i, ndims ;
	hsize_t dims [3], dims_out[3] ;
	float *pixloc ;
	DataSet dset ;
	DataSpace dspace ;
	H5T_class_t type_class ;



	string dset_name ("/radiance_data") ;
	dset = h5f->openDataSet (dset_name) ;
	type_class = dset.getTypeClass() ;
	
	// get dataspace dimensions
	dspace = dset.getSpace () ;
	ndims = dspace.getSimpleExtentDims (dims, NULL) ;
	for (i=0; i<ndims; i++) {
		cout << i<< "  " << dims[i] << endl ;
	}

	
	dims_out[0] = dims [0] ;
	dims_out[1] = dims [1] ;
	dims_out[2] = dims [2] ;
	DataSpace tspace (3, dims) ;

	hsize_t count_out[3] ;
	hsize_t offset_out[]={0,0,band} ;
	hsize_t stride[] = {1,1,1} ;
	hsize_t block[] = {1,1,1} ;
	
	count_out[0] = dims[0] ;
	count_out[1] = dims[1] ;
	count_out[2] = 1 ;
	
	//offset_out[0] = iblock ;
	DataSpace mspace1 (ndims, count_out) ;

	dspace.selectHyperslab (H5S_SELECT_SET, count_out, offset_out) ;
	dset.read (banddat, PredType::NATIVE_FLOAT, mspace1, dspace) ;
	dset.close() ;
	mspace1.close() ;
	dspace.close() ;

}
	

void InHdf5::read_lines (int start_line, int nlines_sub, float *banddat) {
	int i, ndims ;
	hsize_t dims [5] ;
	float *pixloc ;
	DataSet dset ;
	DataSpace dspace ;
	H5T_class_t type_class ;
	string dset_name ("/radiance_data") ;
	dset = h5f->openDataSet (dset_name) ;
	type_class = dset.getTypeClass() ;
	
	dspace = dset.getSpace () ;
	ndims = dspace.getSimpleExtentDims (dims, NULL) ;
	for (i=0; i<ndims; i++) {
		cout << i<< "  " << dims[i] << endl ;
	}
	nl = dims [0] ;
	ns = dims [1] ;
	nb = dims [2] ;
	hsize_t count_out[3] ;
	hsize_t offset_out[]={start_line,0,0} ;
	hsize_t stride[] = {1,1,1,} ;
	hsize_t block[] = {1,1,1,} ;
	
	count_out[0] = nlines_sub ;
	count_out[1] = ns ;
	count_out[2] = nb ;
	DataSpace mspace1 (ndims, count_out) ;

	dspace.selectHyperslab (H5S_SELECT_SET, count_out, offset_out, stride, block) ;
	dset.read (banddat, PredType::NATIVE_FLOAT, mspace1, dspace) ;
	
}



/*
int main (int argc,char *argv[]) {
	int nl = 23396 ;
	InHdf5 *in_hdf = new InHdf5("/hbeta/harold/workdir/resamp_psf/data/HyTES-1542143562717a/HyTES-1542143562717a/20180220t203237_HawaiiHI_L1.hdf5") ;
	Hyti_sim *hsim = new Hyti_sim () ;
	float *fdat = new float [4096L *  512 * 256] ;
	float *ploc = new float [nl * 512 * 3] ;
	float *waves = new float [256] ;
	vector <int> *wbin = new vector<int> [25] ;
//	in_hdf->read_geom(ploc) ;
	for (int i=0; i<256 ; i++ ) {
		cout << ploc[i*512 * 3] <<  "  " << ploc[i*512*3+ 1] << " " <<
			ploc[i*512*3+2] << endl ;
	}
	// hytes has 256 wavelength bands packed in an array, we read
	// the center scene column, which gives a 1d 256 element array
	in_hdf->read_wavelengths(waves) ;
	int *wavebins = new int [25] ;
	hsim->find_bins (waves, wbin, 256) ;
	
	
//	for (int i=0; i<256 ; i++ ) 
//		cout << i << "  : " <<waves[i] << endl ;
	in_hdf->read_lines (100, 1024, fdat) ;
	//in_hdf->read_band (220, fdat) ;
	FILE *fout = fopen ("test.dat", "wb") ; 
	fwrite (fdat, 4, 1024L * 256 * 512, fout) ;
	fclose (fout) ;
	//

	
	delete [] fdat ;
	delete [] waves ;
	delete in_hdf ;
	delete hsim ;

}
*/

