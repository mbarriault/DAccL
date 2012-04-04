//
//  main.cpp
//  Wave
//
//  Created by Mike Barriault on 12-04-01.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#include <iostream>
#include "System.h"

using namespace DACCL;

struct Wave : public System {
    real k; // Spacial grid spacing
    real C; // Circumference
    long m;
    Wave(long m) : System(Tuple(2,m,3)), m(m) {
        C = 2*M_PI;
        finalTime = C;
        k = C/m;
        FOR(i,m) {
            Y(i,0) = cos(i*k);
            Y(i,1) = sin(i*k);
            Y(i,2) = -sin(i*k);
            Z(i,0) = sin(i*k);
            Z(i,1) = -cos(i*k);
            Z(i,2) = cos(i*k);
        }
        setMaxStep(k);
        calcYprime();
    }
    
    void Step() {}
    
    Function Residue(const real& time, const Function& y, const Function& z) {
        // Wave equation
        // z_{tt} - y_{xx} = 0
        // z0 - y1 = 0
        // z1 - (y2(+k)-y2(-k))/(2k) = 0
        // z2 - (y1(+k)-y1(-k))/(2k) = 0
        Function R = NewFunction();
        FOR(i,m) {
            R(i,0) = z(i,0) - y(i,1);
            R(i,1) = z(i,1) - (y(i+1,2)-y(i-1,2))/(2*k);
            R(i,2) = z(i,2) - (y(i+1,1)-y(i-1,1))/(2*k);
        }
        return R;
    }
    
    Matrix Jacobian(const real& time, const Function& y, const Function& z, real& CJ) {
        Matrix PD = NewMatrix();
        FOR(i,m) FOR(j,m) {
            PD(i,0,j,0) = CJ*delta(i,j);
            PD(i,1,j,1) = CJ*delta(i,j);
            PD(i,2,j,2) = CJ*delta(i,j);
            PD(i,0,j,1) = -delta(i,j);
            PD(i,1,j,2) = -(delta(i+1,j)-delta(i-1,j))/(2*k);
            PD(i,2,j,1) = -(delta(i+1,j)-delta(i-1,j))/(2*k);
        }
        return PD;
    }
};

int main(int argc, const char * argv[])
{
    long m = 12;
    typedef std::pair<long,long> MT;
    std::vector<MT> T;
    LOOP(7) {
        timeval start = gettime();
        Wave wave(m);
        wave.DASSL();
        T.push_back(MT(m,elapsed(start)));
        m *= 2;
    }
    FOR(i,T.size()-1) {
        long m1 = T[i].first;
        long m2 = T[i+1].first;
        long t1 = T[i].second;
        long t2 = T[i+1].second;
        std::cout << (log(t2)-log(t1))/(log(m2)-log(m1)) << std::endl;
    }
    return 0;
}

