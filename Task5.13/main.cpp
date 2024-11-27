#include <stdio.h>
#include <stdlib.h>
#include <new>


#include "func.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage %s <number_of_threads> <number_of_elements> <file>\n", argv[0]);
        return 1;
    }
    int p = 0, n = 0;
    FILE* f = nullptr;
    double* array = nullptr;
    Arg* args = nullptr;
    if ((sscanf(argv[1], "%d", &p) != 1) || (sscanf(argv[2], "%d", &n) != 1)) {
        printf("Usage %s <number_of_threads> <number_of_elements> <file>\n", argv[0]);
        return 2;
    }
    if (p <= 0 || n <= 0) {
        printf("At least one thread and at least one number\n");
        return 3;
    }

    f = fopen(argv[3], "r");
    if (f == nullptr) {
        printf("Cannot open file\n");
        return 4;
    }

    array = new (std::nothrow) double[n];
    args = new (std::nothrow) Arg[p];
    if (array == nullptr || args == nullptr) {
        printf("Not enough memmory\n");
        fclose(f);
        delete[] array;
        delete[] args;
        return 5;
    }

    io_status read_array = read_array_from_file(f, array, n);
    if (read_array == io_status::error_read) {
        printf("Cannot read elements from file\n");
        delete[] array;
        delete[] args;
        fclose(f);
        return 6;
    }
    if (read_array == io_status::not_enough_elements) {
        printf("Not enought elements in file\n");
        delete[] array;
        delete[] args;
        fclose(f);
        return 7;
    }

    for(int i = 0; i < p; ++i) {
        args[i].k = i;
        args[i].p = p;
        args[i].n = n;
        args[i].array = array;
    }

    double global_time = get_full_time();
    for(int i = 1; i < p; ++i) {
        if (pthread_create(&(args[i].tid), nullptr, thread_func, args + i) != 0) {
            printf("Cannot create new thread\n");
            for(int l = 1; l < i; l++) {
                pthread_join(args[l].tid, nullptr);
            }
            delete[] array;
            delete[] args;
            fclose(f);
            return 8;
        }
    }

    args[0].tid = pthread_self();
    thread_func(args + 0);
    for(int i = 1; i < p; ++i) {
        pthread_join(args[i].tid, nullptr);
    }
    global_time = get_full_time() - global_time;

#if 0
    printf("RESULT %2d:", p);
    for(int i = 0; i < n; ++i) {
        printf(" %8.2e", array[i]);
    }
    printf("\n");

    printf("Changed elements : %2d\n", args[0].changed);
    printf("All time = %lf\n", global_time);
    for(int i = 0; i < p; ++i) {
        printf("Time for thread number %d = %lf\n", i, args[i].local_time);
    }
#endif

#if 1
    printf("RESULT :");
    for(int i = 0; i < n; ++i) {
        printf(" %3.2lf", array[i]);
    }
    printf("\n");
    printf("Changed elements : %2d\n", args[0].changed);
#endif

    fclose(f);
    delete[] array;
    delete[] args;
    return 0;
}


