# simdprune
Pruning elements in SIMD vectors

Suppose that you are given an vector like 0,1,1,0,3,1,1,4 and you want to remove
all 1s to get 0,0,3,4,... One way to do this is to compare the original vector
with the  vector  1,1,1,1,1,1,1,1 to get the mask 0b01100110 (where a 1 appears if and only if the corresponding elements are equal). We then want to
pass the mask 0b01100110 and the vector 0,1,1,0,3,1,1,4 to some function that
will produce a vector that begins with 0,0,3,4, skipping the 1s.


The AVX-512 instruction sets offer ``vcompress`` instructions for this purpose, but other
instructions sets like SSSE3 or AVX2 provide no help.

That's where this library comes in.

Further documentation: [Quickly pruning elements in SIMD vectors using the simdprune library](http://lemire.me/blog/2017/04/25/quickly-pruning-elements-in-simd-vectors-using-the-simdprune-library/)


## Practical examples

- [Removing duplicates from lists quickly](http://lemire.me/blog/2017/04/10/removing-duplicates-from-lists-quickly/)
- [How quickly can you remove spaces from a string?](http://lemire.me/blog/2017/01/20/how-quickly-can-you-remove-spaces-from-a-string/)

## Usage

To prune every other value:

```
  // 128-bit vectors (SSSE3)
  prune_epi8(x,0b1010101010101010);
  prune_epi16(x,0b10101010);
  prune_epi32(x,0b1010);
  // 256-bit vectors (AVX2)
  prune256_epi32(x,0b10101010);
```
Replacing the various masks by, say, ``0b1`` would prune just the first value.


## How to install

Just copy the header files and include them.  Look at ``demo.c`` for an example.

