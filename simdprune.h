/**
* (c) Daniel Lemire
* License: Apache License 2.0
*/


#ifndef SIMDPRUNE_H
#define SIMDPRUNE_H

#include "simdprune_tables.h"


#ifdef __SSE3__
/*
* The mask "marks" values in 'x' for deletion. So if
* 'x' is odd, then the first value is marked for deletion.
* This function produces a new vector that start with all
* values that have not been deleted.
*
* Passing a mask of 0 would simply copy the provided vector.
*/

// prune bytes, mask should be in [0,1<<16)
// values corresponding to a 1-bit in the mask are removed from output
__m128i prune_epi8( __m128i x, int mask) {
      return _mm_shuffle_epi8(
          x, _mm_loadu_si128((const __m128i *)mask128_epi8 + mask));
}

// prune 16-bit values, mask should be in [0,1<<8)
// values corresponding to a 1-bit in the mask are removed from output
__m128i prune_epi16( __m128i x, int mask) {
      return _mm_shuffle_epi8(
          x, _mm_loadu_si128((const __m128i *)mask128_epi16 + mask));
}

// prune 32-bit values, mask should be in [0,1<<4)
// values corresponding to a 1-bit in the mask are removed from output
__m128i prune_epi32( __m128i x, int mask) {
      return _mm_shuffle_epi8(
          x, _mm_loadu_si128((const __m128i *)mask128_epi32 + mask));
}

#endif // __SSE3__

#ifdef __AVX2__
// prune 32-bit values, mask should be in [0,1<<8)
// values corresponding to a 1-bit in the mask are removed from output
__m256i prune256_epi32( __m256i x, int mask) {
    return _mm256_permutevar8x32_epi32(x,_mm256_loadu_si256((const __m256i *)mask256_epi32 + mask));
}
#endif //  __AVX2__

#endif
