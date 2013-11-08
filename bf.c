#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "bf.h"
#include "siphash.h"

const char *SIPHASH_KEY = "Proton is awesom";


bloomfilter *bf_open(const char *filename, uint64_t n, double p)
{
    uint64_t m, size;
    struct stat st;
    int k, fd, ret;
    uint8_t *data;
    bloomfilter *bf;

    m = (uint64_t)ceil((n * log(p)) / log(1.0 / (pow(2.0, log(2.0)))));
    k = (int)round(log(2.0) * m / n);

    if(m & 0xFFFFF) {
        m = m | 0xFFFFF + 1;
    }

    fd = open(filename, O_RDWR | O_CREAT);
    if(fd == -1) return 0;
    if(fstat(fd, &st)) {
        close(fd);
        return NULL;
    }

    size = st.st_size;

    if(size & 0xFFF || st.st_size < (m >> 8)) {
        size = m >> 8;
        if(ftruncate(fd, size)) {
            close(fd);
            return NULL;
        }
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) {
        close(fd);
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
    close(bf->fd);
    free(bf);
}


void bf_add(bloomfilter *bf, const char *data, uint64_t size)
{
    int i;
    uint64_t hash;

    char mydata[size];
    memcpy(mydata, data, size);

    for(i=0; i<bf->k; i++) {
        hash = siphash(SIPHASH_KEY, mydata, size);
        bf->data[hash>>8] |= 1 << (hash & 7);
        mydata[0]++;
    }
}


int bf_in(bloomfilter *bf, const char *data, uint64_t size)
{
    int i;
    uint64_t hash;

    char mydata[size];
    memcpy(mydata, data, size);

    for(i=0; i<bf->k; i++) {
        hash = siphash(SIPHASH_KEY, mydata, size);
        if(!(bf->data[hash>>8] & (1 << (hash & 7)))) {
            return 0;
        }
        mydata[0]++;
    }
    return 1;
}
