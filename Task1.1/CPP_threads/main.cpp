#include <iostream>
#include <thread>
#include <vector>

#include "func.hpp"


int main(int argc, char* argv[]) {
    int p = argc - 1, res = 0;
    bool flag = false;
    std::vector<std::thread> threads;
    std::vector<Arg> a;
    if (argc == 1) {
        std::cout << "Usage " << argv[0] << "  <files>" << std::endl;
        return 1;
    }
    threads.reserve(p - 1);
    a.reserve(p);
    for(int k = 0; k < p; ++k) {
        a.emplace_back(p, k, argv[k + 1]);
    }
    for(int k = 0; k < (p - 1); ++k) {
        threads.emplace_back(thread_func, std::ref(a[k]));
    }
    thread_func(a[p - 1]);
    for(int k = 0; k < (p - 1); ++k) {
        threads[k].join();
    }
    process_arg(a, res, flag);
    if(flag == false) {
        printf("Result = %d\n", res);
    }
    return 0;
}



