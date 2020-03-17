all: clean compile run

run:
	./heat.out lain.pgm conductivity.pgm processed.pgm

clean:
	- rm *.out

compile:
	gcc -fopt-info-vec -O3 -lm -Werror -fopenmp main.c function.c -o heat.out


gif: gif_compile run gif_convert


gif_compile:
	gcc -lm -Werror -fopenmp main.c function.c -o heat.out -D EXPORT_FRAMES

gif_clean:
	- rm frames/*

gif_convert:
	convert frames/* gif.gif
