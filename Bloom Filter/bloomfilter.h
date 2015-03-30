/*
 * bloomfilter.h
 *
 *  Created on: Mar 24, 2015
 *      Author: vyshnav
 */

#ifndef BLOOMFILTER_H_
#define BLOOMFILTER_H_

// Taken from http://en.literateprograms.org/Bloom_filter_(C).

typedef unsigned int (*hashfunc_t)(const char *);
typedef struct _bloom_filter_t
{
    size_t asize;
    unsigned char *a;
    size_t nfuncs;
    hashfunc_t *funcs;
} bloom_filter_t;

bloom_filter_t *bloom_create(size_t size, size_t nfuncs, ...);
int bloom_destroy(bloom_filter_t *bloom);
int bloom_add(bloom_filter_t *bloom, const char *s);
int bloom_check(bloom_filter_t *bloom, const char *s);

#endif /* BLOOMFILTER_H_ */
