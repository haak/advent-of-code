/* Shared helpers for the --viz terminal animations (2025 C solutions).
 *
 * The visualisations are drawn by the C programs themselves using ANSI
 * escape sequences, so there is no JavaScript or other second
 * implementation of the puzzle logic: the animation walks the very same
 * arrays the solver uses.
 *
 * Everything here is `static inline` so each day remains a single
 * self-contained translation unit and -Wall -Wextra stays quiet.
 *
 * Escape sequence refresher:
 *   ESC [ 2 J      clear the screen
 *   ESC [ H        move the cursor to the top-left corner
 *   ESC [ ? 25 l   hide the cursor      (l = low)
 *   ESC [ ? 25 h   show the cursor      (h = high)
 *   ESC [ 3 1 m    set a colour; ESC [ 0 m resets all attributes
 *
 * Set the environment variable VIZ_FAST=1 to skip the sleeps (useful for
 * tests, screen recordings, or when you are in a hurry).
 */

#ifndef AOC_2025_VIZ_H
#define AOC_2025_VIZ_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VIZ_RESET "\033[0m"
#define VIZ_BOLD "\033[1m"
#define VIZ_DIM "\033[2m"
#define VIZ_RED "\033[31m"
#define VIZ_GREEN "\033[32m"
#define VIZ_YELLOW "\033[33m"
#define VIZ_BLUE "\033[34m"
#define VIZ_MAGENTA "\033[35m"
#define VIZ_CYAN "\033[36m"
#define VIZ_INVERSE "\033[7m"

/* Hide the cursor so the animation does not flicker. */
static inline void viz_begin(void)
{
    fputs("\033[?25l", stdout);
}

/* Restore colours and the cursor on the way out. */
static inline void viz_end(void)
{
    fputs(VIZ_RESET "\033[?25h\n", stdout);
    fflush(stdout);
}

/* Clear the screen and park the cursor in the top-left corner. */
static inline void viz_clear(void)
{
    fputs("\033[2J\033[H", stdout);
}

/* True when VIZ_FAST is set to a non-empty value. */
static inline int viz_fast(void)
{
    const char *v = getenv("VIZ_FAST");

    return v != NULL && v[0] != '\0';
}

/* Sleep for `ms` milliseconds, unless VIZ_FAST is set. */
static inline void viz_delay(int ms)
{
    struct timespec ts;

    if (viz_fast())
        return;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

#endif
