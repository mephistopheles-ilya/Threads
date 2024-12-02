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
    int n1 = 0;
    int n2 = 0;
    double* matrix = nullptr;
    pthread_t tid = 0;
    pthread_barrier_t* barrier = nullptr;

    int changed = 0;
    int error = 0;

    double local_time = 0;
};

io_status read_matrix_from_file(FILE* f, double* matrix, int n1, int n2);
void* thread_func(void* args);
double get_cpu_time();
double get_full_time();
