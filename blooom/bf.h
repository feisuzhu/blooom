#ifndef BF_H
#define BF_H
#include <stdint.h>

typedef struct _bloomfilter {
    uint64_t m;  // in bits
    int k;  // number of hash functions
    int fd;
    uint64_t mapsize;
    uint8_t *data;  // bloom filter bits
} bloomfilter;

bloomfilter *bf_fdopen(int fd, uint64_t n, double p);
void bf_close(bloomfilter *bf);
void bf_add(bloomfilter *bf, const unsigned char *data, uint64_t size);
int bf_in(bloomfilter *bf, const unsigned char *data, uint64_t size);

#endif // BF_H
