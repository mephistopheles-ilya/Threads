#include "func.hpp"
#include "reduce_sum.hpp"

void func_1(FILE* fp, Arg* a) {
    double num1 = 0, num2 = 0, num3 = 0;
    int res1 = fscanf(fp, "%lf", &num1);
    int res2 = fscanf(fp, "%lf", &num2);
    int res3 = fscanf(fp, "%lf", &num3);
    if (res1 == 0 || res2 == 0 || res3 == 0) {
        a->error = io_status::error_read;
        return;
    }
    if(res1 == EOF || res2 == EOF || res3 == EOF) {
        a->error = io_status::succes;
        a->res = 0;
        return;
    }
    if(num2 < num1 && num2 < num3) {
        a -> res += 1;
    }
    num1 = num2;
    num2 = num3;
    while((res3 = fscanf(fp, "%lf", &num3)) == 1) {
            if(num2 < num1 && num2 < num3) {
                a -> res += 1;
            }
            num1 = num2;
            num2 = num3;
    }
    if(res3 == 0) {
        a -> error = io_status::error_read;
    }
    if(res3 == EOF) {
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
    fclose(fp);
    a -> error = io_status::succes;
    reduce_sum(a -> p, &(a -> res), 1);
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
                default:
                    break;
            }
        }
    }
}

