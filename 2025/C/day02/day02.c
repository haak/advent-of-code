/* Advent of Code 2025 — Day 2: Gift Shop
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * The input is one line of comma-separated ID ranges:
 *
 *     11-22,95-115,998-1012,...
 *
 * An ID is "invalid" when its digits are one block repeated. For example
 * 55 (5 twice), 6464 (64 twice) and 1111111 (1 seven times) are invalid.
 *
 *   part 1: sum the invalid IDs where the block repeats EXACTLY twice.
 *   part 2: sum the invalid IDs where the block repeats AT LEAST twice.
 *           (A number counted in part 1 is also counted in part 2.)
 *
 * Worked example (--test): 1227775554 and 4174379265.
 *
 * THE MAIN IDEA
 * -------------
 * The ranges contain billions of numbers, so checking every ID one by one
 * would be far too slow. Instead we build the invalid IDs directly:
 *
 *     pick a block length   e.g. 2  ->  a block like "64"
 *     pick a repeat count   e.g. 3  ->  "64" + "64" + "64" = 646464
 *     keep it if it lies inside one of the input ranges
 *
 * There are only about a million such candidates in total, which a laptop
 * handles instantly.
 *
 * One subtlety: the same number can be produced twice. 1111 is "11" twice
 * but also "1" four times, so the candidate list is sorted and de-duplicated
 * before summing.
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - `unsigned long long` is an integer type with at least 64 bits, good
 *     for IDs up to about 1.8 * 10^19. Plain `int` would overflow.
 *   - A `struct` groups related values. `typedef` gives the struct a short
 *     name (`Range`) so we do not have to write `struct Range` every time.
 *   - `malloc`/`realloc` get memory from the heap at run time; `free`
 *     gives it back. `fread` reads a whole file in one go.
 *   - `qsort` sorts an array. We give it a comparison function; `const
 *     void *` is how qsort passes "a pointer to an element, type unknown",
 *     and we cast it back to `Range *` inside.
 *   - `strtoull` parses an unsigned number and updates a pointer so the
 *     next call can continue from where the last one stopped.
 *   - Integer overflow is silent in C (the value just wraps), so we check
 *     before multiplying instead of hoping for the best.
 *
 * Build:  cc -O2 -Wall -Wextra -o day02 day02.c
 * Run:    ./day02            (reads ./input.txt)
 *         ./day02 other.txt
 *         ./day02 --test     (worked example: 1227775554 / 4174379265)
 *         ./day02 --viz      (animated walk through the worked example;
 *                             VIZ_FAST=1 skips the delays)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../viz.h"

#define MAX_RANGES 1024        /* the input has far fewer ranges than this */
#define MAX_FOUND 1000000      /* room for every invalid ID we might find */

typedef struct
{
    unsigned long long lo;     /* first ID in the range, inclusive */
    unsigned long long hi;     /* last ID in the range, inclusive  */
} Range;

/* Candidates collected from all ranges, before de-duplication. */
static unsigned long long found[MAX_FOUND];
static int n_found;

/* How many decimal digits does v have?  (0 has 1 digit.) */
static int ndigits(unsigned long long v)
{
    int n = 1;

    while (v >= 10)
    {
        v /= 10;
        ++n;
    }
    return n;
}

/* 10^e as an unsigned long long: pow10(3) == 1000. */
static unsigned long long pow10u(int e)
{
    unsigned long long p = 1;

    while (e-- > 0)
        p *= 10;
    return p;
}

/* Add every invalid ID in [lo, hi] whose block repeats between
 * `min_repeats` and `max_repeats` times (0 in max_repeats means "no limit").
 *
 * `len`     = how many digits the whole number has
 * `block`   = how many digits the repeating block has
 * `repeats` = len / block
 * `base`    = the block itself, treated as a number
 *
 * A block may not start with 0, so base runs from 10^(block-1) to 10^block-1
 * (for a 2-digit block: 10..99). */
