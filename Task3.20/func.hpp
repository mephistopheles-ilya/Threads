#pragma once

#include <pthread.h>
#include <stdio.h>

#define EPS 1e-16

enum class io_status {
    undef,
    error_open,
    error_read,
    all_empty,
    no_fib,
    succes
};

struct Arg {
    double first = 0;
    double second = 0;

    int p = 0; 
    int k = 0; 
    pthread_t tid = 0;
    const char* name = nullptr;
    io_status error = io_status::undef;

    double local_fib_max = 0;
    int is_local_fib_exist = 0;
    double all_fib_max = 0;
    int is_all_fib_exist = 0;


    int elements_in_file = 0;
    int all_elements = 0;
    int error_flag = 0;
    int greater_than_fib = 0;


    double before_last = 0;
    double last = 0;
};


void func_1(FILE* fp, Arg* a);
void func_2(FILE* fp, Arg* a);
void* thread_func(void* arg); 
void process_arg(Arg* a, int p);
void synchronize(int p, Arg* a);
int find_next_elements(Arg* arr_ptr, int j, int p, double& num2, double& num3);
void update_max_fib(double& all_fib_max, int& is_all_fib_exist, double num1, double num2, double num3);
