//
// Created by 闵梦涛 on 2021/7/19.
//

#ifndef CRASHSIM_RANDOM_H
#define CRASHSIM_RANDOM_H
#include "random"
#include "SFMT-src-1.4.1/SFMT.h"


using namespace std;

class Random{
public:
    sfmt_t sfmt;
    Random(){
        srand(unsigned(time(0)));
        sfmt_init_gen_rand(&sfmt, rand());
    }

    Random(unsigned seed) {
        srand(unsigned(seed));
        sfmt_init_gen_rand(&sfmt, rand());
    }

    unsigned generateRandom() {
        return sfmt_genrand_uint32(&sfmt);
    }

    double drand(){
        return generateRandom() % RAND_MAX / (double) RAND_MAX;
    }
};
#endif //CRASHSIM_RANDOM_H
