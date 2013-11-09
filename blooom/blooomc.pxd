from libc.stdint cimport uint64_t


cdef extern from "bf.h":
    ctypedef struct bloomfilter:
        pass

    bloomfilter *bf_fdopen(int fd, uint64_t n, double p)
    void bf_close(bloomfilter *bf)
    void bf_add(bloomfilter *bf, const unsigned char *data, uint64_t size)
    bint bf_in(bloomfilter *bf, const unsigned char *data, uint64_t size)
    bint bf_mlock(bloomfilter *bf)
    bint bf_munlock(bloomfilter *bf)
