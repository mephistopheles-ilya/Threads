import random

def generate_random_sequence(length, lower_bound, upper_bound):
    return [random.randint(lower_bound, upper_bound) for _ in range(length)]

def modify_sequence(arr):
    if len(arr) <= 2:
        return arr
    for i in range(len(arr) - 1, 1, -1):
        arr[i] = (arr[i - 1] + arr[i - 2]) / 2
    return arr

def write_sequence_to_file(sequence, filename):
    with open(filename, 'w') as file:
        for number in sequence:
            file.write(f"{number}\n")

def main():
    length = random.randint(1, 20)
    lower_bound = -100
    upper_bound = 100
    filename = 'random_sequence.txt'

    random_sequence = generate_random_sequence(length, lower_bound, upper_bound)
    
    modified_sequence = modify_sequence(random_sequence.copy())

    write_sequence_to_file(random_sequence, filename)

    print(length, end=", ")
    print("RESULT : ", end="")
    for number in modified_sequence:
        print(f'{number:3.2f}', end="")
        print(" ", end="")
    print()

if __name__ == "__main__":
    main()

