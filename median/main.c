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
  Histogram histo = histo_new(ksize, 50, 0., 50.);

  // on met les bords à 0 car y traîter la médiane ne fait pas grand sens
  for (int i = 0; i < radius; ++i)
    output[i] = 0.;
  for (int i = len - radius; i < len; ++i)
    output[i] = 0.;

  //init the histogram
  for (int i = 0; i < ksize; ++i)
  {
    histo_push(&histo, input[i]);
  }
  
  int i;
  
  for (i = radius; i < len-radius; ++i) {
      output[i] = histo_smart_median(&histo);
      histo_push(&histo, input[i + radius]);
  }

  histo_free(&histo);
}

void print_result(float *input, float *output, int len) {
  for (int i = 0; i < len; ++i) {
    printf("%lf %lf\n", input[i], output[i]);
  }
}

int main() {
    printf("MARCO\n");
    
  srand(omp_get_wtime());

  int len = 10000;
  int radius = 100;

  float *input = malloc(sizeof(float) * len);
  float *output = malloc(sizeof(float) * len);

  float pi = 3.14;


  double t0, t1, t2;
  double t_naive, t_opt;

  t0 = omp_get_wtime();
  median_naive(input, output, radius, len);
  t1 = omp_get_wtime();
  t_naive = t1 - t0;
  printf("naive: %lf \n", t_naive);
//  print_result(input, output, len);

  for (int n = 1; n < N_THREADS; ++n) {
      omp_set_num_threads(n);
      
    t1 = omp_get_wtime();
    median_opt(input, output, radius, len);
    t2 = omp_get_wtime();
    t_opt = t2 - t1;
    printf("opti(%d THREADS) : %lf    ", n, t_opt);
    printf("speedup: %lf \n", t_naive / t_opt);
  }

//  print_result(input, output, len);

  free(input);
  free(output);
  
  return 0;
}
