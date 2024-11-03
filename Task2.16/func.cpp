#include <cmath>
#include <stdlib.h>
#include <stdio.h>

#include "func.hpp"
#include "reduce_sum.hpp"

void func_1(FILE* fp, Arg* a) {
    double num1 = 0, num2 = 0;
    int res1 = fscanf(fp, "%lf", &num1);
    int res2 = fscanf(fp, "%lf", &num2);
    if (res1 == 0 || res2 == 0) {
        a->error = io_status::error_read;
        return;
    }
    if(res1 == EOF) {
        a->error = io_status::succes;
        a->empty_files = 1;
        a->all_numbers = 0;
        return;
    }
    if(res2 == EOF) {
        a->error = io_status::succes;
        a-> all_numbers = 1;
        return;
    }
    if(std::fabs(num1 - num2) < EPS) {
        a->is_stable_exist = 1;
        a->max_stable_elem = num1;
    }
    a->all_numbers = 2;
    num1 = num2;
    while((res2 = fscanf(fp, "%lf", &num2)) == 1) {
        a->all_numbers += 1; 
        if(std::fabs(num1 - num2) < EPS) {
            if (a->is_stable_exist == 0) {
                a->is_stable_exist = 1;
                a->max_stable_elem = num1;
            } else {
                a->max_stable_elem = (a->max_stable_elem > num1 ? a->max_stable_elem : num1);
            }
        }
        num1 = num2;
    }
    if(res2 == 0) {
        a -> error = io_status::error_read;
    }
    if(res2 == EOF) {
        a -> error = io_status::succes;
    }
    return;
}

void func_2(FILE* fp, Arg* a) {
    double num1 = 0;
    int res1 = 0;
    while((res1 = fscanf(fp, "%lf", &num1)) == 1) {
            if(num1 > a->max_stable_elem) {
                a->greater_than_stable += 1;
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
    
    func_1(fp, a); // count max_stable_elem
    fclose(fp);
    if (a -> error != io_status::succes) {
        a -> error_flag = 1;
    } else {
        a -> error_flag = 0;
    }
    reduce_sum(a -> p, &(a -> error_flag), 1);
    if(a -> error_flag > 0) {
        return nullptr;
    }
    int remember_local = a->is_stable_exist;
    reduce_sum(a -> p, &(a -> all_numbers), 3);
    if(a -> empty_files == a -> p) {
        a -> error  = io_status::all_empty;
        return nullptr;
    }
    if(a -> is_stable_exist == 0) {
        a -> error = io_status::no_stable;
        return nullptr;
    }
    a->is_stable_exist = remember_local;

    reduce_max(a -> p, &(a -> max_stable_elem), 1, a ->is_stable_exist);

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
    reduce_sum(a -> p, &(a -> error_flag), 1);
    if(a -> error_flag > 0) {
        if (fp != nullptr) {
            fclose(fp);
        }
        return nullptr;
    }

    a -> error = io_status::succes;
    reduce_sum(a -> p, &(a -> greater_than_stable), 1);
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
                case io_status::no_stable:
                    printf("There is no constant segemnts.\n");
                    printf("Let in this case Result = %d\n", a->all_numbers);
                    printf("It is amount of all numbers\n");
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

