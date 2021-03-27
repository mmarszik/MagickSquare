#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "MRndCPP/include_rnd.h"

using TRnd = MRnd::TMRnd;

constexpr int WIDTH  = 4;
constexpr int HEIGHT = 4;
constexpr int SIZE = WIDTH*HEIGHT;

constexpr int MIN =   0;
constexpr int MAX =  20;

constexpr int POPULATION =  ( 1 << 12 );
//constexpr int BEST       =  ( 1 <<  4 );

struct Genotype {
    int mgSquare[SIZE];
    int& operator[] (const int i) {
        return mgSquare[i];
    }
    int operator[] (const int i) const {
        return mgSquare[i];
    }
};

struct Spec {
    Genotype work;
    Genotype best;
    int eval;
};

int evaluate( const Genotype &genotype ) {
    int weight = 1000000;
    int eval = -SIZE * weight;
    int counts[MAX-MIN+1];
    for( int i=0 ; i<MAX-MIN+1 ; i++ ) {
        counts[i] = 0;
    }
    int min=genotype[0],max=genotype[0];
    for(int i=0 ; i<SIZE ; i++ ) {
        if( genotype[i] > MAX || genotype[i] < MIN ) {
            abort();
        }
        counts[ genotype[i] ]++;
        if( max < genotype[i] ) max = genotype[i];
        if( min > genotype[i] ) min = genotype[i];
    }
    for( int i=0 ; i<MAX-MIN+1 ; i++ ) {
        eval += counts[i] * counts[i] * weight;
    }
    const int base = genotype[0] + genotype[1] + genotype[2] + genotype[3];

    weight = 10000;
    if( base != (genotype[ 0]+genotype[ 1]+genotype[ 2]+genotype[ 3]) ) { eval += weight; }
    if( base != (genotype[ 4]+genotype[ 5]+genotype[ 6]+genotype[ 7]) ) { eval += weight; }
    if( base != (genotype[ 8]+genotype[ 9]+genotype[10]+genotype[11]) ) { eval += weight; }
    if( base != (genotype[12]+genotype[13]+genotype[14]+genotype[15]) ) { eval += weight; }

    if( base != (genotype[ 0]+genotype[ 4]+genotype[ 8]+genotype[12]) ) { eval += weight; }
    if( base != (genotype[ 1]+genotype[ 5]+genotype[ 9]+genotype[13]) ) { eval += weight; }
    if( base != (genotype[ 2]+genotype[ 6]+genotype[10]+genotype[14]) ) { eval += weight; }
    if( base != (genotype[ 3]+genotype[ 7]+genotype[11]+genotype[15]) ) { eval += weight; }

    if( base != (genotype[ 0]+genotype[ 5]+genotype[10]+genotype[15]) ) { eval += weight; }
    if( base != (genotype[ 3]+genotype[ 6]+genotype[ 9]+genotype[12]) ) { eval += weight; }

    if( base != (genotype[ 5]+genotype[ 6]+genotype[ 9]+genotype[10]) ) { eval += weight; }

    if( base != (genotype[ 0]+genotype[ 1]+genotype[ 4]+genotype[ 5]) ) { eval += weight; }
    if( base != (genotype[ 8]+genotype[ 9]+genotype[12]+genotype[13]) ) { eval += weight; }
    if( base != (genotype[ 2]+genotype[ 3]+genotype[ 6]+genotype[ 7]) ) { eval += weight; }
    if( base != (genotype[10]+genotype[11]+genotype[14]+genotype[15]) ) { eval += weight; }

    if( base != (genotype[ 4]+genotype[ 5]+genotype[ 8]+genotype[ 9]) ) { eval += weight; }
    if( base != (genotype[ 1]+genotype[ 2]+genotype[ 5]+genotype[ 6]) ) { eval += weight; }
    if( base != (genotype[ 6]+genotype[ 7]+genotype[10]+genotype[11]) ) { eval += weight; }
    if( base != (genotype[ 9]+genotype[10]+genotype[13]+genotype[14]) ) { eval += weight; }

    weight = 100;
    if( base != (genotype[ 0]+genotype[ 1]+genotype[14]+genotype[15]) ) { eval += weight; }
    if( base != (genotype[ 4]+genotype[ 5]+genotype[10]+genotype[11]) ) { eval += weight; }
    if( base != (genotype[ 8]+genotype[ 9]+genotype[ 6]+genotype[ 7]) ) { eval += weight; }
    if( base != (genotype[12]+genotype[13]+genotype[ 2]+genotype[ 3]) ) { eval += weight; }

    if( base != (genotype[ 0]+genotype[ 4]+genotype[11]+genotype[15]) ) { eval += weight; }
    if( base != (genotype[ 1]+genotype[ 5]+genotype[10]+genotype[14]) ) { eval += weight; }
    if( base != (genotype[ 2]+genotype[ 6]+genotype[ 9]+genotype[13]) ) { eval += weight; }
    if( base != (genotype[ 3]+genotype[ 7]+genotype[ 8]+genotype[12]) ) { eval += weight; }

    if( base != (genotype[ 1]+genotype[ 2]+genotype[13]+genotype[14]) ) { eval += weight; }
    if( base != (genotype[ 4]+genotype[ 8]+genotype[ 7]+genotype[11]) ) { eval += weight; }

//    weight = 1;
//    eval += (max + max - min) * weight;

    return eval;
}

