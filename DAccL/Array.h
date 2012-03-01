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
//#include <initializer_list>

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
        //this->assign(N.Pr(), 0.);
        this->assign(N.Pr());
    }
    
    Array(Tuple N, T* ptr) : N(N) {
        //this->assign(ptr, ptr + N.Pr());
        this->assign(ptr, N.Pr());
    }
    
    Array(Tuple N, const T* ptr) : N(N) {
        //this->assign(ptr, ptr + N.Pr());
        this->assign(ptr, N.Pr());
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
        this->assign(N.Pr());
        memcpy((void*)__begin_, (void*)other.__begin_, N.Pr()*sizeof(T));
    }
    
    Array<T>& operator=(const Array<T>& other) {
        if ( this == &other )
            return *this;
        N = other.N;
        this->assign(N.Pr());
        memcpy((void*)__begin_, (void*)other.__begin_, N.Pr()*sizeof(T));
        return *this;
    }
    
    Array<T>& operator=(Array<T>&& other) {
        N = other.N;
        own = true;
        delete[] __begin_;
        __begin_ = other.__begin_;
        __end_ = other.__end_;
        other.own = false;
        return *this;
    }
    
    Array<T>& assign(long s) {
        own = true;
        __begin_ = new T[s];
        __end_ = &__begin_[s];
        return *this;
    }
    
    Array<T>& assign(const T* ptr, long s) {
        own = false;
        __begin_ = (T*)ptr;
        __end_ = (T*)ptr + s;
        return *this;
    }
    
    T& at(long i) {
        i = mod(i, this->size());
        return __begin_[i];
    }
    
    T at(long i) const {
        i = mod(i, (long)this->size());
        real val = __begin_[i];
        if ( val != val ) {
            std::cout << "Found a NaN!" << i << "," << this->size() << std::endl;
            exit(0);
        }
        return val;
    }
    
    long size() const {
        return __end_-__begin_;
    }
    
//    Array(std::initializer_list<T> init) : std::vector<T>(init) {}
    
    T& operator[](long i) {
        return this->at( i );
    }
    
    T operator[](long i) const {
        return this->at( i );
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
        return this->at(N.map(o));
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
        return this->at(N.map(o));
    }
    
    T* pointer() {
        return __begin_;
    }
    
    Array<T>& set(const Array<T>& other) {
        PFOR(i,this->size()) this->at(i) = other.at(i);
        return *this;
    }
    
    Array<T>& align(const T* ptr) {
        this->assign(ptr, ptr + N.Pr());
        return *this;
    }
    
    Array<T>& increment(const Array<T>& other) {
        PFOR(i,this->size()) this->at(i) += other.at(i);
        return *this;
    }
    
    Array<T>& decrement(const Array<T>& other) {
        PFOR(i,this->size()) this->at(i) -= other.at(i);
        return *this;
    }
    
    Array<T>& product(const Array<T>& other) {
        PFOR(i,this->size()) this->at(i) *= other.at(i);
        return *this;
    }
    
    Array<T>& negative() {
        PFOR(i,this->size()) this->at(i) *= -1;
        return *this;
    }
    
    Array<T>& expand(real a) {
        PFOR(i,this->size()) this->at(i) *= a;
        //for ( T& t : *this )
        //    t *= a;
        return *this;
    }
    
    Array<T> operator-() const {
        return Array<T>(*this) *= -1;
    }
    
    Array<T> operator+(const Array<T>& x) const {
        return Array<T>(*this) += x;
    }
    
    Array<T> operator-(const Array<T>& x) const {
        return Array<T>(*this) -= x;
    }
    
    Array<T> operator*(const Array<T>& x) const {
        return Array<T>(*this) *= x;
    }
    
    Array<T> operator*(real a) const {
        return Array<T>(*this) *= a;
    }
    
    Array<T> operator/(real a) const {
        return Array<T>(*this) /= a;
    }
    
    Array<T>& operator+=(const Array<T>& x) {
        increment(x);
        return *this;
    }
    
    Array<T>& operator-=(const Array<T>& x) {
        decrement(x);
        return *this;
    }
    
    Array<T>& operator*=(const Array<T>& x) {
        product(x);
        return *this;
    }
    
    Array<T>& operator*=(real a) {
        expand(a);
        return *this;
    }
    
    Array<T>& operator/=(real a) {
        expand(1./a);
        return *this;
    }
    
    Array<T>& operator&=(const Array<T>& x) {
        set(x);
        return *this;
    }
    
    friend Array<T> operator*(real, const Array<T>&);
//    friend Array<T> operator*(real, const Array<T>);
    
};

template<class T> Array<T> operator*(real a, const Array<T>& x) { return x*a; }

//template<class T> Array<T> operator*(real a, const Array<T> x) { return x*a; }

#endif
