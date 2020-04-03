#include <stdio.h>
#include <stdlib.h>

#include "histogram.h"

int histo_bin_left_of(Histogram* histo, int bin)
{
     for (int i = bin-1; i >= 0; --i)
     {
	  if (histo->bins[i] > 0)
	       return i;
     }
     printf("ERROR: NO NON-ZERO BIN LEFT OF GIVEN BIN\n");
}

// O(size)
// finds the first non-zero bin right of the given bin
int histo_bin_right_of(Histogram* histo, int bin)
{
     for (int i = bin+1; i < histo->n_bins; ++i)
     {
	  if (histo->bins[i] > 0)
	       return i;
     }
     printf("ERROR: NO NON-ZERO BIN RIGHT OF GIVEN BIN\n");
}

// O(1)
int
histo_float_to_intern(Histogram* histo, float entry)
{
     return (int) ((entry-histo->min)/(histo->max-histo->min) * ((float) histo->n_bins));
}

// O(1)
float
histo_intern_to_float(Histogram* histo, int intern)
{
     return histo->min + ((float) intern)*(histo->max - histo->min)/((float) histo->n_bins);
}

Histogram
histo_new(int size, int n_bins, float min, float max)
{
     Histogram histo = {min, max, size, 0, n_bins, 0, 0, 0, calloc(size, sizeof(int)), calloc(n_bins, sizeof(int))};
     histo.bins[0] = size;
     return histo;
}

// O(1)
void
histo_decrease(Histogram* histo, int bin)
{
     histo->bins[bin]--;

     if (bin < histo->median_bin)
	  histo->left--;
     if (bin > histo->median_bin)
	  histo->right--;

     int m = (histo->size + 1)/2;
     if (histo->left >= m)
     {
	  int left_bin = histo_bin_left_of(histo, histo->median_bin);
	  histo->right += histo->bins[histo->median_bin];
	  histo->left -= histo->bins[left_bin];
	  histo->median_bin = left_bin;
     }
     else if (histo->left + histo->bins[histo->median_bin] < m)
     {
	  int right_bin = histo_bin_right_of(histo, histo->median_bin);
	  histo->left += histo->bins[histo->median_bin];
	  histo->right -= histo->bins[right_bin];
	  histo->median_bin = right_bin;
	  
     }
     

}

// O(1)
void
histo_increase(Histogram* histo, int bin)
{
     histo->bins[bin]++;

     if (bin < histo->median_bin)
	  histo->left++;
     if (bin > histo->median_bin)
	  histo->right++;


     int m = (histo->size + 1)/2;
     if (histo->left >= m)
     {
	  int left_bin = histo_bin_left_of(histo, histo->median_bin);
	  histo->right += histo->bins[histo->median_bin];
	  histo->left -= histo->bins[left_bin];
	  histo->median_bin = left_bin;
     }
     else if (histo->left + histo->bins[histo->median_bin] < m)
     {
	  int right_bin = histo_bin_right_of(histo, histo->median_bin);
	  histo->left += histo->bins[histo->median_bin];
	  histo->right -= histo->bins[right_bin];
	  histo->median_bin = right_bin;
	  
     }

}

// O(1)
void
histo_push_intern(Histogram* histo, int bin)
{
     int i = histo->lastest;
     int old_bin = histo->entries[i];
     histo_decrease(histo, old_bin);
     histo_increase(histo, bin);
     histo->entries[i] = bin;
     histo->lastest = (i+1) % histo->size;
}

// O(1)
void histo_push(Histogram* histo, float value)
{
     histo_push_intern(histo, histo_float_to_intern(histo, value));
}

// O(size)
float
histo_stupid_median(Histogram* histo)
{
     int sum = 0;
     int i;
     for (i = 0; sum < (histo->size + 1)/2; ++i)
     {
	  sum += histo->bins[i];
     }
     return histo_intern_to_float(histo, i-1);
}

void
histo_print(Histogram* histo)
{
     printf("ENTRIES: ");
     for (int i = 0; i < histo->size; ++i)
	  printf("%lf ", histo_intern_to_float(histo, histo->entries[i]));
     printf("\n");
     printf("BINS: ");
     for (int i = 0; i < histo->n_bins; ++i)
	  printf("%d ", histo->bins[i]);
     printf("\n");
     printf("MEDIAN: %lf\n", histo_stupid_median(histo));
     printf("QUICK MEDIAN: %lf\n", histo_intern_to_float(histo, histo->median_bin));
     printf("\n");
}

void
histo_free(Histogram * histo)
{
     free(histo->entries);
     free(histo->bins);
}
