//
//  main.cpp
//  Sample
//
//  Created by Mike Barriault on 12-03-20.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>
#include "System.h"

using namespace DACCL;

struct Sample : public System {
    Sample(real hMax=0) : System(Tuple(1,1)) {
        Y(0) = 1;
        currentTime = 1;
        finalTime = 25;
        specifyJacobian();
        iterateSlowly();
        setMaxOrd(5);
        if ( hMax) setMaxStep(hMax/2);
    }
    
    Function Residue(const real& t, const Function& y, const Function& z) {
        // z + 5ty^2 - 5/t + 1/t^2
        auto R = NewFunction();
        R(0) = z(0) + 5*t*y(0)*y(0) - 5./t + 1./(t*t);
        return R;
    }
    
    Matrix Jacobian(const real& t, const Function& y, const Function& z, real& CJ) {
        auto PD = NewMatrix();
        PD(0,0) = CJ + 10*t*y(0);
        return PD;
    }
};

int main(int argc, const char * argv[])
{
    Sample sample1;
    sample1();
    Sample sample2(sample1.timeStep());
    sample2();
    real err1 = fabs(sample1.Y(0)-1./sample1.currentTime);
    real err2 = fabs(sample2.Y(0)-1./sample1.currentTime);
    real h1 = sample1.timeStep();
    real h2 = sample2.timeStep();
    std::cout << log(err1/err2)/log(h1/h2) << std::endl; // Should be close to MAXORD
    return 0;
}

