#pragma once

#include <pthread.h>


using ulli = unsigned long long int;

struct  Arg {
    int p = 0;
    int k = 0;
    ulli n = 0;

    ulli begin = 0;
    ulli min_prime_number = 0;
    ulli max_prime_number = 0;
    ulli max_gap = 0;
    ulli prime_numbers_on_segment = 0;
    ulli end = 0;
    bool stop = false;

    pthread_t tid = 0;

    double local_time = 0;
};

void* thread_func(void* args);
double get_cpu_time();
double get_full_time();
ulli find_prime_numbers(ulli begin, ulli end, Arg* a);
bool is_prime_1(ulli number);
bool is_prime_2(ulli number);
void synchronize(int p, Arg* a);
