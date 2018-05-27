benchmark: benchmark.c
	cc -O3 -o benchmark benchmark.c -march=native

test:benchmark
	./benchmark

clean:
	rm -r -f benchmark
