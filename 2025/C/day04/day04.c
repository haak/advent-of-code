/* Advent of Code 2025 — Day 4: Printing Department
 *
 * Grid of paper rolls ('@') and empty floor ('.'). A roll can be reached by a
 * forklift when fewer than four of the eight neighbouring cells hold rolls.
 *
 *   part 1: how many rolls are accessible right now?
 *   part 2: remove every accessible roll, repeat until nothing is left to
 *           remove, and count the total removed.
 *
 * Build:  cc -O2 -Wall -Wextra -o day04 day04.c
 * Run:    ./day04            (reads ./input.txt)
 *         ./day04 other.txt
 *         ./day04 --test     (worked example: 13 / 43)
 */

#include <stdio.h>
#include <string.h>

#define MAXDIM 600

static char grid[MAXDIM][MAXDIM];
static int rows;
static int cols;

static int load(FILE *fp)
{
    char line[MAXDIM + 2];
    int r = 0;

    rows = cols = 0;
    while (fgets(line, sizeof line, fp))
    {
        int len;

        line[strcspn(line, "\r\n")] = '\0';
        len = (int)strlen(line);
        if (len == 0)
            continue;
        if (r >= MAXDIM || len >= MAXDIM)
            return -1;
        memcpy(grid[r], line, (size_t)len);
        grid[r][len] = '\0';
        if (len > cols)
            cols = len;
        ++r;
    }
    rows = r;
    return rows > 0 ? 0 : -1;
}

static int neighbours(int r, int c)
{
    int dr, dc;
    int count = 0;

    for (dr = -1; dr <= 1; ++dr)
        for (dc = -1; dc <= 1; ++dc)
        {
            int rr, cc;

            if (dr == 0 && dc == 0)
                continue;
            rr = r + dr;
            cc = c + dc;
            if (rr < 0 || rr >= rows || cc < 0 || cc >= cols)
                continue;
            if (grid[rr][cc] == '@')
                ++count;
        }
    return count;
}

static long count_accessible(void)
{
    long n = 0;
    int r, c;

    for (r = 0; r < rows; ++r)
        for (c = 0; c < cols; ++c)
            if (grid[r][c] == '@' && neighbours(r, c) < 4)
                ++n;
    return n;
}

static long remove_accessible(void)
{
    long total = 0;
    int changed;

    do
    {
        int r, c;

        changed = 0;
        /* Mark first, delete after the pass: removals are simultaneous. */
        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == '@' && neighbours(r, c) < 4)
                {
                    grid[r][c] = 'x';
                    ++changed;
                }

        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == 'x')
                    grid[r][c] = '.';

        total += changed;
    } while (changed > 0);

    return total;
}

static int run(FILE *fp)
{
    long part1, part2;

    if (load(fp) != 0)
        return 1;

    part1 = count_accessible();
    part2 = remove_accessible();

    printf("grid: %dx%d\n", rows, cols);
    printf("part 1: %ld\n", part1);
    printf("part 2: %ld\n", part2);
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "..@@.@@@@.\n"
        "@@@.@.@.@@\n"
        "@@@@@.@.@@\n"
        "@.@@@@..@.\n"
        "@@.@@@@.@@\n"
        ".@@@@@@@.@\n"
        ".@.@.@.@@@\n"
        "@.@@@.@@@@\n"
        ".@@@@@@@@.\n"
        "@.@.@@@.@.\n";

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        FILE *tmp = tmpfile();
        long p1, p2;
        int rc;

        if (!tmp)
            return 1;
        fputs(sample, tmp);
        rewind(tmp);
        rc = load(tmp) == 0 ? 0 : 1;
        if (rc == 0)
        {
            p1 = count_accessible();
            p2 = remove_accessible();
            if (p1 != 13 || p2 != 43)
            {
                printf("FAIL: expected 13 / 43, got %ld / %ld\n", p1, p2);
                rc = 1;
            }
            else
            {
                printf("sample OK: part1=13 part2=43\n");
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
