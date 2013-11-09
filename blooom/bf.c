#include <malloc.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "bf.h"
#include "siphash.h"

const char *SIPHASH_KEY = (
    "There is a place called Gensokyo, "
    "where lives over 100 girls. "
    "Youmu is the best of all. "
    "Yuyuko can eat much. "
    "Satori can do mind reading, except Koishi's."
    "Ran and Eirin have first class intelligence, "
    "while Cirno is an idiot."
    "If you pay Reimu 100000 yen, "
    "you can do anything you want to her."
    "A granny always claims her age as 17, "
    "her name is Yukar#^dff56&dsg&#$%@%"
);


bloomfilter *bf_fdopen(int fd, uint64_t n, double p)
{
    uint64_t m, size;
    struct stat st;
    int k;
    uint8_t *data;
    bloomfilter *bf;

    m = (uint64_t)ceil((n * log(p)) / log(1.0 / (pow(2.0, log(2.0)))));
    k = (int)round(log(2.0) * m / n);

    if(m & 0xFFFFF) {
        m = (m | 0xFFFFF) + 1;
    }

    if(fstat(fd, &st)) {
        return NULL;
    }

    size = st.st_size;

    if(size & 0xFFF || st.st_size < (m >> 8)) {
        size = m >> 8;
        if(ftruncate(fd, size)) {
            return NULL;
        }
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) {
        return NULL;
    }

    bf = malloc(sizeof(bloomfilter));
    bf->m = m;
    bf->k = k;
    bf->fd = fd;
    bf->mapsize = size;
    bf->data = data;

    return bf;
}


void bf_close(bloomfilter *bf)
{
    munmap(bf->data, bf->mapsize);
    memset(bf, 0, sizeof(bloomfilter));
    free(bf);
}


void bf_add(bloomfilter *bf, const unsigned char *data, uint64_t size)
{
    int i;
    uint64_t hash;
    const unsigned char *pkey = (const unsigned char *)SIPHASH_KEY;

    for(i=0; i<bf->k; i++) {
        hash = siphash(pkey, data, size);
        bf->data[((hash>>8) % bf->m) >> 8] |= (1 << (hash & 7));
        pkey++;
    }
}


int bf_in(bloomfilter *bf, const unsigned char *data, uint64_t size)
{
    int i;
    uint64_t hash;
    const unsigned char *pkey = (const unsigned char *)SIPHASH_KEY;

    for(i=0; i<bf->k; i++) {
        hash = siphash(pkey, data, size);
        if(!(bf->data[((hash>>8) % bf->m) >> 8] & (1 << (hash & 7)))) {
            return 0;
        }
        pkey++;
    }
    return 1;
}
