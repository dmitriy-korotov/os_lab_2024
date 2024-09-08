#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "find_min_max.h"

void testGetMinMax(void) {
  int positive_numbers[] = { 1, 3, 5, 2, 120, 123 };
  int negative_numbers[] = { -1, -3, -6, -10, -120, -323 };
  int some_numbers[] = { -1, 3, -6, 10, 120, -323 };
  int numbers_with_zero[] = { -1, 3, 0, 10, 120, -323 };
  int same_numbers[] = { 4, 4, 4, 4 };
  int one_number[] = { 1 };

  struct MinMax min_max = GetMinMax(positive_numbers, 0, sizeof(positive_numbers) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, 1);
  CU_ASSERT_EQUAL(min_max.max, 123);

  min_max = GetMinMax(negative_numbers, 0, sizeof(negative_numbers) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, -323);
  CU_ASSERT_EQUAL(min_max.max, -1);

  min_max = GetMinMax(some_numbers, 0, sizeof(some_numbers) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, -323);
  CU_ASSERT_EQUAL(min_max.max, 120);

  min_max = GetMinMax(numbers_with_zero, 0, sizeof(numbers_with_zero) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, -323);
  CU_ASSERT_EQUAL(min_max.max, 120);

  min_max = GetMinMax(same_numbers, 0, sizeof(same_numbers) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, 4);
  CU_ASSERT_EQUAL(min_max.max, 4);

  min_max = GetMinMax(one_number, 0, sizeof(one_number) / sizeof(int));
  CU_ASSERT_EQUAL(min_max.min, 1);
  CU_ASSERT_EQUAL(min_max.max, 1);
}

int main() {
  CU_pSuite pSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  pSuite = CU_add_suite("Suite", NULL, NULL);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ((NULL == CU_add_test(pSuite, "test of RevertString function",
                           testGetMinMax))) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
