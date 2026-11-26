#!/usr/bin/env python3
"""Advent of Code 2025 — Day 2: Gift Shop.

Input: comma-separated ranges like "11-22,95-115,...".
Invalid ID = a number made only of a digit block repeated.

Part 1: the block repeats exactly twice (55, 6464, 123123).
Part 2: the block repeats at least twice (1111111, 123123123, ...).

Ranges can span billions of IDs, so candidates are generated from the
repeated-block shape instead of iterating every number.

Usage:
    python3 day02.py            # reads ./input.txt
    python3 day02.py other.txt
    python3 day02.py --test     # worked example
"""

import sys

SAMPLE = (
    "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,"
    "1698522-1698528,446443-446449,38593856-38593862,565653-565659,"
    "824824821-824824827,2121212118-2121212124"
)


def parse_ranges(text):
    ranges = []
    for chunk in text.replace("\n", ",").split(","):
        chunk = chunk.strip()
        if not chunk:
            continue
        lo, hi = chunk.split("-")
        ranges.append((int(lo), int(hi)))
    return ranges


def repeated_numbers(lo, hi, min_repeats, max_repeats=None):
    """Yield numbers in [lo, hi] whose digits are a block repeated.

    min_repeats/max_repeats bound how many times the block repeats.
    """
    for length in range(len(str(lo)), len(str(hi)) + 1):
        for block in range(1, length // 2 + 1):
            if length % block:
                continue
            repeats = length // block
            if repeats < min_repeats:
                continue
            if max_repeats is not None and repeats > max_repeats:
                continue

            # A block of `block` digits must not start with 0.
            for base in range(10 ** (block - 1), 10 ** block):
                value = int(str(base) * repeats)
                if value > hi:
                    break
                if value >= lo:
                    yield value


def solve(ranges, min_repeats, max_repeats=None):
    found = set()
    for lo, hi in ranges:
        found.update(repeated_numbers(lo, hi, min_repeats, max_repeats))
    return sum(found)


def main(argv):
    if len(argv) > 1 and argv[1] == "--test":
        ranges = parse_ranges(SAMPLE)
        part1 = solve(ranges, 2, 2)
        part2 = solve(ranges, 2)
        assert part1 == 1227775554, f"part1 sample: {part1}"
        assert part2 == 4174379265, f"part2 sample: {part2}"
        print("sample OK: part1=1227775554 part2=4174379265")
        return 0

    path = argv[1] if len(argv) > 1 else "input.txt"
    with open(path) as f:
        ranges = parse_ranges(f.read())

    print(f"part 1: {solve(ranges, 2, 2)}")
    print(f"part 2: {solve(ranges, 2)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
