#pragma once

#include <stdio.h>
#include <vector>

enum class io_status {
    undef,
    error_open,
    error_read,
    succes
};

struct alignas(64) Arg {
    int p = 0; // amount of threads
    int k = 0; // number of thread
    const char* name = nullptr; //name of file
    io_status error = io_status::undef;
    int res = 0;
    Arg(int p, int k, const char* name, io_status error = io_status::undef, int res = 0):
        p(p), k(k), name(name), error(error), res(res) {}
};


void func_1(FILE* fp, Arg& a);
void thread_func(Arg& arg); 
void process_arg(std::vector<Arg>& a, int& res, bool& flag);

