//
//  main.cpp
//  Sample
//
//  Created by Mike Barriault on 12-03-20.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>
#include "System.h"

struct Sample : public System {
    Sample() : System(Tuple(1,1)) {
        Y(0) = 1;
        currentTime = 1;
        finalTime = 25;
        specifyJacobian();
        iterateSlowly();
        setMaxOrd(2);
    }
    
    void Step() {
        double e = fabs(Y(0) - 1./currentTime);
        int p = int(log(e)/log(timeStep())+0.5); // 0.5 so number is rounded, not floored
        std::cout << currentTime << "\t" << Y(0) << "\t" << e << "\t" << timeStep() << "\t" << p << std::endl;
    }
    
    Function Residue(const real& t, const Function& y, const Function& z, int& iRes) {
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
    Sample sample;
    sample.DASSL();
    return 0;
}

