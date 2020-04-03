typedef struct Histogram
{
     float min; // min value containable
     float max; // max value containable
     int size; // capacity
     int lastest; // oldest inserted value
     int n_bins; // number of bins
     int median_bin; // the bin containing the median
     int left; // the count of values left of the median
     int right; // the count of values right of the median
     int * entries; // the entries
     int * bins; // the bins
} Histogram;




int histo_bin_left_of(Histogram* histo, int bin);

// O(size)
// finds the first non-zero bin right of the given bin
int histo_bin_right_of(Histogram* histo, int bin);

// O(1)
int
histo_float_to_intern(Histogram* histo, float entry);

// O(1)
float
histo_intern_to_float(Histogram* histo, int intern);

Histogram
histo_new(int size, int n_bins, float min, float max);

// O(1)
void
histo_decrease(Histogram* histo, int bin);

// O(1)
void
histo_increase(Histogram* histo, int bin);

// O(1)
void
histo_push_intern(Histogram* histo, int bin);

// O(1)
void histo_push(Histogram* histo, float value);

// O(size)
float
histo_stupid_median(Histogram* histo);

void
histo_print(Histogram* histo);

void
histo_free(Histogram * histo);

float
histo_smart_median(Histogram* histo);
