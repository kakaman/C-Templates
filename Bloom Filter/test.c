/*
 * test.c
 *
 *  Created on: Mar 24, 2015
 *      Author: vyshnav
 */

#include <stdio.h>
#include <string.h>
#include <error.h>

#include"bloomfilter.h"

// Taken from http://en.literateprograms.org/Bloom_filter_(C).
unsigned int sax_hash(const char *key)
{
    unsigned int h = 0;

    while (*key)
        h ^= (h << 5) + (h >> 2) + (unsigned char) *key++;

    return h;
}

unsigned int sdbm_hash(const char *key)
{
    unsigned int h = 0;
    while (*key)
        h = (unsigned char) *key++ + (h << 6) + (h << 16) - h;
    return h;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[1024];
    char *p;
    bloom_filter_t *bloom_filter_t;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR: No word file specified\n");
        return -1;
    }

    if (!(bloom_filter_t = bloom_create(2500000, 2, sax_hash, sdbm_hash)))
    {
        fprintf(stderr, "ERROR: Could not create bloom_filter_t filter\n");
        return -1;
    }

    if (!(fp = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
        return -1;
    }

    while (fgets(line, 1024, fp))
    {
        if ((p = strchr(line, '\r')))
            *p = '\0';
        if ((p = strchr(line, '\n')))
            *p = '\0';

        bloom_add(bloom_filter_t, line);
    }

    fclose(fp);

    while (fgets(line, 1024, stdin))
    {
        if ((p = strchr(line, '\r')))
            *p = '\0';
        if ((p = strchr(line, '\n')))
            *p = '\0';

        p = strtok(line, " \t,.;:\r\n?!-/()");
        while (p)
        {
            if (!bloom_check(bloom_filter_t, p))
            {
                printf("No match for word \"%s\"\n", p);
            }
            p = strtok(NULL, " \t,.;:\r\n?!-/()");
        }
    }

    bloom_destroy(bloom_filter_t);

    return 0;
}
