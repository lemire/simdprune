// gcc -o benchmark benchmark.c  -mbmi2 -mavx2 -O3 && ./benchmark
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "simdprune.h"


#define RDTSC_START(cycles)                                             \
    do {                                                                \
        uint32_t cyc_high, cyc_low;                                     \
        __asm volatile("cpuid\n"                                        \
                       "rdtsc\n"                                        \
                       "mov %%edx, %0\n"                                \
                       "mov %%eax, %1" :                                \
                       "=r" (cyc_high),                                 \
                       "=r"(cyc_low) :                                  \
                       : /* no read only */                             \
                       "%rax", "%rbx", "%rcx", "%rdx" /* clobbers */    \
                       );                                               \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                \
    } while (0)

#define RDTSC_STOP(cycles)                                              \
    do {                                                                \
        uint32_t cyc_high, cyc_low;                                     \
        __asm volatile("rdtscp\n"                                       \
                       "mov %%edx, %0\n"                                \
                       "mov %%eax, %1\n"                                \
                       "cpuid" :                                        \
                       "=r"(cyc_high),                                  \
                       "=r"(cyc_low) :                                  \
                       /* no read only registers */ :                   \
                       "%rax", "%rbx", "%rcx", "%rdx" /* clobbers */    \
                       );                                               \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                \
    } while (0)

static __attribute__ ((noinline))
uint64_t rdtsc_overhead_func(uint64_t dummy) {
    return dummy;
}

uint64_t global_rdtsc_overhead = (uint64_t) UINT64_MAX;

#define RDTSC_SET_OVERHEAD(test, repeat)			      \
  do {								      \
    uint64_t cycles_start, cycles_final, cycles_diff;		      \
    uint64_t min_diff = UINT64_MAX;				      \
    for (int i = 0; i < repeat; i++) {			      \
      __asm volatile("" ::: /* pretend to clobber */ "memory");	      \
      RDTSC_START(cycles_start);				      \
      test;							      \
      RDTSC_STOP(cycles_final);                                       \
      cycles_diff = (cycles_final - cycles_start);		      \
      if (cycles_diff < min_diff) min_diff = cycles_diff;	      \
    }								      \
    global_rdtsc_overhead = min_diff;				      \
    printf("rdtsc_overhead set to %d\n", (int)global_rdtsc_overhead);     \
  } while (0)							      \


/*
 * Prints the best number of operations per cycle where
 * test is the function call, answer is the expected answer generated by
 * test, repeat is the number of times we should repeat and size is the
 * number of operations represented by test.
 */
