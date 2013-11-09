import os
from libc.stdint cimport uint64_t
from blooomc cimport bloomfilter, bf_fdopen, bf_close, bf_add, bf_in, bf_mlock, bf_munlock


cdef class Bloomfilter:
    cdef bloomfilter *bf

    def __cinit__(self, filename, uint64_t n, double p):
        cdef bloomfilter *bf
        cdef int fd

        self.bf = NULL;

        fd = os.open(filename, os.O_RDWR | os.O_CREAT, 0644)
        bf = bf_fdopen(fd, n, p)
        os.close(fd)
        if bf == NULL:
            raise Exception(
                "Can't create bloomfilter, "
                "check disk space, file type(should be regular file)"
            )

        self.bf = bf

    def add(self, str data):
        bf_add(self.bf, data, len(data))

    def contains(self, str data):
        return bf_in(self.bf, data, len(data))

    def mlock(self):
        return bf_mlock(self.bf)

    def munlock(self):
        return bf_munlock(self.bf)

    def __dealloc__(self):
        if self.bf:
            bf_close(self.bf)
