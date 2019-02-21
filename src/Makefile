CC = g++ -g -m64 
cc = gcc -g -m64
#CC = /hbeta/harold/Downloads/hdf5/bin/h5c++ -g 
#cc = /hbeta/harold/Downloads/hdf5/bin/h5cc -g 

OBJS_hyti = InHdf5.o Hyti_sim.o main_hyti.o FileOperations.o
#HDFLIB =  -L/home/harold/lhome/hdf5/lib -lz -lhdf5 -lhdf5_cpp -lhdf5_fortran
#HDFLIB =  -L/usr/local/lib -L/home/harold/lhome/hdf5/lib -lsz -lz -lhdf5_hl_cpp -lhdf5_cpp -lhdf5hl_fortran -lhdf5_fortran -lhdf5_hl -lhdf5 
HDFLIB =    -lhdf5_cpp -lhdf5 -lsz -lz 
#HDFINCS = -I/home/harold/Downloads/hdf5/include
#HDFLIB =  -L/usr/local/hdf/lib -lhdf5 -lhdf5_cpp -lhdf5_fortran -lz
#HDFINCS = -I/usr/local/hdf/include

hyti_sim : ${OBJS_hyti}
	${CC} ${OBJS_hyti} ${HDFLIB} -o hyti_sim

resamp_psf : resamp_psf.cpp FileOperations.o InRadImage.o OutResamp.o
	${CC} resamp_psf.cpp FileOperations.o InRadImage.o OutResamp.o -o resamp_psf

install : resamp_psf
	cp resamp_psf /home/harold/programs/imsim/bin/.

FileOperations.o : FileOperations.cpp
	${CC} -c FileOperations.cpp

InRadImage.o : InRadImage.cpp
	${CC} -c InRadImage.cpp

OutResamp.o : OutResamp.cpp
	${CC} -c OutResamp.cpp


.cpp.o :
	${CC} ${HDFINCS} -c $*.cpp

