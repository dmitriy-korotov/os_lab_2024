#include "find_min_max.h"

#include <math.h>
#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  while (begin < end) {
    min_max.min = fmin(min_max.min, array[begin]);
    min_max.max = fmax(min_max.max, array[begin]);
    begin++;
  }
  return min_max;
}
