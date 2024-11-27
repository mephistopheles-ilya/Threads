#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <iomanip>

struct A {
    int begin = 0;
    int end = 0;
    double new_value = 0;
};

#define EPS 1e-16

int main(int argc, char* argv[]) {
    if(argc != 6) {
        std::cout << "Usage <amount of elements> <a> <b> <init> <res>" << std::endl;
        return 1;
    }
    int n = 0, a = 0, b = 0;
    if(sscanf(argv[1], "%d", &n) != 1 || sscanf(argv[2], "%d", &a) != 1 || sscanf(argv[3], "%d", &b) != 1) {
        std::cout << "Usage <amount of elements> <output file> <a> <b>" << std::endl;
        return 2;
    }

    std::random_device rd; 
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> number(a, b);
    std::vector<double> array{1, 0, -1, 25, 51};
    n = array.size();
#if 0
    for(int i = 0; i < n; ++i) {
        array[i] = number(generator);
    }
#endif
    std::vector<A> changes;
    int begin = 0, end = 0;
    for(int i = 1; i < n - 1;) {
        if (std::fabs(array[i] - (array[i - 1] + array[i + 1]) * 0.5) < EPS) {
            begin = i - 1;
            end = i + 1;
            int j = i + 1;
            for(;j < n - 1; ++j) { 
                if (std::fabs(array[j] - (array[j - 1] + array[j + 1]) * 0.5) < EPS) {
                    end = j + 1;
                } else {
                    break;
                }
            }
            A a{begin, end, (array[begin] + array[end]) * 0.5};
            changes.push_back(a);
            i = end;
        } else {
            ++i;
        }
    }
    std::ofstream of_(argv[4]);
    of_ << std::fixed << std::setprecision(2) << std::setw(3);
    std::ostream_iterator<double> oit_(of_, " ");
    std::ranges::copy(array, oit_);
    of_ << "\n";

    int changed = 0;
    for(auto& a: changes) {
        std::cout << "Begin : " << a.begin << ' ' << "End : " << a.end << ' ' << "Value : " << a.new_value << std::endl; 
        for(int i = a.begin; i <= a.end; ++i) {
            array[i] = a.new_value;
            ++changed;
        }
    }

    std::ofstream of(argv[5]);
    of << std::fixed << std::setprecision(2) << std::setw(3);
    std::ostream_iterator<double> oit(of, " ");
    std::ranges::copy(array, oit);
    of << "\n";
    std::cout << changed << std::endl;
    return 0;
}





