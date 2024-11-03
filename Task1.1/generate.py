import random

def create_files(num_files):
    res_sum = 0
    for i in range(num_files):
        with open(f"tests/file_{i}", "w") as f:
            num_numbers = random.randint(0, 10000)
            numbers = [random.uniform(-100000, 100000) for _ in range(num_numbers)]
            f.write("\n".join(map(str, numbers)))

            count = sum(1 for j in range(1, len(numbers) - 1) if numbers[j] < numbers[j - 1]
                    and numbers[j] < numbers[j + 1])
            res_sum += count
    return res_sum

res = create_files(20)
print(f"Result = {res}")
