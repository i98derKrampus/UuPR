#include <complex>
#include <iostream>
#include <sstream>

#include "mkl.h"

void zaxpy(int* N, std::complex<double>* alpha, std::complex<double>* X, int* INCX, std::complex<double>* Y, int* INCY){
    for(int i = 0; i < *N; i++){
        /*std::stringstream ss;
        ss << "Y=" << *Y << " X=" << *X << " alpha=" << *alpha << "\n";
        std::cout << ss.str();*/
        *Y = (*alpha) * (*X) + *Y;
        Y += *INCY;
        X += *INCX;

    }
}