void initSpec( TRnd &rnd, Spec &spec ) {
    for( int i=0 ; i<SIZE ; i++ ) {
        spec.work[i] = rnd.range(MIN,MAX);
    }
    spec.best = spec.work;
    spec.eval = evaluate( spec.work );
}

void initPopulation( TRnd &rnd, Spec population[POPULATION] ) {
    for( int i=0 ; i<POPULATION ; i++ ) {
        initSpec(rnd,population[i]);
    }
//    std::sort( population , population+POPULATION , [](const Spec &a, const Spec &b){ return a.eval < b.eval;} );
}

void chaos( TRnd &rnd, Spec &spec) {
    if( rnd() % 1024 < 512 ) {
        std::swap( spec.work[rnd() % SIZE] , spec.work[rnd() % SIZE] );
    } else {
        spec.work[rnd() % SIZE] = rnd.range(MIN,MAX);
    }
    const int tmp = evaluate(spec.work);
    if( tmp <= spec.eval ) {
        spec.eval = tmp;
        spec.best = spec.work;
    } else if( rnd() % 1024 < 256 ) {
        spec.work = spec.best;
    }
}

void print(const Spec &spec) {
    std::cout << "eval = " << spec.eval << std::endl;
    for( int i=0 ; i<SIZE ; i++ ) {
        const int v = spec.best[i];
        if( v < 100 ) std::cout << " ";
        if( v <  10 ) std::cout << " ";
        std::cout << " " << v;
        if( (i+1) % WIDTH==0 ) {
            std::cout << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    const time_t start = time(NULL);
    (void)argc;
    (void)argv;
    TRnd rnd(1234);
    static Spec population[POPULATION];
    initPopulation(rnd,population);
    std::cout << "time = " << (time(NULL)-start) << std::endl;

    long loop = 0;
    while( population[0].eval > 0 ) {
        double d = rnd.getFloat();
        d *= d;
        int r = (int)( d * POPULATION);
        if( r >= POPULATION ) {
            r--;
        }
        chaos( rnd , population[r] );
        if( r > 0 && population[r].eval < population[r-1].eval ) {
            std::swap( population[r] , population[r-1] );
        }
        if( loop ++ % (1ull<<24) == 0 ) {
            std::cout << "time = " << (time(NULL)-start) << std::endl;
            std::cout << "loop = " << loop / (1<<20) << "M " << std::endl;
            print( population[0] );
        }
    }
    std::cout << "loop = " << loop << std::endl;
    print( population[0] );
    std::cout << "end program...." << std::endl;
    return 0;
}
