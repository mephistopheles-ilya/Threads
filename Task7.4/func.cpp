#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

#include "func.hpp"

//inline ulli step = 750000;
//inline ulli step = 100000;
inline ulli step = 80000;


static ulli number_count = 0;
static ulli prime_count = 0;
static ulli max_prime = 0;
static ulli full_max_gap = 0;



void* thread_func(void* args) {
    double local_time = get_cpu_time();
    
    Arg* a = (Arg*)args;
    int p = a->p;
    int k = a->k;

    ulli begin = 0, end = 0;

    while(a->stop == false) {
        begin = number_count + k * step;
        end = begin + step;
        a->begin = begin;
        a->end = end;

        find_prime_numbers(begin, end, a);
        synchronize(p, a);

    }

    local_time = get_cpu_time() - local_time;
    a->local_time = local_time;
    return nullptr;
}

ulli find_prime_numbers(ulli begin, ulli end, Arg* a) {
    ulli count = 0;
    ulli min_prime_number = end;
    ulli max_prime_number = begin;
    ulli max_gap = 0;
    ulli prev_prime_number = 0;

    for(ulli i = begin; i < end; ++i) {
        if(is_prime_1(i) == true) {
            ++count;
            if (i < min_prime_number) {
                min_prime_number = i;
            }
            if (i > max_prime_number) {
                max_prime_number = i;
            }
            if (i - prev_prime_number > max_gap && prev_prime_number > 0) {
                max_gap = i - prev_prime_number;
            }
            prev_prime_number = i;
        }
    }
    a->max_gap = (a->max_gap > max_gap) ? a->max_gap : max_gap;
    a->prime_numbers_on_segment = count;
    a->max_prime_number = max_prime_number;
    a->min_prime_number = min_prime_number;
    return count;
}


bool is_prime_2(ulli number) {
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

bool is_prime_1(ulli number) {
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

void synchronize(int p, Arg* a) {
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    pthread_mutex_lock(&m);
    ++t_in;
    if(t_in >= p) {
        t_out = 0;

        Arg* args = a - (a->k);
        ulli n = a->n;
        ulli max_gap = (max_prime > 0) ? (args[0].min_prime_number - max_prime) : 0;
        max_prime = args[p - 1].max_prime_number;
        for(int i = 0; i < p; ++i) {
            prime_count += args[i].prime_numbers_on_segment; 
            if (prime_count > n) {
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
            } else if (prime_count == n) {
                for(int j = 0; j < p; ++j) {
                    args[j].stop = true;
                }
                break;
            }
            if (args[i].max_gap > max_gap) {
                max_gap = args[i].max_gap;
            }
            if (i >= 1 && (args[i].min_prime_number - args[i - 1].max_prime_number) > max_gap) {
                max_gap = args[i].min_prime_number - args[i - 1].max_prime_number;
            }
        }
        if (max_gap > full_max_gap) {
            full_max_gap = max_gap;
        }
        for(int i = 0; i < p; ++i) {
            args[i].max_gap = full_max_gap;
        }
        number_count += p * step;

        pthread_cond_broadcast(&c_in);
    } else {
        while(t_in < p) {
            pthread_cond_wait(&c_in, &m);
        }
    }
    ++t_out;
    if(t_out >= p) {
        t_in = 0;
        pthread_cond_broadcast(&c_out);
    } else {
        while (t_out < p) {
            pthread_cond_wait(&c_out, &m);
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
