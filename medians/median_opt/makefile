all: clean compile compile_O3 run run_O3
clean:
	- rm *.out

compile:
	gcc -lm -Werror -fopenmp *.c -o median.out -D N_THREADS=16
compile_O3:
	gcc -O3 --opt-info-vec -lm -Werror -fopenmp *.c -o median_O3.out -D N_THREADS=16

run:
	./median.out
run_O3:
	./median_O3.out
all_threads:
	for number in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do \
		gcc -O3 --opt-info-vec -lm -Werror -fopenmp main.c -o median.out -D N_THREADS=$$number && echo "N_THREADS =" $$number && ./median.out && echo "" ; \
	done
