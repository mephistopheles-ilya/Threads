#include <stdio.h>
#include <stdlib.h>
#include <new>


#include "func.hpp"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage %s <number_of_threads> <number_of_elements_1> <number_of_elements_2> <file>\n", argv[0]);
        return 1;
    }
    int p = 0, n1 = 0, n2 = 0;
    FILE* f = nullptr;
    double* matrix = nullptr;
    Arg* args = nullptr;
    pthread_barrier_t barrier;
    if ((sscanf(argv[1], "%d", &p) != 1) || (sscanf(argv[2], "%d", &n1) != 1) || (sscanf(argv[3], "%d", &n2) != 1)) {
        printf("Usage %s <number_of_threads> <number_of_elements_1> <number_of_elements_2 <file>\n", argv[0]);
        return 2;
    }
    if (p <= 0 || n1 <= 0 || n2 <= 0) {
        printf("At least one thread and at least one number\n");
        return 3;
    }

    f = fopen(argv[4], "r");
    if (f == nullptr) {
        printf("Cannot open file\n");
        return 4;
    }

    matrix = new (std::nothrow) double[n1 * n2];
    args = new (std::nothrow) Arg[p];
    if (matrix == nullptr || args == nullptr) {
        printf("Not enough memmory\n");
        fclose(f);
        delete[] matrix;
        delete[] args;
        return 5;
    }

    io_status read_matrix = read_matrix_from_file(f, matrix, n1, n2);
    if (read_matrix == io_status::error_read) {
        printf("Cannot read elements from file\n");
        delete[] matrix;
        delete[] args;
        fclose(f);
        return 6;
    }
    if (read_matrix == io_status::not_enough_elements) {
        printf("Not enought elements in file\n");
        delete[] matrix;
        delete[] args;
        fclose(f);
        return 7;
    }

    pthread_barrier_init(&barrier, nullptr, p);
    for(int i = 0; i < p; ++i) {
        args[i].k = i;
        args[i].p = p;
        args[i].n1 = n1;
        args[i].n2 = n2;
        args[i].matrix = matrix;
        args[i].barrier = &barrier;
    }

    double global_time = get_full_time();
    for(int i = 1; i < p; ++i) {
        if (pthread_create(&(args[i].tid), nullptr, thread_func, args + i) != 0) {
            printf("Cannot create new thread\n");
            for(int l = 1; l < i; l++) {
                pthread_join(args[l].tid, nullptr);
            }
            delete[] matrix;
            delete[] args;
            pthread_barrier_destroy(&barrier);
            fclose(f);
            return 8;
        }
    }

    args[0].tid = pthread_self();
    args[0].barrier = &barrier;
    thread_func(args + 0);
    for(int i = 1; i < p; ++i) {
        pthread_join(args[i].tid, nullptr);
    }
    global_time = get_full_time() - global_time;


    if (args[0].error == 0) {
        printf("RESULT %2d:\n", p);
        for(int i = 0; i < n1; ++i) {
            for (int j = 0; i < n2; ++j) {
                printf(" %8.2e", matrix[i]);
            }
            printf("\n");
        }
        printf("\n");

        printf("Changed elements : %2d\n", args[0].changed);
        printf("All time = %lf\n", global_time);
        for(int i = 0; i < p; ++i) {
            printf("Time for thread number %d = %lf\n", i, args[i].local_time);
        }
    }


    fclose(f);
    delete[] matrix;
    delete[] args;
    pthread_barrier_destroy(&barrier);
    return 0;
}


