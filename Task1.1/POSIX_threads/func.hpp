#pragma once

#include <pthread.h>
#include <stdio.h>

enum class io_status {
    undef,
    error_open,
    error_read,
    succes
};

struct Arg {
    int p = 0; // amount of threads
    int k = 0; // number of thread
    pthread_t tid = 0; // id of thread
    const char* name = nullptr; //name of file
    io_status error = io_status::undef;
    int res = 0;
};


void func_1(FILE* fp, Arg* a);
void* thread_func(void* arg); 
void process_arg(Arg* a, int& res, bool& flag, int p);

