#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <stdio.h>

#include "func.hpp"

inline ulli step = 10;


static ulli number_count = 0;
static ulli prime_count = 0;
static ulli max_prime = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void* thread_func(void* args) {
    double local_time = get_cpu_time();
    
    Arg* a = (Arg*)args;
    ulli n = a->n;
    int p = a->p;
    int k = a->k;

    ulli begin = 0, end = 0;
    ulli prime_numbers_on_segment = 0;

    while(a->stop == false) {
        begin = number_count + k * step;
        end = begin + step;
        a->begin = begin;
        a->end = end;

        find_prime_numbers(begin, end, a);
        synchronize(p, a, number_count, prime_count, max_prime);

    }

    local_time = get_cpu_time() - local_time;
    a->local_time = local_time;
    return nullptr;
}

ulli find_prime_numbers(ulli begin, ulli end, Arg* a) {
    ulli count = 0;
    a->min_prime_number = end;
    a->max_prime_number = begin;
    ulli max_gap = 0;
    ulli prev_prime_number = 0;

    for(ulli i = begin; i < end; ++i) {
        if(is_prime_1(i) == true) {
            ++count;
            if (i < a->min_prime_number) {
                a->min_prime_number = i;
            }
            if (i > a->max_prime_number) {
                a->max_prime_number = i;
            }
            if (i - prev_prime_number >= max_gap) {
                max_gap = (prev_prime_number != 0) ? i - prev_prime_number : 0;
            }
            prev_prime_number = i;
        }
    }
    a->max_gap = max_gap;
    a->prime_numbers_on_segment = count;
    return count;
}


// prime numbers  6*k+1 or 6*k-1
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

void synchronize(int p, Arg* a, ulli& number_count, ulli& prime_count, ulli &max_prime) {
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    //if(p <= 1)  return;
    pthread_mutex_lock(&m);
    ++t_in;
    if(t_in >= p) {

        Arg* args = a - (a->k);
        ulli n = a->n;
        ulli max_gap = (max_prime > 0) ? (args[0].min_prime_number - max_prime) : 0;
        max_prime = args[p - 1].max_prime_number;
        int i = 0;
        for(i = 0; i < p; ++i) {
            prime_count += args[i].prime_numbers_on_segment; 
            if (prime_count >= n) {
                ulli count = prime_count - args[i].prime_numbers_on_segment;
                ulli prev_prime_number = 0;
                i = args[i].begin;
                while(count < n) {
                    if (is_prime_1(i) == true) {
                        ++count;
                        if (prev_prime_number > 0 && i - prev_prime_number > max_gap) {
                            max_gap = i - prev_prime_number;
                        }
                        prev_prime_number = i;
                    }
                    ++i;
                }
                for(int j = 0; j < p; ++j) {
                    args[j].stop = true;
                }
                break;
            }
            if (args[i].max_gap > max_gap) {
                max_gap = args[i].max_gap;
            }
            if (i < (p - 1) && (args[i + 1].min_prime_number - args[i].max_prime_number) > max_gap) {
                max_gap = args[i + 1].min_prime_number - args[i].max_prime_number;
            }
        }
        for(i = 0; i < p; ++i) {
            args[i].max_gap = max_gap;
        }
        number_count += p * step;

        pthread_cond_broadcast(&c_in);
    } else {
        while(t_in < p) {
            pthread_cond_wait(&c_in, &m);
        }
    }
    pthread_mutex_unlock(&m);
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
