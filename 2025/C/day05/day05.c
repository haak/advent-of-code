/* Advent of Code 2025 — Day 5: Cafeteria
 *
 * Input: fresh ingredient ID ranges ("lo-hi", inclusive), a blank line, then
 * a list of available ingredient IDs.
 *
 *   part 1: how many available IDs fall in at least one range?
 *   part 2: how many IDs are fresh in total (size of the union of ranges)?
 *
 * Ranges are huge (10^14) and overlap, so part 2 sorts and merges them
 * instead of enumerating IDs.
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

typedef struct
{
    unsigned long long lo;
    unsigned long long hi;
} Range;

static Range ranges[MAX_RANGES];
static int n_ranges;
static unsigned long long ids[MAX_IDS];
static int n_ids;

static int load(FILE *fp)
{
    char line[256];
    int in_ids = 0;

    n_ranges = n_ids = 0;
    while (fgets(line, sizeof line, fp))
    {
        unsigned long long lo, hi;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
        {
            in_ids = 1;
            continue;
        }

        if (!in_ids)
        {
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
            ids[n_ids++] = strtoull(line, NULL, 10);
        }
    }
    return n_ranges > 0 ? 0 : -1;
}

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

static long count_fresh_ids(void)
{
    long count = 0;
    int i, j;

    for (i = 0; i < n_ids; ++i)
        for (j = 0; j < n_ranges; ++j)
            if (ids[i] >= ranges[j].lo && ids[i] <= ranges[j].hi)
            {
                ++count;
                break;
            }
    return count;
}

static unsigned long long count_union(void)
{
    unsigned long long total = 0;
    unsigned long long cur_lo, cur_hi;
    int i;

    qsort(ranges, (size_t)n_ranges, sizeof ranges[0], by_start);

    cur_lo = ranges[0].lo;
    cur_hi = ranges[0].hi;
    for (i = 1; i < n_ranges; ++i)
    {
        /* Merge overlapping and adjacent ranges. */
        if (ranges[i].lo <= cur_hi + 1)
        {
            if (ranges[i].hi > cur_hi)
                cur_hi = ranges[i].hi;
        }
        else
        {
            total += cur_hi - cur_lo + 1;
            cur_lo = ranges[i].lo;
            cur_hi = ranges[i].hi;
        }
    }
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
        FILE *tmp = tmpfile();
        long p1;
        unsigned long long p2;
        int rc = 1;

        if (!tmp)
            return 1;
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
