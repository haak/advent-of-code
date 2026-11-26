#!/usr/bin/env python3
"""Advent of Code 2025 — Day 1: Secret Entrance.

Dial 0..99 starts at 50. Rotations like "L68" / "R48".

Part 1: how many rotations leave the dial pointing at 0?
Part 2: how many times does the dial point at 0 at any point *during* a
        rotation (including the final click of each rotation)?

Usage:
    python3 day01.py            # reads ./input.txt
    python3 day01.py other.txt
    python3 day01.py --test     # runs the worked example and asserts
"""

import sys

START = 50
SIZE = 100

SAMPLE = """\
L68
L30
R48
L5
R60
L55
L1
L99
R14
L82
"""


def parse(text):
    for line in text.split():
        direction, distance = line[0], int(line[1:])
        yield direction, distance


def solve(rotations):
    pos = START
    landings = 0   # part 1: rotations that end on 0
    crossings = 0  # part 2: every click that lands on 0

    for direction, distance in rotations:
        # First click (1..SIZE) that reaches 0 in this direction.
        if direction == "R":
            first = (-pos) % SIZE or SIZE
        else:  # "L"
            first = pos % SIZE or SIZE

        if first <= distance:
            crossings += 1 + (distance - first) // SIZE

        step = distance if direction == "R" else -distance
        pos = (pos + step) % SIZE
        if pos == 0:
            landings += 1

    return landings, crossings


def main(argv):
    if len(argv) > 1 and argv[1] == "--test":
        got = solve(parse(SAMPLE))
        assert got == (3, 6), f"sample failed: {got}, expected (3, 6)"
        print("sample OK: part1=3 part2=6")
        return 0

    path = argv[1] if len(argv) > 1 else "input.txt"
    with open(path) as f:
        part1, part2 = solve(parse(f.read()))
    print(f"part 1: {part1}")
    print(f"part 2: {part2}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
