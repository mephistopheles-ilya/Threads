#include <stdio.h>
#include <stdlib.h>
#include <new>

#include "func.hpp"


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage %s <p> <n>\n", argv[0]);
        return 1;
    }
    int p = 0;
    unsigned long long int n = 0;
    if (sscanf(argv[1], "%d", &p) != 1 || sscanf(argv[2], "%llu", &n) != 1) {
        printf("Usage %s <p> <n>\n", argv[0]);
        return 1;
    }
    if (p <= 0) {
        printf("At least one thread\n");
        return 1;
    }

    Arg* args = new (std::nothrow) Arg[p];
    Arg* copy = new (std::nothrow) Arg[p];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, p);
    if (args == nullptr) {
        printf("Not enough memmory\n");
        pthread_barrier_destroy(&barrier);
        return 2;
    }

    for(int i = 0; i < p; ++i) {
        args[i].k = i;
        args[i].p = p;
        args[i].n = n;
        args[i].barrier = &barrier;
        args[i].copy = copy;
    }

    double full_time = 0;

    full_time = get_full_time();

    for(int i = 1; i < p; ++i) {
        if (pthread_create(&(args[i].tid), nullptr, thread_func, args + i) != 0) {
            printf("Cannot create new thread\n");
            for(int l = 1; l < i; l++) {
                pthread_join(args[l].tid, nullptr);
            }
            delete[] args;
            pthread_barrier_destroy(&barrier);
            return 2;
        }
    }
    args[0].tid = pthread_self();
    thread_func(args + 0);
    for(int i = 1; i < p; ++i) {
        pthread_join(args[i].tid, nullptr);
    }

    full_time = get_full_time() - full_time;

    printf("Result = %llu\n", args[0].answer);

    printf("Full time : %lf\n", full_time);

    delete[] args;
    delete[] copy;
    pthread_barrier_destroy(&barrier);
    return 0;
}