static void collect(unsigned long long lo, unsigned long long hi,
                    int min_repeats, int max_repeats)
{
    int lo_len = ndigits(lo);
    int hi_len = ndigits(hi);
    int len, block;

    for (len = lo_len; len <= hi_len; ++len)
    {
        /* A block must be at most half the number's length, otherwise it
         * cannot repeat even twice. */
        for (block = 1; block <= len / 2; ++block)
        {
            int repeats = len / block;
            unsigned long long base, base_hi, mult, value;
            int i;

            if (len % block != 0)
                continue;              /* the block does not fit exactly */
            if (repeats < min_repeats)
                continue;
            if (max_repeats > 0 && repeats > max_repeats)
                continue;

            mult = pow10u(block);      /* shift left by one block: x100 here */
            base = pow10u(block - 1);  /* smallest block with no leading 0  */
            base_hi = mult - 1;        /* largest block with `block` digits  */

            for (; base <= base_hi; ++base)
            {
                int too_big = 0;

                /* If even the first digit of the block is already past hi,
                 * every larger block will be too, so stop. */
                if (base > hi)
                    break;

                value = base;
                for (i = 1; i < repeats; ++i)
                {
                    /* We want value = value * mult + base. Before doing it,
                     * check the result cannot exceed hi. `(hi - base) / mult`
                     * is the largest value that is still safe. Writing the
                     * check this way avoids overflow in the check itself. */
                    if (value > (hi - base) / mult)
                    {
                        too_big = 1;
                        break;
                    }
                    value = value * mult + base;
                }

                /* Candidates only grow as `base` grows, so once one is too
                 * big we can leave the loop. */
                if (too_big || value > hi)
                    break;

                if (value >= lo)
                {
                    if (n_found >= MAX_FOUND)
                    {
                        fprintf(stderr, "candidate buffer full\n");
                        exit(1);
                    }
                    found[n_found++] = value;
                }
            }
        }
    }
}

/* qsort needs a function that compares two elements and returns
 * negative / zero / positive for less / equal / greater. */
