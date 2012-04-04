//
//  main.cpp
//  Heat
//
//  Created by Mike Barriault on 12-03-19.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>
#include "System.h"

using namespace DACCL;

struct Heat : public System {
    real kT; // Coefficient of thermal conductivity
    real rho; // Density
    real cp; // Specific heat capacity
    real alpha; // Thermal diffusivity
    real k; // Spacial grid spacing
    real l; // Length of rod
    long m;
    
    Heat(long m) : System(Tuple(1,m+1)), m(m) { }
    
    static Heat Copper(long m) {
        Heat Cu = Heat(m);
        Cu.l = 1.; // m
        Cu.kT = 401; // W/(m K) copper
        Cu.rho = 894; // kg/m^3 copper
        Cu.cp = 385; // J/(kg K) copper
        Cu.Setup();
        return Cu;
    }
    
    static Heat PolyvinylChloride(long m) {
        Heat PVC = Heat(m);
        PVC.l = 1.; // m
        PVC.kT = 0.14; // W/(m K) copper
        PVC.rho = 1300; // kg/m^3 copper
        PVC.cp = 0.9; // J/(kg K) copper
        PVC.Setup();
        return PVC;
    }
    
    void Setup() {
        alpha = kT/(rho*cp);
        finalTime = 120.; // 2 minutes
        k = l/(m+1);
        Y(0) = 373;
        Y(-1) = 273;
        FRO(i,1,m) Y(i) = 293; // Initially at room temperature
        FRO(i,1,m) Z(i) = alpha*(Y(i-1)-2*Y(i)+Y(i+1))/(k*k);
        Z(0) = Z(1);
        Z(-1) = Z(-2);
        setMaxOrd(2);
        setBanded(1, 1);
        nonNegative();
    }
    
    void Step() {
        std::cout << currentTime << "\t";
        FOR(i,m+1) std::cout << Y(i) << "\t";
        std::cout << std::endl;
    }

    Function Residue(const real& time, const Function& y, const Function& z) {
        // Heat equation
        // z - y_{xx} = 0
        // y(0) - 373 = 0
        // y(-1) - 273 = 0
        Function R = NewFunction();
        R(0) = y(0)-373; // Left end in boiling water - z(0) = 0
        FRO(i,1,m) R(i) = z(i) - alpha*(y(i-1)-2*y(i)+y(i+1))/(k*k);
        R(-1) = y(-1)-273; // Right end in ice water - z(0) = 0
        return R;
    }
    
    Matrix Jacobian(const real& time, const Function& y, const Function& z, real& CJ) {
        Matrix PD = NewMatrix();
        int center = bandWidth();
        PD(center,0) = 1.;
        FRO(j,1,m) FRO(i,1,center+2) {
//            PD(i,j) = CJ*delta(i, j) - alpha*(delta(i-1,j) - 2*delta(i,j) + delta(i+1,j))/(k*k); // For unbanded
            PD(i,j) = CJ*delta(i, center) - alpha*(delta(i,center-1) - 2*delta(i,center) + delta(i,center+1))/(k*k); // For banded
        }
        PD(center,-1) = 1.;
        return PD;
    }
};

int main(int argc, const char * argv[])
{
    {
    int m = 19;
    Heat Cu = Heat::Copper(m);
    Cu.DASSL();
    std::cout << "Residue calls for Cu: " << Cu.resCalls() << std::endl;
    std::cout << "Jacobian calls for Cu: " << Cu.jacCalls() << std::endl;
    Heat PVC = Heat::PolyvinylChloride(m);
    PVC.DASSL();
    std::cout << "Residue calls for PVC: " << PVC.resCalls() << std::endl;
    std::cout << "Jacobian calls for PVC: " << PVC.jacCalls() << std::endl;
    std::cout << std::endl;
    }
    {
    int m = 39;
    Heat Cu = Heat::Copper(m);
    Cu.DASSL();
    std::cout << "Residue calls for Cu: " << Cu.resCalls() << std::endl;
    std::cout << "Jacobian calls for Cu: " << Cu.jacCalls() << std::endl;
    Heat PVC = Heat::PolyvinylChloride(m);
    PVC.DASSL();
    std::cout << "Residue calls for PVC: " << PVC.resCalls() << std::endl;
    std::cout << "Jacobian calls for PVC: " << PVC.jacCalls() << std::endl;
    }
    return 0;
}

