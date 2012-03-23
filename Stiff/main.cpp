//
//  main.cpp
//  Stiff
//
//  Created by Mike Barriault on 12-03-21.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>
#include "System.h"

using namespace DACCL;

struct Stiff : public System {
    real lambda;
    Stiff(real lambda) : System(Tuple(1,3)), lambda(lambda) {
        // y1(0) = (3 + 2exp(-lambda) + exp(-2lambda)) / (2+exp(-lambda))
        // y2(0) = z1(0) = -lambda (1 - exp(-lambda) - 2exp(-2lambda)) / (2+exp(-lambda))
        // y3(0) = z2(0) = lambda^2 (1 + exp(-lambda) + 4exp(-2lambda)) / (2+exp(-lambda)) - pi^2
        // z3(0) = -lambda^3 (1 - exp(-lambda) - 8exp(-2lambda)) / (2+exp(-lambda))
        real elambda = exp(-lambda);
        real e2lambda = exp(-2*lambda);
        Y(0) = (1+elambda+e2lambda)/(2+elambda) + 1;
        Y(1) = Z(0) = -lambda * (1-elambda-2*e2lambda)/(2+elambda);
        Y(2) = Z(1) = lambda*lambda * (1+elambda+4*e2lambda)/(2+elambda) - M_PI*M_PI;
        Z(2) = -lambda*lambda*lambda * (1-elambda-8*e2lambda)/(2+elambda);
        finalTime = 1;
        specifyJacobian();
        iterateSlowly();
        setStopping(1.);
    }
    
    void Step() {
        std::cout << currentTime << "\t";
        FOR(i,3) std::cout << Y(i) << "\t";
        std::cout << std::endl;
    }
    
    Function Residue(const real& t, const Function& y, const Function& z, int& iRes) {
        // z0 - y1 = 0
        // z1 - y2 = 0
        // z2 + 2lambda^3 y1 - lambda^2 y2 - 2lambda y3 - q3(t)
        // q3(t) = (lambda^2 + pi^2) (2lambda cos(pi*t) + pi sin(pi*t))
        auto R = NewFunction();
        R(0) = z(0) - y(1);
        R(1) = z(1) - y(2);
        R(2) = z(2) + 2*lambda*lambda*lambda*y(0) - lambda*lambda*y(1) - 2*lambda*y(2) - (lambda*lambda+M_PI*M_PI)*(2*lambda*cos(M_PI*t)+M_PI*sin(M_PI*t));
        return R;
    }
    
    Matrix Jacobian(const real& t, const Function& y, const Function& z, real& CJ) {
        auto PD = NewMatrix();
        PD(0,0) = CJ;
        PD(0,1) = -1.;
        PD(1,1) = CJ;
        PD(1,2) = -1.;
        PD(2,0) = -2*lambda;
        PD(2,1) = -lambda*lambda;
        PD(2,2) = CJ + 2*lambda*lambda*lambda;
        return PD;
    }
};

int main(int argc, const char * argv[])
{
    Stiff Stiff(50);
    Stiff.DASSL();
    return 0;
}

