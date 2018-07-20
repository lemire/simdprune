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
 *
 */

// prune bytes, mask should be in [0,1<<16)
// values corresponding to a 1-bit in the mask are removed from output
//
// The table used for this operation occupies 1 MB.
//
// The last value not deleted is used to pad the result.
// trick: by leaving the highest bit (1<<15) to zero whether
// you want to delete the last value or not, you can end up using
// only the first half of the table (which limits cache usage).
__m128i prune_epi8(__m128i x, int mask) {
  return _mm_shuffle_epi8(
      x, _mm_loadu_si128((const __m128i *)mask128_epi8 + mask));
}

static inline __m128i left_shift_bytes(__m128i x, int count) {
  // we'd like to shift by count bytes, but it can't be done directly without immediates
  __m128i p1 = _mm_sll_epi64(x, _mm_cvtsi64_si128(count * 8));
  __m128i p2 = _mm_srl_epi64(_mm_unpacklo_epi64(_mm_setzero_si128(),x), _mm_cvtsi64_si128(64 - count * 8));
  return _mm_or_si128(p1, p2);
}


// prune bytes, mask should be in [0,1<<16)
// values corresponding to a 1-bit in the mask are removed from output
// like thinprune_epi8 but uses a 2kB table.
__m128i thinprune_epi8( __m128i x, int mask) {
      int mask1 = mask & 0xFF;
      int pop = 8 - __builtin_popcount(mask1);
      int mask2 = mask >> 8;
      __m128i m1 = _mm_loadl_epi64((const __m128i *)(thintable_epi8 + mask1));
      __m128i m2 = _mm_loadl_epi64((const __m128i *)(thintable_epi8 + mask2));
      __m128i m2add = _mm_add_epi8(m2,_mm_set1_epi8(8));
      __m128i m2shifted = left_shift_bytes(m2add,pop);
      __m128i shufmask = _mm_or_si128(m2shifted,m1);
      return _mm_shuffle_epi8(
          x, shufmask);
}

// prune 16-bit values, mask should be in [0,1<<8)
// values corresponding to a 1-bit in the mask are removed from output
//
// The table used for this operation occupies 4 kB.
//
// The last value not deleted is used to pad the result.
// trick: by leaving the highest bit (1<<7) to zero whether
// you want to delete the last value or not, you can end up using
// only the first half of the table (which limits cache usage).
__m128i prune_epi16(__m128i x, int mask) {
  return _mm_shuffle_epi8(
      x, _mm_loadu_si128((const __m128i *)mask128_epi16 + mask));
}

// prune 32-bit values, mask should be in [0,1<<4)
// values corresponding to a 1-bit in the mask are removed from output
__m128i prune_epi32(__m128i x, int mask) {
  return _mm_shuffle_epi8(
      x, _mm_loadu_si128((const __m128i *)mask128_epi32 + mask));
}

#endif // __SSE3__

#ifdef __AVX2__
// prune 32-bit values, mask should be in [0,1<<8)
// values corresponding to a 1-bit in the mask are removed from output
__m256i prune256_epi32(__m256i x, int mask) {
  return _mm256_permutevar8x32_epi32(
      x, _mm256_loadu_si256((const __m256i *)mask256_epi32 + mask));
}

// Uses 64bit pdep / pext to save a step in unpacking.
// source:
// http://stackoverflow.com/questions/36932240/avx2-what-is-the-most-efficient-way-to-pack-left-based-on-a-mask
// ***Note that _pdep_u64 is very slow on AMD Ryzen.***
__m256 pext_prune256_epi32(__m256 src, unsigned int mask) {
  uint64_t expanded_mask =
      _pdep_u64(mask, 0x0101010101010101); // unpack each bit to a byte
  expanded_mask *= 0xFF;
  const uint64_t identity_indices = 0x0706050403020100;
  uint64_t wanted_indices = _pext_u64(identity_indices, expanded_mask);
  __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
  __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);
  return _mm256_permutevar8x32_epi32(src,shufmask);
}

#endif //  __AVX2__

#endif
