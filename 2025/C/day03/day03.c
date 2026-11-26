/* Advent of Code 2025 — Day 3: Lobby
 *
 * Each input line is a bank of battery digits. Turn on exactly k batteries,
 * keeping their order, to make the largest possible number; sum that maximum
 * over all banks.
 *
 *   part 1: k = 2
 *   part 2: k = 12
 *
 * Greedy is optimal: for each output position, take the largest digit that
 * still leaves enough digits to fill the remaining positions.
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

static unsigned long long max_joltage(const char *bank, int len, int k)
{
    unsigned long long total = 0;
    int start = 0;
    int rem;

    if (k > len)
        return 0;

    for (rem = k; rem > 0; --rem)
    {
        int last = len - rem; /* last index we may pick from */
        int best = start;
        int i;

        for (i = start; i <= last; ++i)
        {
            if (bank[i] > bank[best])
                best = i;
        }
        total = total * 10 + (unsigned long long)(bank[best] - '0');
        start = best + 1;
    }
    return total;
}

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

static void strip(char *s)
{
    size_t n = strlen(s);

    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' ||
                     s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
}

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
            continue;

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
        /* run from a temp copy so the same parser is exercised */
        FILE *tmp = tmpfile();
        int rc;

        if (!tmp)
            return 1;
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
