

import h5py
import numpy as np


filename = '../data/HyTES-1541795246784a/HyTES-1541795246784a/20180206t222742_HawaiiHI_L1.hdf5'
fid=h5py.h5f.open(filename)

dset_full=h5py.h5d.open(fid,'/radiance_data') 

dspace = dset_full.get_space()
dims = dspace.get_simple_extent_dims()

print dims
count = (dims[0], dims[1],dims[2])
start = (0,0,0)
block=(1,1,1)
stride=(1,1,1)

myband = np.zeros((dims[0],dims[1],dims[2]), dtype=np.float32)

dset_full.read(h5py.h5s.ALL, dspace, myband[:,:,0])
print myband.shape






