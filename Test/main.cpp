//
//  main.cpp
//  Test
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>

#include "System.h"

struct testIVP : public System {
    real lambda;
    testIVP() : System(Tuple(1,1)) {
        lambda = -100.;
        finalTime = 10.;

        Y = 5.;
        Z = lambda*Y;
        iterateSlowly();
    }
    
    BLANK_STEP
    BLANK_JACOBIAN

    Function Residue(const real& time, const Function& y, const Function& z, int& iRes) {
        return z - y*lambda;
    }
};

int main(int argc, const char * argv[])
{
    testIVP S;
    S.DASSL();
    
    std::cout << "This program should output\n    0.000227006\n    -0.000227006\n" << std::endl;

    
    std::cout << S.Y(0,0) << std::endl;
    std::cout << S.Z(0,0) << std::endl;
    return 0;
}

