#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "func.hpp"
#include "reduce_sum.hpp"

void func_1(FILE* fp, Arg* a) {
    double num1 = 0, num2 = 0, num3 = 0;
    int res1 = fscanf(fp, "%lf", &num1);
    int res2 = fscanf(fp, "%lf", &num2);
    int res3 = fscanf(fp, "%lf", &num3);

    if (res1 == 0 || res2 == 0 || res3 == 0) {
        a -> error = io_status::error_read;
        return;
    }

    if(res1 == EOF) {
        a -> error = io_status::succes;
        a -> elements_in_file = 0;
        return;
    }
    if (res1 == 1 && res2 == EOF) {
        a -> error = io_status::succes;
        a -> elements_in_file = 1;
        a -> first = num1;
        return;
    }
    if(res1 == 1 && res2 == 1 && res3 == EOF) {
        a -> error = io_status::succes;
        a -> elements_in_file = 2;
        a -> first = num1;
        a -> second = num2;
        return;
    }
    if(res1 == 1 && res2 == 1 && res3 == 1) {
        a -> error = io_status::succes;
        a -> elements_in_file = 3;
        a -> first = num1;
        a -> second = num2;
        a -> before_last = num3;
        if (std::fabs(num1 - num2 - num3) < EPS) {
            a -> local_fib_max = std::max({num1, num2, num3});
            a -> is_local_fib_exist = 1;
        }
    }

    num1 = num2;
    num2 = num3;
    while((res3 = fscanf(fp, "%lf", &num3)) == 1) {
        ++(a -> elements_in_file);
        if (std::fabs(num1 - num2 - num3) < EPS) {
            if (a -> is_local_fib_exist == 1) {
                a -> local_fib_max = std::max({num1, num2, num3, a -> local_fib_max});
            } else {
                a -> local_fib_max = std::max({num1, num2, num3});
                a -> is_local_fib_exist = 1;
            }
        }
        num1 = num2;
        num2 = num3;
        if (a -> elements_in_file == 4) {
            a -> last = num3;
        }
        if (a -> elements_in_file > 4) {
            a -> before_last = a ->last;
            a -> last = num3;
        }
    }

    if (res3 == EOF) {
        a -> error = io_status::succes;
        return;
    }
    if (res3 == 0) {
        a -> error = io_status::error_read;
        return;
    }
    return;
}


void func_2(FILE* fp, Arg* a) {
    double num1 = 0;
    int res1 = 0;
    while((res1 = fscanf(fp, "%lf", &num1)) == 1) {
            if(num1 > a->all_fib_max) {
                a->greater_than_fib += 1;
            }
    }
    if(res1 == 0) {
        a -> error = io_status::error_read;
    }
    if(res1 == EOF) {
        a -> error = io_status::succes;
    }
    return;
}



void* thread_func(void* arg) {
    Arg* a = (Arg*)arg;
    FILE* fp = fopen(a -> name, "r");
    
    if (fp == nullptr) {
        a -> error = io_status::error_open;
        a -> error_flag = 1;
    } else {
        a -> error_flag = 0;
    }
    reduce_sum(a -> p, &(a -> error_flag), 1);
    if(a -> error_flag > 0) {
        if (fp != nullptr) {
            fclose(fp);
        }
        return nullptr;
    }
    
    func_1(fp, a);
    fclose(fp);
    if (a -> error != io_status::succes) {
        a -> error_flag = 1;
    } else {
        a -> error_flag = 0;
    }

    synchronize(a -> p, a);
    if(a -> error_flag > 0) {
        return nullptr;
    }
    if(a -> all_elements == 0) {
        a -> error  = io_status::all_empty;
        return nullptr;
    }
    if(a -> is_all_fib_exist == 0) {
        a -> error = io_status::no_fib;
        return nullptr;
    }

    fp = fopen(a->name, "r");
    if (fp == nullptr) {
        a -> error = io_status::error_open;
        a -> error_flag = 1;
    } else {
        a -> error_flag = 0;
    }
    reduce_sum(a -> p, &(a -> error_flag), 1);
    if(a -> error_flag > 0) {
        if (fp != nullptr) {
            fclose(fp);
        }
        return nullptr;
    }
    func_2(fp, a); // count result
    fclose(fp);
    if (a -> error != io_status::succes) {
        a -> error_flag = 1;
    } else {
        a -> error_flag = 0;
    }
    reduce_sum(a -> p, &(a -> error_flag), 2);
    if(a -> error_flag > 0) {
        if (fp != nullptr) {
            fclose(fp);
        }
        return nullptr;
    }

    a -> error = io_status::succes;
    return nullptr;
}

void process_arg(Arg* a, int p) {
    for(int k = 0; k < p; ++k) {
        if(a[k].error != io_status::succes) {
            switch(a[k].error) {
                case io_status::error_open:
                    printf("Error with file opening [%s]\n", a[k].name);
                    break;
                case io_status::error_read:
                    printf("Error with file reading [%s]\n", a[k].name);
                    break;
                case io_status::all_empty:
                    printf("All files are empty. Let in this case Result = 0\n");
                    k = p;
                    for(int i = 0; i < p; ++i) {
                        a -> error_flag = 1;
                    }
                    break;
                case io_status::no_fib:
                    printf("There is no fibonachi segments. Let in this case Result = 0\n");
                    for(int i = 0; i < p; ++i) {
                        a -> error_flag = 1;
                    }
                    k = p;
                    break;
                default:
                    break;
            }
        }
    }
}

