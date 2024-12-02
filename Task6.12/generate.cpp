#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <iterator>
#include <iomanip>



int main(int argc, char* argv[]) {
    if(argc != 7) {
        std::cout << "Usage <amount_of_elements1> <amount_of_elements2> <a> <b> <init> <res>" << std::endl;
        return 1;
    }
    int n1 = 0, n2 = 0, a = 0, b = 0;
    if(sscanf(argv[1], "%d", &n1) != 1 || sscanf(argv[2], "%d", &n2) != 1 ||
            sscanf(argv[3], "%d", &a) != 1 || sscanf(argv[4], "%d", &b) != 1) {
        std::cout << "Usage <amount_of_elements1> <amount_of_elements2> <a> <b> <init> <res>" << std::endl;
        return 2;
    }

    if (n1 <= 0 || n2 <= 0) {
        std::cout << "Wrong type of arguments" << std::endl;
    }

    int n = n1 * n2;
    std::random_device rd; 
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> number(a, b);
    std::uniform_int_distribution<int> place(0, n - 1);
    std::vector<double> matrix(n);
    std::vector<double> matrix_copy(n);

    for(int i = 0; i < n; ++i) {
        matrix[i] = number(generator);
        matrix_copy[i] = matrix[i];
    }
    int pl = place(generator);
    matrix[pl] = -100;
    matrix_copy[pl] = -100;

    int min_elem = matrix[0];
    for(int i = 0; i < n; ++i) {
        if (matrix[i] < min_elem) {
            min_elem = matrix[i];
        }
    }

    int changed = 0;
    for(int i = 1; i < (n1 - 1); ++i) {
        for(int j = 1; j < (n2 - 1); ++j) {
            if(matrix[i * n2 + j] <= matrix[i * n2 + (j - 1)] && matrix[i * n2 + j] <= matrix[i * n2 + (j + 1)]
            && matrix[i * n2 + j] <= matrix[(i - 1) * n2 + j] && matrix[i * n2 + j] <= matrix[(i + 1) * n2 + j]) {
                matrix_copy[i * n2 + j] = min_elem;
                ++changed;
            }
        }
    }

    std::ofstream of_(argv[5]);
    of_ << std::fixed << std::setprecision(2);
    for(int i = 0; i < n1; ++i) {
        for(int j = 0; j < n2; ++j) {
            of_ << " " << std::setw(6) << matrix[i * n2 + j];
        }
        of_ << std::endl;
    }


    std::ofstream of(argv[6]);
    of << changed << std::endl;
    of << std::fixed << std::setprecision(2);
    for(int i = 0; i < n1; ++i) {
        for(int j = 0; j < n2; ++j) {
            of << " " <<  std::setw(6) << matrix_copy[i * n2 + j];
        }
        of << std::endl;
    }
    of << std::endl;

    return 0;
}





