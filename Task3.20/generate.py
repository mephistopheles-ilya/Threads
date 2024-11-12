import random
import os

def generate_sequence(length, min_value, max_value):
    return [random.randint(min_value, max_value) for _ in range(length)]


def add_random_numbers(sequence, num_to_add, min_value, max_value):
    for _ in range(num_to_add):
        random_number = random.randint(min_value, max_value)
        random_index = random.randint(0, len(sequence))
        sequence.insert(random_index, random_number)
    return sequence

def find_max_element(sequence):
    max_element = None
    for i in range(len(sequence) - 2):
        if sequence[i] == sequence[i + 1] + sequence[i + 2]:
            if max_element is None:
                max_element = max(sequence[i], sequence[i + 1], sequence[i + 2])
            else:
                max_element =  max(sequence[i], sequence[i + 1], sequence[i + 2], max_element)
    return max_element

def count_greater_than_max(sequence, max_element):
    if max_element is None:
        return None
    return sum(1 for x in sequence if x > max_element)

def save_parts_to_files(sequence, filename):
    index = 0
    counter = 0
    res = None
    while index < len(sequence):
        res = counter
        part_length = random.randint(0, 8)
        if (random.randint(0, 1) == 0):
            part_length = 0
        if part_length > (len(sequence) - index):
            part_length = len(sequence) - index
        part = sequence[index:index + part_length]
        index += part_length

        current_filename = filename + "_" + f"{counter}"
        with open(current_filename, 'w') as f:
            f.write('\n'.join(map(str, part)))
        counter += 1
    return res


def main():

    length = 15
    min_value = -1
    max_value = 2
    filename = 'tests/file'
    number = None

    sequence = generate_sequence(length, min_value, max_value)
    add_random_numbers(sequence, random.randint(5, 10), 4, 10)
    
    max_element = find_max_element(sequence)
    number = save_parts_to_files(sequence, filename)
    if max_element is None:
        print(f'{number},There is no fibonachi segments. Let in this case Result = 0')
    else:
        count = count_greater_than_max(sequence, max_element)
        print(f'{number},Result = ', count)


if __name__ == "__main__":
    main()
