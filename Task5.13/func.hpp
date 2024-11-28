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

    int changed = 0;

    bool sequence_in_beginning = false;
    bool sequence_in_ending = false;
    bool one_big_sequence = false;

    double end_of_beginning = 0;
    double begin_of_ending = 0;

    int number_of_end_of_beginning = 0;
    int number_of_begin_of_ending = 0;

    double begin_of_beginning = 0;
    double end_of_ending = 0;

    double after_segemnt_end = 0;
    double before_segment_begin = 0;

    double local_time = 0;

    double segment_begin = 0;
    double segment_end = 0;

    bool end_of_segment_before = false;
    bool begin_of_segment_next = false;

    pthread_barrier_t* barrier = nullptr;

    double new_prev_val = 0;
    double new_next_val = 0;

    void print_structure();
};

io_status read_array_from_file(FILE* f, double* array, int n);
void* thread_func(void* args);
double get_cpu_time();
double get_full_time();
void synchronize(int p, Arg* a);
