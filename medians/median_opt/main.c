#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "histogram.h"


void swap(float *T, int i, int j) {
  float tmp = T[i];
  T[i] = T[j];
  T[j] = tmp;
}

void tri(float *T, int n) {

  int j;

  for (int i = 1; i < n; ++i) {
    int k = -1;

    for (j = 0; j < i; ++j)
      if (T[j] >= T[i]) {
        k = j;
        break;
      }

    if (k >= 0) {
      for (j = i; j > k; --j) {
        swap(T, j - 1, j);
      }
    }
  }
}

void median_naive(float *input, float *output, int radius, int len) {
  int ksize = 2 * radius + 1;
  float histogram[ksize];

  // on met les bords à 0 car y traîter la médiane ne fait pas grand sens
  for (int i = 0; i < radius; ++i)
    output[i] = 0.;
  for (int i = len - radius; i < len; ++i)
    output[i] = 0.;

  for (int i = radius; i + radius < len; ++i) {
    for (int j = 0; j < ksize; ++j) {
      histogram[j] = input[j + i - radius];
    }

    tri(histogram, ksize);

    output[i] = histogram[radius];
  }
}

void median_opt(float *input, float *output, int radius, int len)
{
  int ksize = 2 * radius + 1;
  Histogram histo = histo_new(ksize, 50, 0., 51.);

  //init the histogram
  for (int i = 0; i < ksize; ++i)
  {
    histo_push(&histo, input[i]);
  }
  
  int i;
  
    for (i = radius; i < len-radius; ++i) {
      histo_push(&histo, input[i + radius]);
      output[i] = histo_smart_median(&histo);
    }

    
  histo_free(&histo);
}

// calls median_opt on several threads
void median_opt_par(float *input, float *output, int radius, int len, int chunk_size)
{

  int start = 0;
#pragma omp parallel for private(start)
  for (start = 0; start < len - chunk_size; start+=(chunk_size-radius*2))
  {
      median_opt(input+start, output+start, radius, chunk_size);
  }
  // unfinished part at the end (neglectable)
  median_opt(input+start, output+start, radius, len-start);
  
}


void print_result(float **arrays, int len, int n_arrays) {
  for (int i = 0; i < len; ++i) {
      for (int j = 0; j < n_arrays; ++j)
	  printf("%.1lf | ", arrays[j][i]);
      printf("\n");
      
  }
}

int main() {
    printf("MARCO\n");
    
  srand(omp_get_wtime());

  int len = 100000;
  int radius = 50;
  int chunk_size = 1024;
  //typedef double aligned_double __attribute__((aligned (16)));
  float *input = malloc(sizeof(float) * len);  
  float *output = malloc(sizeof(float) * len);
  float *output2 = malloc(sizeof(float) * len);
  float *output3 = malloc(sizeof(float) * len);
  float pi = 3.14;
//A = cos(2*pi*0.05*x+2*pi*rand) + 0.5*randn(1,100);
  for (int x = 0; x < len; ++x)
      input[x] = cos(2*pi*0.05*x*2*pi*rand()) + 0.5*((float) ((((int)rand()) % 100)));
  


  double t0, t1, t2, t3;
  double t_naive, t_opt, t_par;

  t0 = omp_get_wtime();
  median_naive(input, output, radius, len);
  t1 = omp_get_wtime();
  t_naive = t1 - t0;
  printf("naive: %lf \n", t_naive);

  for (int n = 1; n < N_THREADS; ++n) {
      omp_set_num_threads(n);      
    t1 = omp_get_wtime();
    median_opt(input, output2, radius, len);
      printf("median_opt OK\n");
    t2 = omp_get_wtime();
    if (chunk_size) median_opt_par(input, output3, radius, len, chunk_size);
    t3 = omp_get_wtime();
    t_opt = t2 - t1;
    t_par = t3 - t2;
    printf("opti(%d THREADS) : %lf -> %lf -> %lf   \n", n, t_naive, t_opt, t_par);
    printf("        speedup (naif->opti): %lf \n", t_naive / t_opt);
    printf("        speedup (naif->opti_par): %lf \n", t_naive/t_par);
    printf("        speedup (opti->opti_par): %lf \n", t_opt/t_par);
  }

  float ** arrays = malloc(8*sizeof(float*));
  
  arrays[0] = input;
  arrays[1] = output;
  arrays[2] = output2;
  arrays[3] = output3;
  
      
  //print_result(arrays, len, 4);

  free(input);
  free(output);
  free(output2);
  free(output3);
  free(arrays);
  
  return 0;
}
