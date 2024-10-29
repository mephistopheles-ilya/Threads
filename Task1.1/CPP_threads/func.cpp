#include "func.hpp"

void func_1(FILE* fp, Arg& a) {
    double num1 = 0, num2 = 0, num3 = 0;
    int res1 = fscanf(fp, "%lf", &num1);
    int res2 = fscanf(fp, "%lf", &num2);
    int res3 = fscanf(fp, "%lf", &num3);
    if (res1 == 0 || res2 == 0 || res3 == 0) {
        a.error = io_status::error_read;
        return;
    }
    if(res1 == EOF || res2 == EOF || res3 == EOF) {
        a.error = io_status::succes;
        a.res = 0;
        return;
    }
    if(num2 < num1 && num2 < num3) {
        a.res += 1;
    }
    num1 = num2;
    num2 = num3;
    while((res3 = fscanf(fp, "%lf", &num3)) == 1) {
            if(num2 < num1 && num2 < num3) {
                a.res += 1;
            }
            num1 = num2;
            num2 = num3;
    }
    if(res3 == 0) {
        a.error = io_status::error_read;
    }
    if(res3 == EOF) {
        a.error = io_status::succes;
    }
    return;
}

void thread_func(Arg& arg) {
    FILE* fp = fopen(arg.name, "r");
    if (fp == nullptr) {
        arg.error = io_status::error_open;
        return;
    }
    func_1(fp, arg);
    fclose(fp);
    return; 
}

void process_arg(std::vector<Arg>& a, int& res, bool& flag) {
    for(auto& arg: a) {
        if(arg.error == io_status::succes) {
            res += arg.res;
        } else {
            switch(arg.error) {
                case io_status::error_open:
                    printf("Error with file opening [%s]\n", arg.name);
                    flag = true;
                    break;
                case io_status::error_read:
                    printf("Error with file reading [%s]\n", arg.name);
                    flag = true;
                    break;
                default:
                    break;
            }
        }
    }
}

