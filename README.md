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

## On processors benefiting from advanced AVX-512 instructions

The AVX-512 instruction set ``vcompress`` helps but is limited to 32-bit and 64-bit words. 

## On ARM processors

Some ARM processors will benefit from Scalable Vector Extensions. It seems that a sequence of SPLICE and INCP instructions can effectively achieve arbitrary prunning. ARM Scalable Vector Extensions also support a related instruction (``compact``) but it also seems to be limited to 32-bit and 64-bit words. 

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

## How fast is it?

The throughput of these functions is likely quite good. The latency spans several cycles, however. Especially expensive is
``prune_epi8`` due to its large table.

These numbers assume that one is able to hide the cache/RAM latency by prefetching the bit masks. Table lookups from RAM
take dozens of cycles at least.

```
  gcc -o benchmark benchmark.c  -mbmi2 -mavx2 -O3 && ./benchmark
This test measures the latency in CPU cycles.
rdtsc_overhead set to 30
runprune_epi8(bitmasks, N, &x)                                  :  4.173 cycles per operation (best)     4.581 cycles per operation (avg)
runthinprune_epi8(bitmasks, N, &x)                              :  10.129 cycles per operation (best)     10.166 cycles per operation (avg)
runprune_epi16(bitmasks, N, &x)                                 :  2.440 cycles per operation (best)     2.451 cycles per operation (avg)
runprune_epi32(bitmasks, N, &x)                                 :  2.368 cycles per operation (best)     2.446 cycles per operation (avg)
runprune256_epi32(bitmasks, N, &xx)                             :  3.125 cycles per operation (best)     3.145 cycles per operation (avg)
runpext_prune256_epi32(bitmasks, N, &xx)                        :  4.123 cycles per operation (best)     4.134 cycles per operation (avg)
```

Why is ``runthinprune_epi8`` so much slower than ``runprune_epi8``? In part because it uses a tiny lookup table and trades reduce memory
usage for much lower speed.

## How to install

Just copy the header files and include them.  Look at ``demo.c`` for an example.

