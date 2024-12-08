#include <ctime>
#include <stdio.h>
#include <stdlib.h>

using ulli = unsigned long long int;

bool is_prime_1(ulli number) {
    if ((number == 2) || (number == 3)) {
        return true;
    }
    if ((number < 2) || (number % 2 == 0) || (number % 3 == 0)) {
        return false;
    }
    for(ulli j = 5; j * j <= number; j += 6) {
        if ((number % j == 0) || (number % (j + 2) == 0)) {
            return false;
        }
    }
    return true;
}

bool is_prime_2(ulli number) {
    if (number == 2) {
        return true;
    }
    if ((number < 2) || (number % 2 == 0)) {
        return false;
    }
    for(ulli j = 3; j * j <= number; j += 2) {
        if (number % j == 0) {
            return false;
        }
    }
    return true;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage %s <n>\n", argv[0]);
        return 1;
    }
    ulli n = 0;
    if (sscanf(argv[1], "%llu", &n) != 1) {
        printf("Usage %s <n>\n", argv[0]);
        return 1;
    }
    ulli max_gap = 0;
    ulli prev_prime_number = 0;
    ulli prime_count = 0;
    ulli i = 1;

    double t = clock();
    while(prime_count < n) {
        if (is_prime_1(i) == true) {
            ++prime_count;
            if (i - prev_prime_number > max_gap && prev_prime_number > 0) {
                max_gap = i - prev_prime_number;
            }
            prev_prime_number = i;
        }
        ++i;
    }
    t = (clock() - t) / CLOCKS_PER_SEC;
    printf("Result = %llu\n", max_gap);
    printf("Time = %lf\n", t);

    return 0;
}



    
