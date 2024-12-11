#pragma once

#include <pthread.h>


using ulli = unsigned long long int;

struct  Arg {
    int p = 0;
    int k = 0;
    ulli n = 0;

    ulli begin = 0;
    ulli local_max_gap = 0;
    ulli max_gap_on_segment = 0;
    ulli prime_numbers_on_segment = 0;
    ulli local_prime_count = 0;
    ulli end = 0;

    ulli answer = 0;

    pthread_t tid = 0;
    pthread_barrier_t* barrier = nullptr;

    Arg* copy = nullptr;

    double local_time = 0;
};

void* thread_func(void* args);
double get_cpu_time();
double get_full_time();
bool is_prime_1(ulli number);
bool is_prime_2(ulli number);
