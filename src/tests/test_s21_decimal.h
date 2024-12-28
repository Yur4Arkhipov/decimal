#ifndef S21_DECIMAL_TEST_H
#define S21_DECIMAL_TEST_H

#include "../s21_util.h"
#include <check.h>

#define LEN(arr) sizeof(arr) / sizeof(arr[0])
#define DECIMAL(a, b, c, d)                                                    \
  (s21_decimal) {                                                              \
    { a, b, c, d }                                                             \
  }

// ARITHMETIC
Suite *suite_add(void);
void test_add(s21_decimal value_1, s21_decimal value_2, s21_decimal check);
void test_add_fail(s21_decimal value_1, s21_decimal value_2, int check);

Suite *suite_sub(void);
void test_sub(s21_decimal decimal1, s21_decimal decimal2, s21_decimal check);
void test_sub_fail(s21_decimal decimal1, s21_decimal decimal2, int check);

Suite *suite_mul(void);
void test_mul(s21_decimal decimal1, s21_decimal decimal2, s21_decimal check);
void test_mul_fail1(s21_decimal decimal1, s21_decimal decimal2, int code_check);
void test_mul_fail2(s21_decimal decimal1, s21_decimal decimal2,
                    s21_decimal decimal_check, int code_check);

Suite *suite_div(void);
void test_div(s21_decimal decimal1, s21_decimal decimal2, s21_decimal check);
void test_div_fail1(s21_decimal decimal1, s21_decimal decimal2, int code_check);
void test_div_fail2(s21_decimal decimal1, s21_decimal decimal2,
                    s21_decimal decimal_check, int code_check);

// CONVERSIONS
Suite *suite_is_equal(void);
void test_is_equal(s21_decimal decimal1, s21_decimal decimal2, int check);

Suite *suite_is_less(void);
void test_is_less(s21_decimal decimal1, s21_decimal decimal2, int check);

Suite *suite_is_greater(void);
void test_is_greater(s21_decimal decimal1, s21_decimal decimal2, int check);

Suite *suite_is_less_or_equal(void);
void test_is_less_or_equal(s21_decimal decimal1, s21_decimal decimal2,
                           int check);

Suite *suite_is_greater_or_equal(void);
void test_is_greater_or_equal(s21_decimal decimal1, s21_decimal decimal2,
                              int check);

Suite *suite_is_not_equal(void);
void test_is_not_equal(s21_decimal decimal1, s21_decimal decimal2, int check);

// CONVERTORS
Suite *suite_from_int_to_decimal(void);
void test_from_int_to_decimal(int number, s21_decimal decimal_check);

Suite *suite_from_decimal_to_int(void);
void test_from_decimal_to_int(s21_decimal decimal, int check);

Suite *suite_from_float_to_decimal(void);
void test_from_float_to_decimal(float f, s21_decimal decimal_check);

Suite *suite_from_decimal_to_float(void);
void test_from_decimal_to_float(s21_decimal decimal, float check);

// OTHER
Suite *suite_truncate(void);
void test_truncate(s21_decimal decimal, s21_decimal decimal_check);

Suite *suite_floor(void);
void test_floor(s21_decimal decimal, s21_decimal decimal_check);

Suite *suite_negate(void);
void test_negate(s21_decimal decimal, s21_decimal decimal_check);

Suite *suite_round(void);
void test_round(s21_decimal decimal, s21_decimal decimal_check);

#endif