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
        lambda = -1.;
        finalTime = 10.;

        Y = 5.;
        Z = lambda*Y;
        
        specifyJacobian();
    }
    
    void Step() {}

    Function Residue(const real& time, const Function& y, const Function& z, int& iRes) {
        return z - y*lambda;
    }
    
    Matrix Jacboian(const real& time, const Function& y, const Function& z, real& CJ) {
        Matrix PD = NewMatrix();
        PD[0] = -lambda + CJ * 1;
        return PD;
    }
};

int main(int argc, const char * argv[])
{
    testIVP S;
    S.DASSL();
    
    std::cout << S.Y(0,0) << std::endl;
    std::cout << S.Z(0,0) << std::endl;
    return 0;
}

