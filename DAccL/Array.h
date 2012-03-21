//
//  Array.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#ifndef DAccL_Array_h
#define DAccL_Array_h

#include "Tuple.h"
#include <iostream>

template<typename T> struct remove_const          { typedef T type; };
template<typename T> struct remove_const<const T> { typedef T type; };
#define nonconst(T) typename remove_const<T>::type

template <typename T> class Array {
private:
    T* __begin_;
    T* __end_;
    bool own;
public:
    Tuple N;
    
    Array() {
        own = false;
    }
    
    Array(long s) : Array(Tuple(1,s)) {}
    
    Array(Tuple N) : N(N) {
        self.assign(N.Pr());
    }
    
    Array(Tuple N, T* ptr) : N(N) {
        self.assign(ptr, N.Pr());
    }
    
    Array(Tuple N, const T* ptr) : N(N) {
        self.assign(ptr, N.Pr());
    }
    
    ~Array() {
        if ( own ) {
            delete __begin_;
        }
        __begin_ = 0;
        __end_ = 0;
        N.clear();
    }
    
    Array(const Array<T>& other) {
        N = other.N;
        self.assign(N.Pr());
        memcpy((void*)__begin_, (void*)other.__begin_, N.Pr()*sizeof(T));
    }
    
    Array<T>& operator=(const Array<T>& other) {
        if ( this == &other )
            return self;
        N = other.N;
        self.assign(N.Pr());
        memcpy((void*)__begin_, (void*)other.__begin_, N.Pr()*sizeof(T));
        return self;
    }
    
    Array<T>& operator=(const real x) {
        at(0) = x;
        return self;
    }
    
    operator real() {
        return at(0);
    }
    
    Array<T>& operator=(Array<T>&& other) {
        N = other.N;
        own = true;
        delete[] __begin_;
        __begin_ = other.__begin_;
        __end_ = other.__end_;
        other.own = false;
        return self;
    }
    
    Array<T>& assign(long s) {
        own = true;
        __begin_ = new T[s];
        __end_ = &__begin_[s];
        FOR(i,s) at(i) = 0.;
        return self;
    }
    
    Array<T>& assign(const T* ptr, long s) {
        own = false;
        __begin_ = (T*)ptr;
        __end_ = (T*)ptr + s;
        return self;
    }
    
    T& at(long i) {
        i = mod(i, self.size());
        return __begin_[i];
    }
    
    T at(long i) const {
        i = mod(i, (long)self.size());
        real val = __begin_[i];
        if ( val != val ) {
            std::cout << "Found a NaN!" << i << "," << self.size() << std::endl;
            throw 0;
            exit(0);
        }
        return val;
    }
    
    long size() const {
        return __end_-__begin_;
    }
    
    T& operator[](long i) {
        return self.at(i);
    }
    
    T operator[](long i) const {
        return self.at(i);
    }
    
    T& operator()(long a, ...) {
        va_list vl;
        va_start(vl, a);
        Tuple o(N.size());
        o[0] = a;
        FRO(i,1,o.size()) {
            o[i] = va_arg(vl, long);
        }
        va_end(vl);
        return self.at(N.map(o));
    }
    
    T operator()(long a, ...) const {
        va_list vl;
        va_start(vl, a);
        Tuple o(N.size());
        o[0] = a;
        FRO(i,1,o.size()) {
            o[i] = va_arg(vl, long);
        }
        va_end(vl);
        return self.at(N.map(o));
    }
    
    T* pointer() const {
        return __begin_;
    }
    
    Array<T>& set(const Array<T>& other) {
        memcpy((void*)self.__begin_, (void*)other.__begin_, N.Pr()*sizeof(T));
        return self;
    }
    
    Array<T>& align(const T* ptr) {
        self.assign(ptr, ptr + N.Pr());
        return self;
    }
    
    Array<T>& increment(const Array<T>& other) {
        PFOR(i,self.size()) self.at(i) += other.at(i);
        return self;
    }
    
    Array<T>& decrement(const Array<T>& other) {
        PFOR(i,self.size()) self.at(i) -= other.at(i);
        return self;
    }
    
    Array<T>& product(const Array<T>& other) {
        PFOR(i,self.size()) self.at(i) *= other.at(i);
        return self;
    }
    
    Array<T>& negative() {
        PFOR(i,self.size()) self.at(i) *= -1;
        return self;
    }
    
    Array<T>& expand(real a) {
        PFOR(i,self.size()) self.at(i) *= a;
        //for ( T& t : *this )
        //    t *= a;
        return self;
    }
    
    Array<T> operator-() const {
        return Array<T>(self) *= -1;
    }
    
    Array<T> operator+(const Array<T>& x) const {
        return Array<T>(self) += x;
    }
    
    Array<T> operator-(const Array<T>& x) const {
        return Array<T>(self) -= x;
    }
    
    Array<T> operator*(const Array<T>& x) const {
        return Array<T>(self) *= x;
    }
    
    Array<T> operator*(real a) const {
        return Array<T>(self) *= a;
    }
    
    Array<T> operator/(real a) const {
        return Array<T>(self) /= a;
    }
    
    Array<T>& operator+=(const Array<T>& x) {
        increment(x);
        return self;
    }
    
    Array<T>& operator-=(const Array<T>& x) {
        decrement(x);
        return self;
    }
    
    Array<T>& operator*=(const Array<T>& x) {
        product(x);
        return self;
    }
    
    Array<T>& operator*=(real a) {
        expand(a);
        return self;
    }
    
    Array<T>& operator/=(real a) {
        expand(1./a);
        return self;
    }
    
    Array<T>& operator&=(const Array<T>& x) {
        set(x);
        return self;
    }
    
};

#endif
