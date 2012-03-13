CC = cc
FC = gfortran
LIBS = -lgfortran

fortran:
    cd DASSL
    ${FC} -m32 -c *.f
    cd ..

libdaccl:   fortran Core.h Array.h