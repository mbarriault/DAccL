//
//  Function.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

// Borrowed from T3nsors T3::Field

#ifndef DAccL_Function_h
#define DAccL_Function_h

#include "Tuple.h"

class Function : public Array<real> {
public:
    Tuple N;
    
    Function(Tuple N=Tuple(1,0)) : N(N) {
        assign(N.Pr(), 0.);
    }
    
    Function(Tuple N, Array<real> F) : N(N), Array<real>(F) {
    }
    
    real& operator()(long a, ...) {
        va_list vl;
        va_start(vl, a);
        Tuple o((long)N.size());
        o[0] = a;
        FRO(i,1,o.size()) {
            o[i] = va_arg(vl, long);
        }
        va_end(vl);
        return at(N.map(o));
    }
    
    real& operator[](long o) {
        return this->at(o);
    }
    
    real operator[](long o) const {
        real val = this->at(o);
        if ( val != val ) {
            std::cout << "Found a NaN!" << o << "," << size() << " " << id << std::endl;
            exit(0);
        }
        return val;
    }
    
    Function operator-() {
        return Function(*this) *= -1;
    }
    
    Function operator+(Function x) {
        return Function(*this) += x;
    }
    
    Function operator-(Function x) {
        return Function(*this) -= x;
    }
    
    Function operator*(Function x) {
        return Function(*this) *= x;
    }
    
    Function operator*(real a) {
        return Function(*this) *= a;
    }
    
    Function operator/(real a) {
        return Function(*this) /= a;
    }
    
    Function& operator+=(const Function& x) {
        increment(x);
        return *this;
    }
    
    Function& operator-=(const Function& x) {
        decrement(x);
        return *this;
    }
    
    Function& operator*=(const Function& x) {
        product(x);
        return *this;
    }
    
    Function& operator*=(real a) {
        expand(a);
        return *this;
    }
    
    Function& operator/=(real a) {
        expand(1./a);
        return *this;
    }
    
    friend Function operator*(real, Function);
    
};

Function operator*(real a, Function x) { return x*a; }

#endif
