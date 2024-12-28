
#include "../s21_decimal.h"
#include "../s21_util.h"
#include "test_s21_decimal.h"

int main(void) {
  Suite *suites[] = {suite_add(),
                     suite_sub(),
                     suite_mul(),
                     suite_div(),
                     suite_is_equal(),
                     suite_is_less(),
                     suite_is_greater(),
                     suite_is_less_or_equal(),
                     suite_is_greater_or_equal(),
                     suite_is_not_equal(),
                     suite_negate(),
                     suite_truncate(),
                     suite_floor(),
                     suite_round(),
                     suite_from_int_to_decimal(),
                     suite_from_decimal_to_int(),
                     suite_from_float_to_decimal(),
                     suite_from_decimal_to_float(),
                     NULL};

  for (Suite **s = suites; *s; s++) {
    SRunner *sr = srunner_create(*s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    srunner_free(sr);
  }
}
