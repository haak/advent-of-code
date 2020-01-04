from collections import Counter    

def meets_part_1(num):
    digits = str(num)
    return list(digits) == sorted(digits) and any(x >= 2 for x in Counter(digits).values())

def meets_part_2(num):
    digits = str(num)
    return list(digits) == sorted(digits) and any(x == 2 for x in Counter(digits).values())

print(sum(1 for num in range(245182, 790572) if meets_part_1(num)))
print(sum(1 for num in range(245182, 790572) if meets_part_2(num)))
