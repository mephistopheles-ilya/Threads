#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <threads.h>
#include <mutex>
#include <algorithm>

#include "func.hpp"
#include "reduce_sum.hpp"

#define EPS 1e-16

std::mutex mut;


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
    a->segment_begin = segment_begin;
    a->segment_end = segment_end;

    int end_of_beginning_step = 0;
    int fullfill_sequence = 0;
    bool fullfill_prev_step = false;
    if (segment_begin < n) { // extra threads will do nothing
        if (segment_begin >= 1 && segment_end <= (n - 1)) { // main segments in the midddle
            for(int i = segment_begin; i < segment_end; ++i) {
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
                            && i == (end_of_beginning_step + 1)) {
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
            if ((segment_end - segment_begin) == fullfill_sequence && fullfill_sequence > 0) {
                a->one_big_sequence = true;
            }
            end_of_beginning_step = 0;
            fullfill_sequence = 0;
            fullfill_prev_step = 0;
        }
        if (segment_begin == 0 && segment_end != n) {
            for(int i = segment_begin + 1; i < segment_end; ++i) {
                if (std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                    ++fullfill_sequence;
                    if(fullfill_prev_step == false) {
                        a-> begin_of_ending = array[i - 1];
                        a->number_of_begin_of_ending = i - 1;
                    }
                    fullfill_prev_step = true;
                    if(i == (segment_end - 1)) {
                        a->sequence_in_ending = true;
                    }
                } else {
                    fullfill_prev_step = false;
                }
            }
            if ((segment_end - segment_begin - 1) == fullfill_sequence && fullfill_sequence > 0) {
                a->one_big_sequence = true;
            }
            end_of_beginning_step = 0;
            fullfill_sequence = 0;
            fullfill_prev_step = 0;
        }
        if (segment_end == n && segment_begin != 0) {
            for(int i = segment_begin; i < (segment_end - 1); ++i) {
                if (std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                    ++fullfill_sequence;
                     if (i == segment_begin) {
                       a->sequence_in_beginning = true; 
                       a->end_of_beginning = array[i + 1];
                       a->number_of_end_of_beginning = i + 1;
                       end_of_beginning_step = i;
                    } else if (a->sequence_in_beginning == true 
                            && i == (end_of_beginning_step + 1)) {
                        a->end_of_beginning = array[i + 1];
                        a->number_of_end_of_beginning = i + 1;
                        end_of_beginning_step = i;
                    }
                 }
            }
            if ((segment_end - segment_begin - 1) == fullfill_sequence && fullfill_sequence > 0) {
                a->one_big_sequence = true;
            }
            end_of_beginning_step = 0;
            fullfill_sequence = 0;
            fullfill_prev_step = 0;
        }
        if (segment_begin == 0 && (segment_end - segment_begin == n)) {
            for(int i = segment_begin + 1; i < (segment_end - 1); ++i) {
                if (std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                    ++fullfill_sequence;
                }
            }
            if ((segment_end - segment_begin - 2) == fullfill_sequence && fullfill_sequence > 0) {
                a->one_big_sequence = true;
                a->end_of_beginning = array[segment_end - 1];
                a->number_of_end_of_beginning = segment_end - 1;
                a->begin_of_ending = array[0];
                a->number_of_begin_of_ending = 0;
            }
            end_of_beginning_step = 0;
            fullfill_sequence = 0;
            fullfill_prev_step = 0;
        }
        if(segment_begin == 0 && (segment_end - segment_begin) == 1) {
            if(n>=3) {
                int i = segment_begin + 1;
                if(std::fabs(array[i] - 0.5 * (array[i + 1] + array[i - 1])) < EPS) {
                    a->one_big_sequence = true;
                    a->begin_of_ending = array[0];
                    a->number_of_begin_of_ending = 0;
                }
            }
        }
        if(segment_begin == (n - 1) && (segment_end - segment_begin) == 1) {
            if(n>=3) {
                int i = segment_begin - 1;
                if(std::fabs(array[i] - 0.5 * (array[i + 1] + array[i - 1])) < EPS) {
                    a->one_big_sequence = true;
                    a->end_of_beginning = array[n - 1];
                    a->number_of_end_of_beginning = n - 1;
                }
            }
        }
   
        if (segment_begin >= 1) {
            a->before_segment_begin = array[segment_begin - 1];
        }
        if (segment_end <= (n - 1)) {
            a->after_segemnt_end = array[segment_end];
        }
    }

    synchronize(p, a);

    //a->print_structure();
    //pthread_barrier_wait(a->barrier);

    int i = 0, j = 0, end_counter = 0, tmp = 0, number_of_last_changed = -1;
    double begin = 0, end = 0, remember_changed_element = 0;
    bool less_eps = false, flag = false;
    if (segment_begin < n) {
        for(i = segment_begin; i < segment_end; ++i) {
            if (a -> one_big_sequence == true) {
                for(j = segment_begin; j < segment_end; ++j) {
                    array[j] = (a->begin_of_beginning + a->end_of_ending) * 0.5;
                    ++changed;
                }
                break;
            }
            if (a -> sequence_in_beginning == true && flag == false) {
                //if (a -> end_of_segment_before == true) {
                //    j = segment_begin + 1;
                //} else {
                //    j = segment_begin;
                //}
                j = segment_begin;
                number_of_last_changed = a->number_of_end_of_beginning;
                remember_changed_element = array[a->number_of_end_of_beginning];
                i = a->number_of_end_of_beginning;
                for(; j <= a->number_of_end_of_beginning; ++j) {
                    array[j] = (a->begin_of_beginning + a->end_of_beginning) * 0.5;
                    ++changed;
                }
                flag = true;
                continue;
            }
            if (a -> sequence_in_ending == true && (i == a->number_of_begin_of_ending 
                    || (i == a->number_of_begin_of_ending + 1 && a->sequence_in_beginning == true && 
                    a->number_of_end_of_beginning==a->number_of_begin_of_ending)
                    || (i == a->number_of_begin_of_ending + 1 && a->number_of_begin_of_ending == number_of_last_changed))){ 
                if (a->end_of_segment_before == true && i == segment_begin) {
                    array[segment_begin] = a->new_prev_val;
                    ++changed;
                    j = i + 1;
                } else {
                    j = i;
                }
                for(; j < segment_end; ++j) {
                    array[j] = (a -> begin_of_ending + a->end_of_ending) * 0.5;
                    ++changed;
                }
                break;
            }
            if (a ->end_of_segment_before == true && i == segment_begin) {
                remember_changed_element = array[segment_begin];
                array[segment_begin] = a->new_prev_val;
                ++changed;
                number_of_last_changed = segment_begin;
                continue;
            }



            if (i == (segment_end - 1) && a -> begin_of_segment_next == true &&
                    (a->end_of_segment_before != true || (segment_end - segment_begin >= 2))) {
                array[segment_end - 1] = a->new_next_val;
                ++changed;
                break;
            }

            if (i >= 1 && i < (n - 1)) {
                //std::lock_guard<std::mutex> lk(mut);
                tmp = i;
                if (i - 1 < segment_begin) {
                    j = i;
                } else {
                    j = i - 1;
                }
                end_counter = -1;
                int elem_i_1 = 0;
                if (i + 1 >= segment_end) {
                    elem_i_1 = a->after_segemnt_end;
                } else {
                    elem_i_1 = array[i + 1];
                }
                if (number_of_last_changed > 0 && (i - 1 == number_of_last_changed)) {
                    less_eps = std::fabs(array[i] - 0.5 * (remember_changed_element + elem_i_1)) < EPS;
                } else {
                    if (i - 1 < segment_begin) {
                        less_eps = std::fabs(array[i] - 0.5 * (a->before_segment_begin + elem_i_1)) < EPS;
                    } else {
                        less_eps = std::fabs(array[i] - 0.5 * (array[i - 1] + elem_i_1)) < EPS;
                    }
                }
                if(less_eps) {
                    if (number_of_last_changed > 0 && (i - 1 == number_of_last_changed)) {
                        begin = remember_changed_element;
                    } else {
                        begin = array[i - 1];
                    }
                    if (i + 1 >= segment_end) {
                        end = a->after_segemnt_end;
                        end_counter = i;
                    } else {
                        end = array[i + 1];
                        end_counter = i + 1;
                    }
                    while(i < (segment_end - 2)) { // because of increment on nex line
                        ++i;
                        if(std::fabs(array[i] - 0.5 * (array[i - 1] + array[i + 1])) < EPS) {
                            end = array[i + 1];
                            end_counter = i + 1;
                        } else {
                            break;
                        }
                    }
                }
                if (number_of_last_changed > 0) {
                    ++j;
                }
                if (a->end_of_segment_before == true && j == segment_begin) {
                    ++j;
                    array[segment_begin] = a->new_prev_val;
                    ++changed;
                } else if (a->end_of_segment_before == true && (segment_end - segment_begin == 1)) {
                    array[segment_begin] = a->new_prev_val;
                    ++changed;
                }
                if (end_counter == -1) {
                    i = tmp; // in last cycyle will be extra plus
                    number_of_last_changed = -1;
                } else {
                    i = end_counter; // in cycyle will be plus extra one
                    number_of_last_changed = end_counter;
                    remember_changed_element = array[end_counter];
                }   
                for(j = j; j <= end_counter; ++j) {
                    array[j] = (begin + end) * 0.5;
                    ++changed;
                }

            }
        }
    }

    a->changed = changed;

    //pthread_barrier_wait(a->barrier);

    //a->print_structure();

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
    if(p <= 1) {
        if (a->one_big_sequence == true) {
            a->end_of_ending = a->end_of_beginning;
            a->begin_of_beginning = a->begin_of_ending;
        }
        return;
    }
    pthread_mutex_lock(&m);
    ++t_in;
    if(t_in >= p) {
        t_out = 0;
        Arg* structs = a - (a -> k);
        int p = a->p;
        int n = a->n;
        p = (p <= n) ? p : n;
        int i = 0, j = 0;
        for(i = 1; i < p; ++i) {
            if (structs[i - 1].one_big_sequence == true || structs[i - 1].sequence_in_ending == true) {
                structs[i].end_of_segment_before = true;
            }
            if (structs[i].one_big_sequence==true || structs[i].sequence_in_beginning==true) {
                structs[i].end_of_segment_before = false;
            }
        }
        for(i = 0; i < (p - 1); ++i) {
            if (structs[i + 1].one_big_sequence == true || structs[i + 1].sequence_in_beginning == true) {
                structs[i].begin_of_segment_next = true;
            }
            if (structs[i].one_big_sequence == true || structs[i].sequence_in_ending==true) {
                structs[i].begin_of_segment_next = false;
            }
        }
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
                        //if(structs[j - 1].one_big_sequence == true) {
                        //    structs[i].end_of_ending = structs[j - 1].end_of_beginning;
                        //} else {
                            structs[i].end_of_ending = structs[j - 1].after_segemnt_end;
                        //}
                    }
                }
            }
            if(structs[i].one_big_sequence == true || structs[i].sequence_in_beginning == true) {
                j = i - 1;
                while(j >= 0 && structs[j].one_big_sequence == true) {
                    j--;
                }
                if(j == -1) {
                    structs[i].begin_of_beginning = structs[0].begin_of_ending;
                }
                if (j >= 0) {
                    if (structs[j].sequence_in_ending == true) {
                        structs[i].begin_of_beginning = structs[j].begin_of_ending;
                    } else {
                        //if (structs[j + 1].one_big_sequence == true) {
                        //    structs[i].begin_of_beginning = structs[j + 1].begin_of_ending;
                        //} else {
                            structs[i].begin_of_beginning = structs[j + 1].before_segment_begin;
                        //}
                    }
                }
            }
        }
        for(i = 0; i < p; ++i) {
            if (structs[i].end_of_segment_before == true) {
               if( structs[i - 1].one_big_sequence == true) {
                   structs[i].new_prev_val = 0.5 * (structs[i - 1].begin_of_beginning + structs[i - 1].end_of_ending);
               } else if (structs[i - 1].sequence_in_ending == true) {
                   structs[i].new_prev_val = 0.5 * (structs[i - 1].begin_of_ending + structs[i - 1].end_of_ending);
               }
            }
        }
        for(i = 0; i < p; ++i) {
            if (structs[i].begin_of_segment_next == true) {
                if (structs[i + 1].one_big_sequence == true) {
                    structs[i].new_next_val = 0.5 * (structs[i + 1].begin_of_beginning + structs[i + 1].end_of_ending);
                } else if (structs[i + 1].sequence_in_beginning == true) {
                    structs[i].new_next_val = 0.5 * (structs[i + 1].begin_of_beginning + structs[i + 1].end_of_beginning);
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

void Arg::print_structure() {
    std::lock_guard<std::mutex> lk(mut);
    std::cout << std::boolalpha;
    std::cout << "Arrary : " << std::endl;
    for(int i = 0; i < n; ++i) {
        std::cout << array[i] << ' ';
    }
    std::cout << std::endl;
    std::cout << "segment_begin = " << segment_begin << std::endl;
    std::cout << "segment_end = " << segment_end << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "tid = " << tid << std::endl;
    std::cout << "p = " << p << std::endl;
    std::cout << "n = " << n << std::endl;
    std::cout << "changed = " << changed << std::endl;
    std::cout << "sequence_in_beginning = " << sequence_in_beginning << std::endl;
    std::cout << "sequence_in_ending = " << sequence_in_ending << std::endl;
    std::cout << "one_big_sequence = " << one_big_sequence << std::endl;
    std::cout << "end_of_beginning = " << end_of_beginning << std::endl;
    std::cout << "begin_of_ending = " << begin_of_ending << std::endl;
    std::cout << "number_of_end_of_beginning = " << number_of_end_of_beginning << std::endl;
    std::cout << "number_of_begin_of_ending = " << number_of_begin_of_ending << std::endl;
    std::cout << "begin_of_beginning = " << begin_of_beginning << std::endl;
    std::cout << "end_of_ending = " << end_of_ending << std::endl;
    std::cout << "after_segemnt_end = " << after_segemnt_end << std::endl;
    std::cout << "before_segment_begin = " << before_segment_begin << std::endl;
    std::cout << "end_of_segment_before = " << end_of_segment_before << std::endl;
    std::cout << "new_prev_val = " << new_prev_val << std::endl;
    std::cout << "begin_of_segment_next = " << begin_of_segment_next << std::endl;
    std::cout << "new_next_val = " << new_next_val << std::endl;
    std::cout << std::endl;
}

