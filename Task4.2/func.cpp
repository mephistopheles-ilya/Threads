#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

#include "func.hpp"
#include "reduce_sum.hpp"


io_status read_array_from_file(FILE *f, double* array, int n) {
    int i = 0, res = 0;
    double tmp = 0;
    for(;i < n; ++i) {
        res = fscanf(f, "%lf", &tmp);
        switch (res) {
            case 1:
                array[i] = tmp;
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
    int n = a->n;
    double *array = a->array;
    int changed = 0;
    int segment_size = n / p;
    double remember1 = 0, remember2 = 0;

    int segment_begin = 0;
    int segment_end = 0;
    if (segment_size == 0) {
        segment_begin = k;
        segment_end = k + 1;
    } else {
        segment_begin = k * segment_size;
        segment_end = segment_begin + segment_size;
        if (k == (p - 1) && (n - segment_end != 0)) {
            segment_end = n;
        }
    }

    if (segment_begin >= 2 && segment_begin < n) {
        remember1 = *(array + segment_begin - 1);
        remember2 = *(array + segment_begin - 2);
    } else if (segment_begin >=1 && segment_begin < n) { 
        remember1 = *(array + segment_begin - 1);
    }

    pthread_barrier_wait(a->barrier);

    for(int i = segment_end - 1; i >= (segment_begin + 2); --i) {
        array[i] = (array[i - 1] + array[i - 2]) / 2;
        ++changed;
    }

    if (segment_begin < n) {
        if (segment_begin >= 2 && (segment_end - segment_begin >= 2)) {
            array[segment_begin + 1] = (remember1 + array[segment_begin]) / 2;
            ++changed;
            array[segment_begin] = (remember1 + remember2) / 2.;
            ++changed;
        } else if (segment_begin >=2 && (segment_end - segment_begin >= 1)) {
            array[segment_begin] = (remember1 + remember2) / 2;
            ++changed;
        } else if (segment_begin >=1 && (segment_end - segment_begin >=2)) {
            array[segment_begin + 1] = (remember1 + array[segment_begin]) / 2;
            ++changed;
        }
    }


    a->changed = changed;

    reduce_sum(p, &(a->changed), 1);

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
