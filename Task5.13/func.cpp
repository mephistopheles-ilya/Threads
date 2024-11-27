#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <cmath>

#include "func.hpp"
#include "reduce_sum.hpp"

#define EPS 1e-16


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

    int end_of_beginning_step = 0;
    int fullfill_sequence = 0;
    bool fullfill_prev_step = false;
    if (segment_begin < n) {
        if(segment_begin >= 1) {
            a->before_segment_begin = array[segment_begin - 1];
        }
        if(segment_end <= (n - 1)) {
            a->after_segemnt_end = array[segment_end];
        }
        for(int i = segment_begin; i < segment_end; ++i) {
            if (i >= 1 && i < (n - 1)) {
                if(std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                    ++fullfill_sequence;
                    if (fullfill_prev_step == false) {
                        a->begin_of_ending = array[i - 1];
                        a->number_of_begin_of_ending = i - 1;
                    }
                    fullfill_prev_step = true;
                    if (i == segment_begin) {
                       a->sequence_in_beginning = true; 
                       a->end_of_beginning = array[i + 1];
                       a->number_of_end_of_beginning = i + 1;
                       end_of_beginning_step = i;
                    } else if (a->sequence_in_beginning == true 
                            && i == (end_of_beginning_step - 1)) {
                        a->end_of_beginning = array[i + 1];
                        a->number_of_end_of_beginning = i + 1;
                        end_of_beginning_step = i;
                    }
                    if (i == (segment_end - 1)) {
                        a->sequence_in_ending = true;
                    }
                } else {
                    fullfill_prev_step = false;
                }
            }
        }
        if ((segment_end - segment_begin) == fullfill_sequence) {
            a->one_big_sequence = true;
        }
    }
    if (segment_begin == (n - 1) && (segment_end - segment_begin) == 1) {
        a->one_big_sequence = true;
        a->sequence_in_beginning = true;
        a->sequence_in_ending = true;
        a->end_of_beginning = array[n - 1];
        if (n >= 2) {
            a->begin_of_ending = array[n - 2];
        } else {
            a->begin_of_ending = array[n - 1];
        }
    }
    if (segment_begin == 0 && (segment_end - segment_begin) == 1) {
        a->one_big_sequence = true;
        a->sequence_in_beginning = true;
        a->sequence_in_ending = true;
        a->end_of_beginning = array[1];
        a->begin_of_ending = array[0];
    }

    synchronize(p, a);

    int i = 0, j = 0, end_counter = 0, tmp = 0;
    double begin = 0, end = 0;
    if (segment_begin < n) {
        for(i = segment_begin; i < segment_end; ++i) {
            if (a -> one_big_sequence == true) {
                for(j = segment_begin; j < segment_end; ++j) {
                    array[j] = (a->begin_of_beginning + a->end_of_ending) * 0.5;
                    ++changed;
                }
                break;
            }
            if (a -> sequence_in_beginning == true) {
                for(j = segment_begin; j <= a->number_of_end_of_beginning; ++j) {
                    array[j] = (a->begin_of_beginning + a->end_of_beginning) * 0.5;
                    ++changed;
                }
                i = j + 1;
                a -> sequence_in_beginning = false;
                continue;
            }
            if (a -> sequence_in_ending == true && i == a->number_of_begin_of_ending) {
                for(j = i; j < segment_end; ++j) {
                    array[j] = (a -> begin_of_ending + a->end_of_ending) * 0.5;
                    ++changed;
                }
                break;
            }
            if (i >= 1 && i < (n - 1)) {
                tmp = i;
                j = i - 1;
                end_counter = -1;
                if(std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                    begin = array[i - 1];
                    end = array[i + 1];
                    while(i < (segment_end - 2)) {
                        ++i;
                        if(std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                            end = array[i + 1];
                            end_counter = i + 1;
                        } else {
                            break;
                        }
                    }
                }
                for(j = j; j < end_counter; ++j) {
                    array[j] = (begin + end) * 0.5;
                    ++changed;
                }
                if (end_counter == -1) {
                    i = (tmp + 1);
                } else {
                    i = end_counter;
                }   
            }
        }
    }

    a->changed = changed;

    reduce_sum(p, &(a->changed), 1);

    a->local_time = get_cpu_time() - local_time;

    return nullptr;
}
    


void synchronize(int p, Arg* a) {
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    if(p <= 1) return;
    pthread_mutex_lock(&m);
    ++t_in;
    if(t_in >= p) {
        t_out = 0;
        Arg* structs = a - (a -> k);
        int p = a->p;
        int i = 0, j = 0;
        for(i = 0; i < p; ++i) {
            if (structs[i].one_big_sequence == true || structs[i].sequence_in_ending == true) {
                j = i + 1;
                while(j < p && structs[j].one_big_sequence == true) {
                    j++;
                }
                if(j == p) {
                    structs[i].end_of_ending = structs[p - 1].end_of_beginning;
                }
                if (j < p) {
                    if(structs[j].sequence_in_beginning == true) {
                        structs[i].end_of_ending = structs[j].end_of_beginning;
                    } else {
                        if(structs[j - 1].one_big_sequence == true) {
                            structs[i].end_of_ending = structs[j - 1].end_of_beginning;
                        } else {
                            structs[i].end_of_beginning = structs[j - 1].after_segemnt_end;
                        }
                    }
                }
            }
            if(structs[i].one_big_sequence == true || structs[i].sequence_in_beginning == true) {
                j = i - 1;
                while(j >= 0 && structs[j].one_big_sequence == true) {
                    j--;
                }
                if(j == 0) {
                    structs[i].begin_of_beginning = structs[0].begin_of_beginning;
                }
                if (j > 0) {
                    if (structs[j].sequence_in_ending == true) {
                        structs[i].begin_of_beginning = structs[j].begin_of_ending;
                    } else {
                        if (structs[j + 1].one_big_sequence == true) {
                            structs[i].begin_of_beginning = structs[j + 1].begin_of_ending;
                        } else {
                            structs[i].end_of_beginning = structs[j + 1].before_segment_begin;
                        }
                    }
                }
            }
        }
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