static int by_value(const void *a, const void *b)
{
    unsigned long long x = *(const unsigned long long *)a;
    unsigned long long y = *(const unsigned long long *)b;

    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

/* Sort the candidates and add each distinct value once. */
static unsigned long long sum_unique(void)
{
    unsigned long long total = 0;
    int i;

    qsort(found, (size_t)n_found, sizeof found[0], by_value);
    for (i = 0; i < n_found; ++i)
    {
        /* Skip a value if the previous one is the same number. */
        if (i > 0 && found[i] == found[i - 1])
            continue;
        total += found[i];
    }
    return total;
}

/* Read the entire file into one malloc'd string and return it.
 * `*len` receives the length. The caller must free() the result. */
static char *read_all(FILE *fp, size_t *len)
{
    size_t cap = 4096;      /* current buffer size in bytes */
    size_t n = 0;
    char *buf = malloc(cap);

    if (!buf)
        return NULL;

    for (;;)
    {
        size_t got;

        /* Make sure there is room for at least one more byte plus a '\0'. */
        if (n + 2 > cap)
        {
            char *bigger;

            cap *= 2;
            bigger = realloc(buf, cap);
            if (!bigger)
            {
                free(buf);
                return NULL;
            }
            buf = bigger;
        }

        got = fread(buf + n, 1, cap - n - 1, fp);
        n += got;
        if (got == 0)
            break;          /* end of file (or a read error; treat as end) */
    }

    buf[n] = '\0';          /* turn the bytes into a C string */
    *len = n;
    return buf;
}

/* Parse "11-22,95-115,..." into the `out` array.
 * Returns the number of ranges, or -1 on bad input. */
static int parse_ranges(char *text, Range *out, int max)
{
    char *p = text;
    int n = 0;

    for (;;)
    {
        unsigned long long lo, hi;

        /* Skip separators and whitespace between ranges. */
        while (*p == ',' || *p == '\n' || *p == '\r' ||
               *p == ' ' || *p == '\t')
            ++p;
        if (*p == '\0')
            break;

        lo = strtoull(p, &p, 10);
        if (*p != '-')
            return -1;
        ++p;
        hi = strtoull(p, &p, 10);

        if (lo > hi || n >= max)
            return -1;
        out[n].lo = lo;
        out[n].hi = hi;
        ++n;
    }
    return n;
}

static void solve(Range *ranges, int n, unsigned long long *part1,
                  unsigned long long *part2)
{
    int i;

    /* Part 1: block repeats exactly twice. */
    n_found = 0;
    for (i = 0; i < n; ++i)
        collect(ranges[i].lo, ranges[i].hi, 2, 2);
    *part1 = sum_unique();

    /* Part 2: block repeats two or more times. */
    n_found = 0;
    for (i = 0; i < n; ++i)
        collect(ranges[i].lo, ranges[i].hi, 2, 0);
    *part2 = sum_unique();
}

/* ---------- animated walk-through (--viz) ------------------------------ */

#define VIZ_W 50
#define VIZ_DELAY_MS 45

/* A cheap stand-in for log10() that needs no maths library: one whole unit
 * per digit, plus a fraction based on where the value sits inside its
 * decade. Good enough to space values from 10 to 10^10 across the screen. */
static double viz_log10(unsigned long long v)
{
    int d;
    unsigned long long p;
    double frac;

    if (v == 0)
        return 0.0;
    d = ndigits(v);
    p = pow10u(d - 1);
    frac = (double)(v - p) / (double)p;
    return (double)(d - 1) + frac / 10.0;
}

static int viz_x(unsigned long long v, unsigned long long maxv)
{
    double top = viz_log10(maxv);
    int x;

    if (top <= 0.0)
        return 0;
    x = (int)(viz_log10(v) / top * (VIZ_W - 1));
    if (x < 0)
        x = 0;
    if (x >= VIZ_W)
        x = VIZ_W - 1;
    return x;
}

/* Draw every input range as a bar on the log axis, then a row of '#' marks
 * for the invalid IDs found so far and the running sum. */
static void draw_gift(const Range *rs, int n, unsigned long long maxv,
                      const char *marks, unsigned long long total,
                      const char *phase)
{
    char bar[VIZ_W + 1];
    int i, j;

    viz_clear();
    printf(VIZ_BOLD "Day 2 - Gift Shop" VIZ_RESET "\n");
    printf(VIZ_CYAN "%s" VIZ_RESET "\n\n", phase);

    for (i = 0; i < n; ++i)
    {
        int a = viz_x(rs[i].lo, maxv);
        int b = viz_x(rs[i].hi, maxv);

        /* Tiny ranges would collapse to one character; widen them to three
         * so every bar is visible. */
        if (b - a < 2)
        {
            b = a + 2;
            if (b >= VIZ_W)
            {
                b = VIZ_W - 1;
                a = b - 2;
            }
        }

        for (j = 0; j < VIZ_W; ++j)
            bar[j] = ' ';
        bar[VIZ_W] = '\0';
        for (j = a; j <= b; ++j)
            bar[j] = '=';
        bar[a] = '[';
        bar[b] = ']';

        printf("%2d  %10llu-%-10llu " VIZ_BLUE "%s" VIZ_RESET "\n",
               i, rs[i].lo, rs[i].hi, bar);
    }

    printf("\n    invalid IDs: " VIZ_GREEN "%s" VIZ_RESET "\n", marks);
    printf("\n    sum so far:  " VIZ_BOLD "%llu" VIZ_RESET "\n", total);
    fflush(stdout);
}

/* Find the candidates, sort them, then reveal them one at a time. */
static int viz(Range *ranges, int n)
{
    unsigned long long maxv = 0;
    unsigned long long total, part1, part2;
    char marks[VIZ_W + 1];
    int i;

    for (i = 0; i < n; ++i)
        if (ranges[i].hi > maxv)
            maxv = ranges[i].hi;

    viz_begin();

    /* Part 1: exactly two repeats. */
    n_found = 0;
    for (i = 0; i < n; ++i)
        collect(ranges[i].lo, ranges[i].hi, 2, 2);
    qsort(found, (size_t)n_found, sizeof found[0], by_value);

    memset(marks, ' ', VIZ_W);
    marks[VIZ_W] = '\0';
    total = 0;
    draw_gift(ranges, n, maxv, marks, total,
              "part 1 - block repeated exactly twice");
    viz_delay(600);

    for (i = 0; i < n_found; ++i)
    {
        if (i > 0 && found[i] == found[i - 1])
            continue;                  /* do not count duplicates twice */
        total += found[i];
        marks[viz_x(found[i], maxv)] = '#';
        draw_gift(ranges, n, maxv, marks, total,
                  "part 1 - block repeated exactly twice");
        viz_delay(VIZ_DELAY_MS);
    }
    part1 = total;
    viz_delay(700);

    /* Part 2: two or more repeats. */
    n_found = 0;
    for (i = 0; i < n; ++i)
        collect(ranges[i].lo, ranges[i].hi, 2, 0);
    qsort(found, (size_t)n_found, sizeof found[0], by_value);

    memset(marks, ' ', VIZ_W);
    total = 0;
    draw_gift(ranges, n, maxv, marks, total,
              "part 2 - block repeated two or more times");
    viz_delay(600);

    for (i = 0; i < n_found; ++i)
    {
        if (i > 0 && found[i] == found[i - 1])
            continue;
        total += found[i];
        marks[viz_x(found[i], maxv)] = '#';
        draw_gift(ranges, n, maxv, marks, total,
                  "part 2 - block repeated two or more times");
        viz_delay(VIZ_DELAY_MS);
    }
    part2 = total;

    printf(VIZ_BOLD "\n  final: part 1 = %llu, part 2 = %llu\n"
           VIZ_RESET, part1, part2);
    viz_end();
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,"
        "1698522-1698528,446443-446449,38593856-38593862,565653-565659,"
        "824824821-824824827,2121212118-2121212124";

    if (argc > 1 && strcmp(argv[1], "--viz") == 0)
    {
        Range ranges[MAX_RANGES];
        int n = parse_ranges((char *)sample, ranges, MAX_RANGES);

        if (n < 0)
        {
            fprintf(stderr, "bad sample\n");
            return 1;
        }
        return viz(ranges, n);
    }

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        Range ranges[MAX_RANGES];
        unsigned long long p1, p2;
        int n = parse_ranges((char *)sample, ranges, MAX_RANGES);

        if (n < 0)
        {
            printf("FAIL: could not parse sample\n");
            return 1;
        }

        solve(ranges, n, &p1, &p2);
        if (p1 != 1227775554ULL || p2 != 4174379265ULL)
        {
            printf("FAIL: expected 1227775554 / 4174379265, "
                   "got %llu / %llu\n", p1, p2);
            return 1;
        }
        printf("sample OK: part1=1227775554 part2=4174379265\n");
        return 0;
    }

    {
        const char *path = argc > 1 ? argv[1] : "input.txt";
        FILE *fp = fopen(path, "r");
        char *text;
        size_t len;
        Range ranges[MAX_RANGES];
        int n;
        unsigned long long p1, p2;

        if (!fp)
        {
            perror(path);
            return 1;
        }
        text = read_all(fp, &len);
        fclose(fp);
        if (!text)
        {
            fprintf(stderr, "out of memory\n");
            return 1;
        }

        n = parse_ranges(text, ranges, MAX_RANGES);
        free(text);
        if (n < 0)
        {
            fprintf(stderr, "bad input\n");
            return 1;
        }

        solve(ranges, n, &p1, &p2);
        printf("ranges: %d\n", n);
        printf("part 1: %llu\n", p1);
        printf("part 2: %llu\n", p2);
    }
    return 0;
}