void synchronize(int p, Arg* a) {
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double all_fib_max = 0;
    static int is_all_fib_exist = 0;
    static int all_elements = 0;
    if(p <= 1) {
        a -> all_fib_max = a -> local_fib_max;
        a -> is_all_fib_exist = a -> is_local_fib_exist;
        a -> all_elements = a -> elements_in_file;
        return;
    }
    pthread_mutex_lock(&m);
    ++t_in;
    if(t_in >= p) {
        t_out = 0;
        int i, j;
        int p = a -> p;
        Arg* arr_ptr = a - (a -> k);
        double num1 = 0, num2 = 0, num3 = 0;
        int next_elems = 0;

        for(i = 0; i < p; ++i) {
            if (arr_ptr[i].error_flag == 1) {
                for(j = 0; j < p; ++j) {
                    arr_ptr[j].error_flag = 1;
                }
                break;
            }
            all_elements += arr_ptr[i].elements_in_file;
            if (arr_ptr[i].elements_in_file == 0) {
                continue;
            }
            if (arr_ptr[i].elements_in_file == 1) {
                num1 = arr_ptr[i].first;
                next_elems = find_next_elements(arr_ptr, i + 1, p, num2, num3);
                if (next_elems == 2) {
                    update_max_fib(all_fib_max, is_all_fib_exist, num1, num2, num3);
                }
                continue;
            }
            if (arr_ptr[i].elements_in_file == 2) {
                next_elems = find_next_elements(arr_ptr, i + 1, p, num2, num3);
                if(next_elems == 1) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].first, arr_ptr[i].second, num2);
                }
                if(next_elems == 2) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].first, arr_ptr[i].second, num2);
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].second, num2, num3);
                }
                continue;
            }
            if (arr_ptr[i].elements_in_file == 3) {
                next_elems = find_next_elements(arr_ptr, i + 1, p, num2, num3);
                if(next_elems == 1) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].second, arr_ptr[i].before_last, num2);
                }
                if(next_elems == 2) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].second, arr_ptr[i].before_last, num2);
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].before_last, num2, num3);
                }
                if (arr_ptr[i].is_local_fib_exist == 1) {
                    if (is_all_fib_exist == 0) {
                        is_all_fib_exist = 1;
                        all_fib_max = arr_ptr[i].local_fib_max;
                    } else {
                        all_fib_max = std::max({all_fib_max, arr_ptr[i].local_fib_max});
                    }
                }
                continue;
            }
            if (arr_ptr[i].elements_in_file >= 4) {
                next_elems = find_next_elements(arr_ptr, i + 1, p, num2, num3);
                if(next_elems == 1) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].before_last, arr_ptr[i].last, num2);
                }
                if(next_elems == 2) {
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].before_last, arr_ptr[i].last, num2);
                    update_max_fib(all_fib_max, is_all_fib_exist, arr_ptr[i].last, num2, num3);
                }
                if (arr_ptr[i].is_local_fib_exist == 1) {
                    if (is_all_fib_exist == 0) {
                        is_all_fib_exist = 1;
                        all_fib_max = arr_ptr[i].local_fib_max;
                    } else {
                        all_fib_max = std::max({all_fib_max, arr_ptr[i].local_fib_max});
                    }
                }
                continue;
            }
        }

        pthread_cond_broadcast(&c_in);
    } else {
        while(t_in < p) {
            pthread_cond_wait(&c_in, &m);
        }
    }
    a -> all_fib_max = all_fib_max;
    a -> is_all_fib_exist = is_all_fib_exist;
    a -> all_elements = all_elements;
   
    ++t_out;
    if(t_out >= p) {
        t_in = 0;
        all_fib_max = 0;
        is_all_fib_exist = 0;
        all_elements = 0;
        pthread_cond_broadcast(&c_out);
    } else {
        while (t_out < p) {
            pthread_cond_wait(&c_out, &m);
        }
    }

    pthread_mutex_unlock(&m);
}

void update_max_fib(double& all_fib_max, int& is_all_fib_exist, double num1, double num2, double num3) {
    if (std::fabs(num1 - num2 - num3) < EPS) {
        if (is_all_fib_exist == 0) {
            is_all_fib_exist = 1;
            all_fib_max = std::max({num1, num2, num3});
        } else {
            all_fib_max = std::max({num1, num2, num3, all_fib_max});
        }
    }
}


int find_next_elements(Arg* arr_ptr, int j, int p, double& num2, double& num3) {
    int found_next_elems = 0;
    for(; j < p; ++j) {
        if (arr_ptr[j].elements_in_file == 0) {
            continue;
        }
        if (arr_ptr[j].elements_in_file == 1) {
            if(found_next_elems == 0) {
                num2 = arr_ptr[j].first;
                found_next_elems = 1;
            } else if(found_next_elems == 1) {
                num3 = arr_ptr[j].first;
                found_next_elems = 2;
                return 2;
            }
        }
        if (arr_ptr[j].elements_in_file >= 2) {
            if (found_next_elems == 0) {
                num2 = arr_ptr[j].first;
                num3 = arr_ptr[j].second;
                found_next_elems = 2;
                return 2;
            } else if (found_next_elems == 1) {
                num3 = arr_ptr[j].first;
                found_next_elems = 2;
                return 2;
            }
        }
    }
    return found_next_elems;
}
