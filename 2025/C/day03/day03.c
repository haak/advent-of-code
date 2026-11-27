/* Advent of Code 2025 — Day 3: Lobby
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * Each input line is a bank of battery digits, for example
 *
 *     987654321111111
 *
 * You must switch on exactly k batteries. The batteries you keep stay in
 * their original order and spell a number (each digit is one place value).
 * Pick the batteries that make that number as large as possible. Then add
 * the best numbers from every bank.
 *
 *   part 1: k = 2
 *   part 2: k = 12
 *
 * Worked example (--test): part 1 gives 357.
 *
 * THE MAIN IDEA
 * -------------
 * Greedy works: for the FIRST digit, take the largest digit that still
 * leaves enough digits to the right to fill the remaining k-1 positions.
 * Then repeat for the next position, starting just after the one you took.
 *
 * Why not a nested loop over every combination? For 12 out of ~100 digits
 * the number of combinations is astronomically large, but the greedy pass
 * is O(k * line_length) — a few thousand steps.
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - A string is a `char` array ending in '\0'. `strlen` counts the
 *     characters before that terminator.
 *   - `'7' - '0'` converts the character '7' into the number 7, because
 *     digits are stored in order in the ASCII table.
 *   - `unsigned long long` holds the 12-digit answers without overflow.
 *   - `static` functions and globals are private to this file.
 *   - `fgets` reads one line at a time; `tmpfile` plus `rewind` lets
 *     --test feed the sample through the same code as the real input.
 *
 * Build:  cc -O2 -Wall -Wextra -o day03 day03.c
 * Run:    ./day03            (reads ./input.txt)
 *         ./day03 other.txt
 *         ./day03 --test     (worked example + brute-force cross-check)
 */

#include <stdio.h>
#include <string.h>

#define MAX_LINE 8192
#define PART2_DIGITS 12

/* Return the largest number obtainable from `bank` by keeping exactly k
 * digits in their original order.
 *
 * `start` is the first index we are still allowed to look at. On each pass
 * we choose one digit, then move `start` past it. */
static unsigned long long max_joltage(const char *bank, int len, int k)
{
    unsigned long long total = 0;
    int start = 0;
    int rem;

    if (k > len)
        return 0;              /* cannot pick more digits than exist */

    /* `rem` counts how many digits are still to be chosen, including this
     * one. It starts at k and counts down to 1. */
    for (rem = k; rem > 0; --rem)
    {
        /* The chosen index can be at most len - rem: if we picked anything
         * later, there would not be enough digits left to fill the other
         * rem - 1 positions. */
        int last = len - rem;
        int best = start;
        int i;

        /* Scan the allowed window for the biggest digit. Because we only
         * replace `best` on a strictly greater digit, ties keep the earliest
         * one, which leaves the most room for later digits. */
        for (i = start; i <= last; ++i)
        {
            if (bank[i] > bank[best])
                best = i;
        }

        /* Append the chosen digit to the number built so far. Multiplying
         * by 10 shifts the old digits one place left. */
        total = total * 10 + (unsigned long long)(bank[best] - '0');

        /* Next digit must come after the one we just used. */
        start = best + 1;
    }
    return total;
}

/* Slow but obviously-correct version for k = 2: try every pair of indices.
 * --test compares this with the greedy answer, so a bug in the clever code
 * cannot silently pass the sample. */
static unsigned long long brute_two(const char *bank, int len)
{
    unsigned long long best = 0;
    int i, j;

    for (i = 0; i < len; ++i)
        for (j = i + 1; j < len; ++j)
        {
            unsigned long long v =
                (unsigned long long)(bank[i] - '0') * 10 +
                (unsigned long long)(bank[j] - '0');
            if (v > best)
                best = v;
        }
    return best;
}

/* Remove trailing whitespace (the newline fgets leaves behind). */
static void strip(char *s)
{
    size_t n = strlen(s);

    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' ||
                     s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
}

/* Read every bank from `fp` and add up both parts.
 * `test` = 1 means we are running the sample and should check the answer. */
static int run(FILE *fp, int test)
{
    char line[MAX_LINE];
    unsigned long long p1 = 0, p2 = 0;
    int banks = 0;

    while (fgets(line, sizeof line, fp))
    {
        int len;

        strip(line);
        len = (int)strlen(line);
        if (len == 0)
            continue;             /* skip blank lines */

        /* In test mode, prove the greedy result matches brute force for the
         * two-digit case before trusting it. */
        if (test && brute_two(line, len) != max_joltage(line, len, 2))
        {
            printf("FAIL: %s greedy=%llu brute=%llu\n", line,
                   max_joltage(line, len, 2), brute_two(line, len));
            return 1;
        }

        p1 += max_joltage(line, len, 2);
        p2 += max_joltage(line, len, PART2_DIGITS);
        ++banks;
    }

    if (test)
    {
        if (p1 != 357ULL)
        {
            printf("FAIL: sample part 1 = %llu, expected 357\n", p1);
            return 1;
        }
        printf("sample OK: part1=357 (brute-force cross-check passed)\n");
        return 0;
    }

    printf("banks: %d\n", banks);
    printf("part 1: %llu\n", p1);
    printf("part 2: %llu\n", p2);
    return 0;
}

int main(int argc, char **argv)
{
    const char *path = "input.txt";
    FILE *fp;

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        static const char *sample =
            "987654321111111\n"
            "811111111111119\n"
            "234234234234278\n"
            "818181911112111\n";
        /* Run from a temp copy so the same parser is exercised. */
        FILE *tmp = tmpfile();
        int rc;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
        fputs(sample, tmp);
        rewind(tmp);
        rc = run(tmp, 1);
        fclose(tmp);
        return rc;
    }

    if (argc > 1)
        path = argv[1];

    fp = fopen(path, "r");
    if (!fp)
    {
        perror(path);
        return 1;
    }
    {
        int rc = run(fp, 0);
        fclose(fp);
        return rc;
    }
}
