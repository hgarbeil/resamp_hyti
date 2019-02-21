#include "hdf5.h"



int main (int argc, char *argv[]) {

	hid_t file_id ;
	int dims[3] ;
	hsize_t offset[3], count[3] ;

	file_id = H5Fopen (infile, H5F_ACC_RDONLY, H5P_DEFAULT) ;
	dataset_id = H5Dopen2 (file_id, "/radiance_data", H5P_DEFAULT) ;

																																																																												//     	    	    	    		    	    	    	    	    	    	    	    	    	    	    	    
																																																																												//
