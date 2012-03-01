//
//  main.cpp
//  Test
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>

#include "System.h"

class testIVP : public System {
public:
    testIVP(Tuple N = Tuple(1,1)) : System(N) {
        finalTime = 10.;
        rPar.assign(1);
        rPar[0] = -1.;
        const real& lambda = rPar[0];
        
        Y(0,0) = 5.;
        Z(0,0) = lambda*Y(0,0);
        
        calcYprime();
        specifyJacobian();
    }

    void Residue(const real& time, const Array<real>& y, const Array<real>& yPrime, Array<real>& residue, int& iRes) {
        const real& lambda = rPar[0];
        
        residue &= yPrime - y*lambda;
    }
    
    void Jacboian(const real& time, const Array<real>& y, const Array<real>& yPrime, Array<real>& PD, real& CJ) {
        const real& lambda = rPar[0];
        
        PD[0] = -lambda + CJ * 1;

    }
};

int main(int argc, const char * argv[])
{
    // insert code here...
    testIVP S;
    S.DASSL();
    
    std::cout << S.Y(0,0) << std::endl;
    std::cout << S.Z(0,0) << std::endl;
    
    return 0;
}

