// clang -o demo demo.c -mavx2

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "simdprune.h"
#ifdef __SSE3__

void print8( __m128i x) {
      uint8_t buffer[16];
    _mm_storeu_si128((__m128i*)buffer,x);
    for(int k = 0; k < 16; ++k) printf("%d ", buffer[k]);
}

void print16( __m128i x) {
      uint16_t buffer[8];
    _mm_storeu_si128((__m128i*)buffer,x);
    for(int k = 0; k < 8; ++k) printf("%d ", buffer[k]);
}

void print32( __m128i x) {
      uint32_t buffer[4];
    _mm_storeu_si128((__m128i*)buffer,x);
    for(int k = 0; k < 4; ++k) printf("%d ", buffer[k]);
}

void demosse3() {
  printf("We prune every other value. Result is padded with last value kept.\n");
  printf("8-bit pruning \n");
  __m128i x;
  x = _mm_set_epi8(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
  print8(x);
  printf("\n");
  x = prune_epi8(x,0b1010101010101010);
  print8(x);
  printf("\n");

  printf("16-bit pruning \n");
  x = _mm_set_epi16(7,6,5,4,3,2,1,0);
  print16(x);
  printf("\n");
  x = prune_epi16(x,0b10101010);
  print16(x);
  printf("\n");

  printf("32-bit pruning \n");
  x = _mm_set_epi32(3,2,1,0);
  print32(x);
  printf("\n");
  x = prune_epi32(x,0b1010);
  print32(x);
  printf("\n");
}

#endif // __SSE3__
#ifdef __AVX2__

void print256_32( __m256i x) {
      uint32_t buffer[8];
    _mm256_storeu_si256((__m256i*)buffer,x);
    for(int k = 0; k < 8; ++k) printf("%d ", buffer[k]);
}
// prune 32-bit values, mask should be in [0,1<<8)
void demoavx2() {
  printf("We prune every other value. Result is padded with last value kept.\n");
  printf("32-bit AVX pruning \n");
  __m256i x;
  x = _mm256_set_epi32(7,6,5,4,3,2,1,0);
  print256_32(x);
  printf("\n");
  x = prune256_epi32(x,0b10101010);
  print256_32(x);
  printf("\n");
}

#endif


int main() {
#ifdef __SSE3__
  demosse3();
#endif
#ifdef __AVX2__
  demoavx2();
#endif
}
