#pragma once

#include <stdio.h>
#include <pthread.h>

enum class io_status {
    undef, 
    error_read,
    not_enough_elements,
    succes
};

struct Arg {
    int k = 0;
    int p = 0;
    int n = 0;
    double* array = nullptr;
    pthread_t tid = 0;
    pthread_barrier_t* barrier = nullptr;

    int changed = 0;

    double local_time = 0;
    double global_time = 0;
};

io_status read_array_from_file(FILE* f, double* array, int n);
void* thread_func(void* args);
double get_cpu_time();
double get_full_time();
