#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <stdio.h>
#include <algorithm>

#include "func.hpp"

const ulli step = 50000;

static ulli number_count = 0;
static ulli prime_count = 0;
static ulli max_gap = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void* thread_func(void* args) {
    double local_time = get_cpu_time();
    
    Arg* a = (Arg*)args;
    ulli n = a->n;

    ulli begin = 0, end = 0;
    ulli prime_numbers_on_segment = 0;
    ulli local_prime_count = 0;
    ulli locla_max_gap = 0;
    ulli max_gap_on_segment = 0;
    ulli copy_prime_count = 0;
    ulli prev_prime_number = 0;

    while(true) {
        pthread_mutex_lock(&mutex);

        prime_count += prime_numbers_on_segment;
        if(prime_count <= n) {
            local_prime_count = prime_count;
            begin = number_count;
            number_count += step;
            end = number_count;
            max_gap = (max_gap > max_gap_on_segment) ? max_gap : max_gap_on_segment;
            locla_max_gap = max_gap;
        }
        copy_prime_count = prime_count;
        
        pthread_mutex_unlock(&mutex);

        if (copy_prime_count >= n) break;
        prime_numbers_on_segment = 0;
        max_gap_on_segment = 0;
        prev_prime_number = 0;
        for(ulli i = begin; i < end; ++i) {
            if(is_prime_1(i) == true) {
                ++prime_numbers_on_segment;
                if (i - prev_prime_number > max_gap_on_segment && prev_prime_number > 0) {
                    max_gap_on_segment = i - prev_prime_number;
                }
                prev_prime_number = i;
            }
        }
    }

    a->begin = begin;
    a->end = end;
    a->prime_numbers_on_segment = prime_numbers_on_segment;
    a->local_prime_count = local_prime_count;
    a->local_max_gap = locla_max_gap;
    a->max_gap_on_segment = max_gap_on_segment;

#if 0
    pthread_barrier_wait(a->barrier);

    pthread_mutex_lock(&mutex);
    if (a->k == 0) {
        std::copy(a, a + (a->p), a->copy);
    }
    //printf("%llu %llu\n", a->max_gap_on_segment, a->local_max_gap);
    pthread_mutex_unlock(&mutex);

    if (a->k == 0) {
        Arg* copy = a->copy;
        std::sort(copy, copy + (a->p), [](const Arg& a, const Arg& b) { return a.begin < b.begin;});
        //ulli must_begin_with = copy[0].begin;
        max_gap = copy[0].local_max_gap;
        prime_count = copy[0].local_prime_count;

        prev_prime_number = 0;
        for(ulli j = copy[0].begin; j < copy[a->p-1].end && prime_count < n; ++j) {
            if(is_prime_1(j) == true) {
                ++prime_count;
                if (j - prev_prime_number > max_gap && prev_prime_number > 0) {
                    max_gap = j - prev_prime_number;
                }
                prev_prime_number = j;
            }
        }
        for(int i = 0; i < a->p; ++i) {
            if(copy[i].begin != must_begin_with) {
                prev_prime_number = 0;
                for(ulli j = copy[i].begin; j < copy[i].end && prime_count < n; ++j) {
                    if(is_prime_1(j) == true) {
                        ++prime_count;
                        if (j - prev_prime_number > max_gap && prev_prime_number > 0) {
                            max_gap = j - prev_prime_number;
                        }
                        prev_prime_number = j;
                    }
                }
                if(prime_count >= n) {
                    break;
                }
                must_begin_with += step;
                continue;
            }
            prime_count += copy[i].local_prime_count;
            if (prime_count > n) {
                prev_prime_number = 0;
                prime_count -= copy[i].local_prime_count;
                ulli j = copy[i].begin;
                while(prime_count < n) {
                    if(is_prime_1(j) == true) {
                        ++prime_count;
                        if (j - prev_prime_number > max_gap && prev_prime_number > 0) {
                            max_gap = j - prev_prime_number;
                        }
                        prev_prime_number = j;
                        ++j;
                    }
                }
                break;
            }
            max_gap = (max_gap > copy[i].max_gap_on_segment) ? max_gap : copy[i].max_gap_on_segment;
            max_gap = (max_gap > copy[i].local_max_gap) ? max_gap : copy[i].local_max_gap;
            must_begin_with += step;
        }
    }

    pthread_barrier_wait(a->barrier);

    a->answer = max_gap;
    local_time = get_cpu_time() - local_time;
    a->local_time = local_time;

#endif
    a->local_time = local_time;
    return nullptr;
}




bool is_prime_1(ulli number) {
    if ((number == 2) || (number == 3)) {
        return true;
    }
    if ((number < 2) || (number % 2 == 0) || (number % 3 == 0)) {
        return false;
    }
    for(ulli j = 5; j * j <= number; j += 6) {
        if ((number % j == 0) || (number % (j + 2) == 0)) {
            return false;
        }
    }
    return true;
}

bool is_prime_2(ulli number) {
    if (number == 2) {
        return true;
    }
    if ((number < 2) || (number % 2 == 0)) {
        return false;
    }
    for(ulli j = 3; j * j <= number; j += 2) {
        if (number % j == 0) {
            return false;
        }
    }
    return true;
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
