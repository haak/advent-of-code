/* Advent of Code 2025 — Day 5: Cafeteria
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * The input has two sections separated by a blank line:
 *
 *     3-5
 *     10-14
 *     ...
 *     <blank line>
 *     1
 *     5
 *     ...
 *
 * The first section lists ranges of "fresh" ingredient IDs: "3-5" means
 * IDs 3, 4 and 5 are all fresh. The second section lists the ingredient IDs
 * that are actually available.
 *
 *   part 1: how many of the available IDs fall in at least one range?
 *   part 2: how many IDs are fresh in total? That is the size of the UNION
 *           of all the ranges — each ID counted once even if several ranges
 *           cover it.
 *
 * Worked example (--test): 3 and 14.
 *
 * THE MAIN IDEA
 * -------------
 * Part 1 is a simple lookup: for each available ID, check the ranges.
 *
 * Part 2 must not loop over every ID: the ranges reach about 5 * 10^14, so
 * counting one by one would take days. Instead, sort the ranges by their
 * start, then walk through them merging any that overlap or touch. The
 * merged ranges are all disjoint, so the answer is just the sum of their
 * lengths (hi - lo + 1).
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - `typedef struct { ... } Range;` creates a new type that bundles two
 *     numbers. `ranges[i].lo` reads a field.
 *   - `qsort` sorts an array using a comparison function we supply. The
 *     function receives `const void *` pointers, which we cast back to
 *     `Range *`.
 *   - `sscanf(line, "%llu-%llu", &lo, &hi)` parses two unsigned long long
 *     values out of text; `strtoull` would work too.
 *   - `FILE *`, `fgets`, `tmpfile` and `rewind` work as in the other days.
 *   - Integer types matter: an `unsigned long long` holds up to about
 *     1.8 * 10^19, which covers these IDs; a 32-bit `int` would not.
 *
 * Build:  cc -O2 -Wall -Wextra -o day05 day05.c
 * Run:    ./day05            (reads ./input.txt)
 *         ./day05 other.txt
 *         ./day05 --test     (worked example: 3 / 14)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RANGES 4096
#define MAX_IDS 100000

/* A fresh-ID range: every number from lo to hi, inclusive. */
typedef struct
{
    unsigned long long lo;
    unsigned long long hi;
} Range;

static Range ranges[MAX_RANGES];
static int n_ranges;
static unsigned long long ids[MAX_IDS];
static int n_ids;

/* Read both sections. Returns 0 on success, -1 on bad input. */
static int load(FILE *fp)
{
    char line[256];
    int in_ids = 0;      /* 0 = reading ranges, 1 = reading IDs */

    n_ranges = n_ids = 0;
    while (fgets(line, sizeof line, fp))
    {
        unsigned long long lo, hi;

        /* Cut off the trailing newline. */
        line[strcspn(line, "\r\n")] = '\0';

        /* A blank line ends the range section and starts the ID section. */
        if (line[0] == '\0')
        {
            in_ids = 1;
            continue;
        }

        if (!in_ids)
        {
            /* "%llu-%llu" means: an unsigned long long, a literal '-', and
             * another unsigned long long. sscanf returns how many values it
             * managed to read. */
            if (sscanf(line, "%llu-%llu", &lo, &hi) != 2)
                return -1;
            if (lo > hi || n_ranges >= MAX_RANGES)
                return -1;
            ranges[n_ranges].lo = lo;
            ranges[n_ranges].hi = hi;
            ++n_ranges;
        }
        else
        {
            if (n_ids >= MAX_IDS)
                return -1;
            /* strtoull parses a decimal number; NULL means "we do not care
             * where parsing stopped". */
            ids[n_ids++] = strtoull(line, NULL, 10);
        }
    }
    return n_ranges > 0 ? 0 : -1;
}

/* qsort comparison for ranges: order by start, lowest first.
 * Returning negative / zero / positive means a < b / a == b / a > b. */
static int by_start(const void *a, const void *b)
{
    const Range *ra = a;
    const Range *rb = b;

    if (ra->lo < rb->lo)
        return -1;
    if (ra->lo > rb->lo)
        return 1;
    return 0;
}

/* Part 1: count IDs that are inside at least one range. */
static long count_fresh_ids(void)
{
    long count = 0;
    int i, j;

    for (i = 0; i < n_ids; ++i)
        for (j = 0; j < n_ranges; ++j)
            if (ids[i] >= ranges[j].lo && ids[i] <= ranges[j].hi)
            {
                ++count;      /* count this ID once */
                break;        /* no need to look at further ranges */
            }
    return count;
}

/* Part 2: total size of the union of all ranges. */
static unsigned long long count_union(void)
{
    unsigned long long total = 0;
    unsigned long long cur_lo, cur_hi;
    int i;

    /* Sorting by start means a range can only overlap the current merged
     * block, never one we have already finished. */
    qsort(ranges, (size_t)n_ranges, sizeof ranges[0], by_start);

    cur_lo = ranges[0].lo;
    cur_hi = ranges[0].hi;

    for (i = 1; i < n_ranges; ++i)
    {
        /* Merge overlapping AND adjacent ranges: 3-5 followed by 6-8
         * covers 3..8 with no gap, so it is one block.
         * `cur_hi + 1` is safe because the IDs are far below the maximum
         * value of unsigned long long, so this cannot wrap around. */
        if (ranges[i].lo <= cur_hi + 1)
        {
            if (ranges[i].hi > cur_hi)
                cur_hi = ranges[i].hi;
        }
        else
        {
            /* The next range starts after a genuine gap: close the current
             * block and start a new one. hi - lo + 1 counts inclusive IDs. */
            total += cur_hi - cur_lo + 1;
            cur_lo = ranges[i].lo;
            cur_hi = ranges[i].hi;
        }
    }
    /* The last block is closed after the loop. */
    total += cur_hi - cur_lo + 1;
    return total;
}

static int run(FILE *fp)
{
    if (load(fp) != 0)
        return 1;
    printf("ranges: %d  ids: %d\n", n_ranges, n_ids);
    printf("part 1: %ld\n", count_fresh_ids());
    printf("part 2: %llu\n", count_union());
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "3-5\n"
        "10-14\n"
        "16-20\n"
        "12-18\n"
        "\n"
        "1\n"
        "5\n"
        "8\n"
        "11\n"
        "17\n"
        "32\n";

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        /* Feed the sample through a temporary file so load() runs too. */
        FILE *tmp = tmpfile();
        long p1;
        unsigned long long p2;
        int rc = 1;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
        fputs(sample, tmp);
        rewind(tmp);
        if (load(tmp) == 0)
        {
            p1 = count_fresh_ids();
            p2 = count_union();
            if (p1 == 3 && p2 == 14)
            {
                printf("sample OK: part1=3 part2=14\n");
                rc = 0;
            }
            else
            {
                printf("FAIL: expected 3 / 14, got %ld / %llu\n", p1, p2);
            }
        }
        fclose(tmp);
        return rc;
    }

    {
        const char *path = argc > 1 ? argv[1] : "input.txt";
        FILE *fp = fopen(path, "r");
        int rc;

        if (!fp)
        {
            perror(path);
            return 1;
        }
        rc = run(fp);
        fclose(fp);
        return rc;
    }
}
