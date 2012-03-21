//
//  Tuple.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

// Borrowed from T3nsors

#ifndef DAccL_Tuple_h
#define DAccL_Tuple_h

#include "Core.h"
#include <vector>

class Tuple : public std::vector<long> {
public:
    Tuple() = default;
    
    Tuple(long n, ...) {
        va_list args;
        va_start(args, n);
        LOOP(n) push_back(va_arg(args, long));
        va_end(args);
    }
    
    Tuple Double() {
        Tuple M = self;
        FOR(i,size()) M.push_back(at(i));
        return M;
    }
    
    long Si(long a=-1) const {
        if ( a== -1 ) a = (long)size();
        long s = 0;
        FOR(i,a) s += at(i);
        return s;
    }
    
    long Pr(long a=-1) const {
        if ( a == -1 ) a = (long)size();
        long p = 1;
        FOR(i,a) p *= at(i);
        return p;
    }
    
    long map(const Tuple& O) const {
        long o = 0;
        FOR(i,size()) {
            long A = mod(O[i], at(i));
            for ( long j=i-1; j>=0; j-- )
                A *= at(j);
            o += A;
        }
        return o;
    }
    
    Tuple map(long o) const {
        Tuple O((long)this->size());
        FOR(i,size()) {
            O[i] = o;
            FOR(j,i) O[i] -= O[j]*Pr(j);
            O[i] /= Pr(i);
            O[i] %= at(i);
        }
        return O;
    }
};

#endif
