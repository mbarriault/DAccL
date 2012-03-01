//
//  Core.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

// Borrowed and modified from T3nsors

#ifndef DAccL_Core_h
#define DAccL_Core_h

#define FOR(n,N) for ( long n=0; n<N; n++ )
#define PFOR(n,N) _Pragma("omp parallel for") for ( long n=0; n<N; n++ )
#define FRO(n,i,N) for ( long n=i; n<N; n++ )
#define PFRO(n,i,N) _Pragma("omp parallel for") for ( long n=i; n<N; n++ )
#define LOOP(N) for ( long __n=0; __n<N; __n++ )

typedef double real;

#include <iomanip>
#include <sstream>
#include <sys/time.h>
#include <cmath>

long mod(long a, long p) {
    return (p + a % p) % p;
}

std::string itos(long i) {
    std::ostringstream out;
    out << i;
    return out.str();
}

std::string ftos(real x) {
    std::ostringstream out;
    out << x;
    return out.str();
}

std::string mkdir(std::string dir) {
    std::string cmd = (std::string)"mkdir " + dir + " 2> /dev/null";
    while ( system(cmd.c_str()) ) {
        for ( unsigned long i = dir.size()-1; i>0; i-- ) {
            dir[i]++;
            if ( dir[i] < 58 ) break;
            else dir[i] -= 10;
        }
        cmd = (std::string)"mkdir " + dir + " 2> /dev/null";
    }
    return dir;
}

std::string timecoord() {
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    char str[12];
    strftime(str, 9, "%U%w%H%M", timeinfo);
    return (std::string)str;
}

timeval gettime() {
    timeval now;
    struct timezone tzp;
    gettimeofday(&now, &tzp);
    return now;
}

long elapsed(timeval before) {
    timeval now = gettime();
    if ( before.tv_usec > now.tv_usec ) {
        now.tv_usec += 1e6;
        now.tv_sec--;
    }
    timeval lapsed;
    lapsed.tv_usec = now.tv_usec - before.tv_usec;
    lapsed.tv_sec = now.tv_sec - before.tv_sec;
    return lapsed.tv_usec + 1e6*lapsed.tv_sec;
}

std::string timerem(timeval before, long multi) {
    std::ostringstream out;
    long lapsed = elapsed(before);
    long left = long((multi*lapsed)/1e6);
    if ( left > 86400 )
        out << left/86400 << "d ";
    if ( left > 3600 )
        out << (left%86400)/3600 << "h ";
    if ( left > 60 )
        out << (left%3600)/60 << "m ";
    out << left%60 << "s" << std::flush;
    return out.str();
}

std::string pad(long n, long w) {
    std::ostringstream out;
    out << std::setw((int)w) << std::setfill('0') << n << std::flush;
    return out.str();
}

std::string pad(real t, real T, real dt) {
    long n = -log10(dt)+2;
    long m = log10(T)+2;
    if ( n >= 0 ) n++;
    long w = n+m;
    if ( n < 0 ) n = 0;
    std::ostringstream out;
    out << std::setw((int)w) << std::setfill('0') << std::setprecision((int)n) << std::fixed << t;
    return out.str();
}


#endif
