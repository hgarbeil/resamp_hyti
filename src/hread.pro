pro hread 

infile='/hbeta/harold/workdir/resamp_hyti/data/HyTES-1542143562717a/HyTES-1542143562717a/20180220t203237_HawaiiHI_L1.hdf5'
fid = h5f_open(infile)

dataset_id1=h5d_open(fid, '/radiance_data')
dataspace_id=h5d_get_space (dataset_id1)
dims = h5s_get_simple_extent_dims(dataspace_id) 


for i=0, 255 do begin

fname = string(format='("oband/band_",i03)',i)
print,"processing" , fname
start = [i,0,0]
count = [1,dims[1],dims[2]]
h5s_select_hyperslab, dataspace_id, start, count, stride=[1,1,1],/reset 
memory_space_id=h5s_create_simple(count)
image = h5d_read(dataset_id1, file_space=dataspace_id, memory_space=memory_space_id) 
openw,1,fname
writeu,1,image
close,/all
endfor

print ,dims
stop
end

