#include <stdio.h>
#include <stdlib.h>

using ulli = unsigned long long int;

bool is_prime(ulli number) {
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

    while(prime_count < n) {
        if (is_prime(i) == true) {
            ++prime_count;
            if (i - prev_prime_number > max_gap) {
                max_gap = (prev_prime_number > 0) ? (i - prev_prime_number) : 0;
            }
            prev_prime_number = i;
        }
        ++i;
    }
    printf("Result = %llu\n", max_gap);

    return 0;
}



    
