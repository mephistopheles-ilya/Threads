#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "func.hpp"


int main(int argc, char* argv[]) {
    int p = argc - 1, k = 0, res = 0;
    bool flag = false;
    Arg* a = nullptr;
    if (argc == 1) {
        printf("Usage %s <files>\n", argv[0]);
        return 1;
    }
    a = new Arg[p];
    if(a == nullptr) {
        printf("Not enough memmory\n");
        return 2;
    }
    for(k = 0; k < p; ++k) {
        a[k].k = k;
        a[k].p = p;
        a[k].name = argv[k + 1];
    }
    for(k = 1; k < p; ++k) {
        if (pthread_create(&a[k].tid, nullptr, thread_func, a + k) != 0) {
            printf("Can not create new thread number %d\n", k);
            for(int l = 1; l < k; l++) {
                pthread_join(a[l].tid, nullptr);
            }
            delete[] a;
            return 3;
        }
    }
    a[0].tid = pthread_self();
    thread_func(a + 0);
    for(k = 1; k < p; ++k) {
        pthread_join(a[k].tid, nullptr);
    }
    process_arg(a, res, flag, p);
    if(flag == false) {
        printf("Result = %d\n", res);
    }
    delete[] a;
    return 0;
}



