#ifndef LIB_H
#define LIB_H

#define NULL (void*)0
typedef unsigned long size_t;

void *memset(void *b, int c, size_t len);
void *memcpy(void *dst, const void *src, size_t n);

void *mem_alloc(size_t size);
void mem_free(void *addr);

#endif
