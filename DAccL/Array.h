//
//  Array.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#ifndef DAccL_Array_h
#define DAccL_Array_h

#include "Object.h"

template <class T> class Array : public Object, public std::vector<T> {
public:
    Array() : std::vector<T>() {}

//    Array(std::initializer_list<T> init) : std::vector<T>(init) {}
    
    T& operator[](long i) {
        return this->at( mod(i, (long)this->size()) );
    }
    
    const T operator[](long i) const {
        return this->at( mod(i, (long)this->size()) );
    }
    
    T* pointer() {
        return &this->at(0);
    }
    
    Array<T>& increment(const Array<T>& other) {
        PFOR(i,this->size()) (*this)[i] += other[i];
        return *this;
    }
    
    Array<T>& decrement(const Array<T>& other) {
        PFOR(i,this->size()) (*this)[i] -= other[i];
        return *this;
    }
    
    Array<T>& product(const Array<T>& other) {
        PFOR(i,this->size()) (*this)[i] *= other[i];
        return *this;
    }
    
    Array<T>& negative() {
        PFOR(i,this->size()) (*this)[i] *= -1;
        return *this;
    }
    
    Array<T>& expand(real a) {
        PFOR(i,this->size()) (*this)[i] *= a;
        // Range-based for loop
        //            for ( T& t : *this )
        //                t *= a;
        return *this;
    }
    
    static T _LC(real a, T& M, va_list args) {
        T* x;
        while ( (a = va_arg(args, real)) and (x = va_arg(args, T*)) ) {
            M += (*x) * a;
        }
        va_end(args);
        return M;
    }
    
    static T _LC(real a, T* x, ...) {
        T M = (*x) * a;
        va_list args;
        va_start(args, x);
        return _LC(a, M, args);
    }
    
    
};

#endif
