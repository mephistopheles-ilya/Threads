import random
import os
import sys

def create_file(filename, length):
    with open(filename, 'w') as f:
        numbers = [random.randint(-100, 100) for _ in range(length)]
        f.write('\n'.join(map(str, numbers)))

def analyze_sequence(filename):
    with open(filename, 'r') as f:
        numbers = list(map(int, f.readlines()))
    
    max_value = None
    length = len(numbers)

    for i in range(len(numbers) - 1):
        if numbers[i] == numbers[i + 1]:
            if max_value is None:
                max_value = numbers[i]
            else:
                if max_value < numbers[i]:
                    max_value = numbers[i]
    return max_value, length


def main():
    num_files = 20
    min_length = 0
    max_length = int(sys.argv[1])

    max_value_overall = None

    for i in range(num_files + 1):
        length = random.randint(min_length, max_length)
        filename = f'tests/file_{i + 1}'
        create_file(filename, length)

    flag = False
    all_lenthg = 0
    for i in range(num_files):
        filename = f'tests/file_{i + 1}'
        max_value, length = analyze_sequence(filename)
        all_lenthg += length
        if max_value_overall is None or (max_value is not None and max_value > max_value_overall):
            max_value_overall = max_value
        if max_value is not None:
            flag = True

    count_greater = 0
    if (max_value_overall is not None) :
        for i in range(num_files):
            filename = f'tests/file_{i + 1}'
            with open(filename, 'r') as f:
                numbers = list(map(int, f.readlines()))
                if (len(numbers) > 0):
                    count_greater += sum(1 for num in numbers if num > max_value_overall)

    if(flag is True):
        print(f'Result = {count_greater}')
    else:
        if all_lenthg == 0:
            print("All files are empty. Let in this case Result = 0");
        else:
            print("There is no constant segemnts.")
            print(f'Let in this case Result = {all_lenthg}')
            print("It is amount of all numbers")

if __name__ == "__main__":
    main()

