#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <new>
#include <string.h>

#include "func.hpp"
#include "reduce_sum.hpp"


io_status read_matrix_from_file(FILE *f, double* matrix, int n1, int n2) {
    int i = 0, res = 0;
    double tmp = 0;
    int n = n1 * n2;
    for(;i < n; ++i) {
        res = fscanf(f, "%lf", &tmp);
        switch (res) {
            case 1:
                matrix[i] = tmp;
                break;
            case 0:
                return io_status::error_read;
                break;
            case EOF:
                return io_status::not_enough_elements;
                break;
            default:
                break;
        }
    }
    return io_status::succes;
}

void* thread_func(void* arg) {
    double local_time = get_cpu_time();

    Arg* a = (Arg *)arg;
    int k = a->k;
    int p = a->p;
    int n1 = a->n1;
    int n2 = a->n2;
    double *matrix = a->matrix;
    int changed = 0;
    int error = 0;
    int i = 0, j = 0;
    double* array1 = new (std::nothrow) double[n2];
    double* array2 = new (std::nothrow) double[n2];
    if (array1 == nullptr || array2 == nullptr) {
        error = 1;
    }

    reduce_sum(p, &error, 1);

    if(error > 0) {
        if (k == 0) {
            printf("Not enough memmory\n");
        }
        delete[] array1;
        delete[] array2;
        return nullptr;
    }

    int segment_size = n1 / p;
    int segment_begin = 0;
    int segment_end = 0;
    if (segment_size == 0) {
        segment_begin = k;
        segment_end = k + 1;
    } else {
        segment_begin = k * segment_size;
        segment_end = segment_begin + segment_size;
        if (k == (p - 1) && (n1 - segment_end != 0)) {
            segment_end = n1;
        }
    }

    double min_elem = matrix[0];
    if (segment_begin < n1) {
        for(int i = segment_begin; i < segment_end; ++i) {
            for(int j = 0; j < n2; ++j) {
                if (matrix[i * n2 + j] < min_elem) {
                    min_elem = matrix[i * n2 + j];
                }
            }
        }

        if (segment_end < n1) {
            memcpy(array2, matrix + segment_end * n2, n2 * sizeof(double));
        } else {
            memcpy(array2, matrix + (n1 - 1) * n2, n2 * sizeof(double));
        }
        if (segment_begin > 1) {
            memcpy(array1, matrix + (segment_begin - 1) * n2, n2 * sizeof(double));
        } else {
            memcpy(array1, matrix, n2 * sizeof(double));
        }
    }

    reduce_min(p, &min_elem, 1);

    if (segment_begin < n1) {
        if (segment_begin != 0 && (segment_end - segment_begin > 1)) {
            i = segment_begin;
            array1[0] = matrix[i * n2 + 0];
            for(j = 1; j < (n2 - 1); ++j) {
                if (matrix[i * n2 + j] <= array1[j - 1] && matrix[i * n2 + j] <= matrix[i * n2 + (j + 1)]
                        && matrix[i * n2 + j] <= array1[j] && matrix[i * n2 + j] <= matrix[(i + 1) * n2 + j]) {
                    array1[j] = matrix[i * n2 + j];
                    matrix[i * n2 + j] = min_elem;
                    ++changed;
                } else {
                    array1[j] = matrix[i * n2 + j];
                }
            }
            ++i;
        } else {
            if (segment_begin == 0) {
                i = segment_begin + 1;
            } else {
                i = segment_begin;
            }
        }

        for(; i < (segment_end - 1); ++i) {
            array1[0] = matrix[i * n2 + 0];
            for(j = 1; j < (n2 - 1); ++j) {
                if (matrix[i * n2 + j] <= array1[j - 1] && matrix[i * n2 + j] <= matrix[i * n2 + (j + 1)]
                        && matrix[i * n2 + j] <= array1[j] && matrix[i * n2 + j] <= matrix[(i + 1) * n2 + j]) {
                    array1[j] = matrix[i * n2 + j];
                    matrix[i * n2 + j] = min_elem;
                    ++changed;
                } else {
                    array1[j] = matrix[i * n2 + j];
                }
            }
        }

        if (segment_end != n1 && i < segment_end) {
            array1[0] = matrix[i * n2 + 0];
            for(j = 1; j < (n2 - 1); ++j) {
                if (matrix[i * n2 + j] <= array1[j - 1] && matrix[i * n2 + j] <= matrix[i * n2 + (j + 1)]
                        && matrix[i * n2 + j] <= array1[j] && matrix[i * n2 + j] <= array2[j]) {
                    array1[j] = matrix[i * n2 + j];
                    matrix[i * n2 + j] = min_elem;
                    ++changed;
                } else {
                    array1[j] = matrix[i * n2 + j];
                }
            }
        }
    }

    a->changed = changed;

    reduce_sum(p, &(a->changed), 1);

    delete[] array1;
    delete[] array2;

    a->local_time = get_cpu_time() - local_time;

    return nullptr;
}
    

double get_cpu_time() {
    struct rusage buf;
    getrusage(RUSAGE_THREAD, &buf);
    return (double)(buf.ru_utime.tv_sec) + (double)(buf.ru_utime.tv_usec)/1000000.;
}
double get_full_time() {
    struct timeval buf;
    gettimeofday(&buf, 0);
    return (double)(buf.tv_sec) + (double)(buf.tv_usec)/1000000.;
}