#define BEST_TIME(test, expected, pre, repeat, size, verbose)                         \
        do {                                                              \
            if (global_rdtsc_overhead == UINT64_MAX) {                    \
               RDTSC_SET_OVERHEAD(rdtsc_overhead_func(1), repeat);        \
            }                                                             \
            if(verbose) printf("%-60s\t: ", #test);                                        \
            fflush(NULL);                                                 \
            uint64_t cycles_start, cycles_final, cycles_diff;             \
            uint64_t min_diff = (uint64_t)-1;                             \
            uint64_t sum_diff = 0;                                        \
            for (int i = 0; i < repeat; i++) {                            \
                pre;                                                      \
                __asm volatile("" ::: /* pretend to clobber */ "memory"); \
                RDTSC_START(cycles_start);                                \
                if(test != expected) {printf("not expected (%d , %d )",(int)test,(int)expected);break;}                     \
                RDTSC_STOP(cycles_final);                                \
                cycles_diff = (cycles_final - cycles_start - global_rdtsc_overhead);           \
                if (cycles_diff < min_diff) min_diff = cycles_diff;       \
                sum_diff += cycles_diff;                                  \
            }                                                             \
            uint64_t S = size;                                            \
            float cycle_per_op = (min_diff) / (double)S;                  \
            float avg_cycle_per_op = (sum_diff) / ((double)S * repeat);   \
            if(verbose) printf(" %.3f cycles per operation (best) ", cycle_per_op);   \
            if(verbose) printf("\t%.3f cycles per operation (avg) ", avg_cycle_per_op);   \
            if(verbose) printf("\n");                                                 \
            if(!verbose) printf(" %.3f ",cycle_per_op);                   \
            fflush(NULL);                                                 \
 } while (0)

// like BEST_TIME, but no check
#define BEST_TIME_NOCHECK(test, pre, repeat, size, verbose)                         \
        do {                                                              \
            if (global_rdtsc_overhead == UINT64_MAX) {                    \
               RDTSC_SET_OVERHEAD(rdtsc_overhead_func(1), repeat);        \
            }                                                             \
            if(verbose) printf("%-60s\t: ", #test);                                        \
            fflush(NULL);                                                 \
            uint64_t cycles_start, cycles_final, cycles_diff;             \
            uint64_t min_diff = (uint64_t)-1;                             \
            uint64_t sum_diff = 0;                                        \
            for (int i = 0; i < repeat; i++) {                            \
                pre;                                                      \
                __asm volatile("" ::: /* pretend to clobber */ "memory"); \
                RDTSC_START(cycles_start);                                \
                test;                     \
                RDTSC_STOP(cycles_final);                                \
                cycles_diff = (cycles_final - cycles_start - global_rdtsc_overhead);           \
                if (cycles_diff < min_diff) min_diff = cycles_diff;       \
                sum_diff += cycles_diff;                                  \
            }                                                             \
            uint64_t S = size;                                            \
            float cycle_per_op = (min_diff) / (double)S;                  \
            float avg_cycle_per_op = (sum_diff) / ((double)S * repeat);   \
            if(verbose) printf(" %.3f cycles per operation (best) ", cycle_per_op);   \
            if(verbose) printf("\t%.3f cycles per operation (avg) ", avg_cycle_per_op);   \
            if(verbose) printf("\n");                                                 \
            if(!verbose) printf(" %.3f ",cycle_per_op);                   \
            fflush(NULL);                                                 \
 } while (0)


__attribute__ ((noinline))
void randomize(int * bitmasks, int N, int mask) {
  for (int k = 0; k < N; k++) {
    bitmasks[k] = rand() & mask;
  }
}

__attribute__ ((noinline))
__m128i runprune_epi8(int * bitmasks, int N, __m128i *x) {
  for (int k = 0; k < N; k++) {
    *x = prune_epi8(*x, bitmasks[k]);
  }
  return *x;
}

__attribute__ ((noinline))
__m128i runthinprune_epi8(int * bitmasks, int N, __m128i *x) {
  for (int k = 0; k < N; k++) {
    *x = thinprune_epi8(*x, bitmasks[k]);
  }
  return *x;
}
__attribute__ ((noinline))
__m128i runprune_epi16(int * bitmasks, int N, __m128i *x) {
  for (int k = 0; k < N; k++) {
    *x = prune_epi16(*x, bitmasks[k]);
  }
  return *x;
}

__attribute__ ((noinline))
__m128i runprune_epi32(int * bitmasks, int N, __m128i *x) {
  for (int k = 0; k < N; k++) {
    *x = prune_epi32(*x, bitmasks[k]);
  }
  return *x;
}

__attribute__ ((noinline))
__m256i runprune256_epi32(int * bitmasks, int N, __m256i *x) {
  for (int k = 0; k < N; k++) {
    *x = prune256_epi32(*x, bitmasks[k]);
  }
  return *x;
}

__m256i runpext_prune256_epi32(int * bitmasks, int N,  __m256i *x) {
  for (int k = 0; k < N; k++) {
    *x = pext_prune256_epi32(*x, bitmasks[k]);
  }
  return *x;
}



int main() {
  printf("This test measures the latency in CPU cycles.\n");
  const int N = 2048;
  int * bitmasks = malloc(sizeof(int) * N);
  const int repeat = 500;
  __m128i x = _mm_set_epi8(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
  BEST_TIME_NOCHECK(runprune_epi8(bitmasks, N, &x), randomize(bitmasks, N, (1<<16)-1), repeat, N, true);
  BEST_TIME_NOCHECK(runthinprune_epi8(bitmasks, N, &x), randomize(bitmasks, N, (1<<16)-1), repeat, N, true);
  BEST_TIME_NOCHECK(runprune_epi16(bitmasks, N, &x), randomize(bitmasks, N, (1<<8)-1), repeat, N, true);
  BEST_TIME_NOCHECK(runprune_epi32(bitmasks, N, &x), randomize(bitmasks, N, (1<<4)-1), repeat, N, true);
  __m256i xx = _mm256_set_epi32(7,6,5,4,3,2,1,0);
  BEST_TIME_NOCHECK(runprune256_epi32(bitmasks, N, &xx), randomize(bitmasks, N, (1<<8)-1), repeat, N, true);
  BEST_TIME_NOCHECK(runpext_prune256_epi32(bitmasks, N, &xx), randomize(bitmasks, N, (1<<8)-1), repeat, N, true);
  free(bitmasks);
  return _mm_extract_epi8(x,0) + _mm256_extract_epi8(xx,0);
}
