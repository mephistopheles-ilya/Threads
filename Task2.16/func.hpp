#pragma once

#include <pthread.h>
#include <stdio.h>

#define EPS 1e-16

enum class io_status {
    undef,
    error_open,
    error_read,
    all_empty,
    no_stable,
    succes
};

struct Arg {
    int p = 0; // amount of threads
    int k = 0; // number of thread
    pthread_t tid = 0; // id of thread
    const char* name = nullptr; //name of file
    io_status error = io_status::undef;
    int error_flag = 0;
    int greater_than_stable = 0;
    double max_stable_elem = 0;
    int all_numbers = 0;
    int is_stable_exist = 0;
    int empty_files = 0;
};


void func_1(FILE* fp, Arg* a);
void func_2(FILE* fp, Arg* a);
void* thread_func(void* arg); 
void process_arg(Arg* a, int p);

