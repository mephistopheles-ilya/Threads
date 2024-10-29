#include "func.hpp"

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
        return nullptr;
    }
    func_1(fp, a);
    fclose(fp);
    return nullptr;
}

void process_arg(Arg* a, int& res, bool& flag, int p) {
    for(int k = 0; k < p; ++k) {
        if(a[k].error == io_status::succes) {
            res += a[k].res;
        } else {
            switch(a[k].error) {
                case io_status::error_open:
                    printf("Error with file opening [%s]\n", a[k].name);
                    flag = true;
                    break;
                case io_status::error_read:
                    printf("Error with file reading [%s]\n", a[k].name);
                    flag = true;
                    break;
                default:
                    break;
            }
        }
    }
}

