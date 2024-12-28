#ifndef S21_UTIL_H
#define S21_UTIL_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "s21_decimal.h"

enum {
  NEGATIVE = 1,
  REQUIRE_INT = 2,
  TRUNCATE = 4,
  ROUND = 8,
  FLOOR = 16,
  DEC_TO_FLT = 32,
};

// Actual value: 1.000000003171076850971051E-28
// < should be used
#define MIN_FLOAT 1e-28
// Actual value: 79228162514264337593543950336
// >= should be used
#define MAX_FLOAT 79228162514264337593543950335.0f

// Max extended value that can be directly converted to decimal
static const uint32_t MAX_EXTENDED_FIT[18] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0,
    0,          0,          0,          0, 0, 0, 0, 0, 0};
// This value can be divided by 10 (with bank rounding) to become
// MAX_EXTENDED_FIT
static const uint32_t LAST_BEFORE_CONVERT[18] = {
    0xfffffffa, 0xffffffff, 0xffffffff, 0x00000009, 0, 0, 0, 0, 0,
    0,          0,          0,          0,          0, 0, 0, 0, 0};

static const uint32_t EXTENDED_1[18] = {1, 0, 0, 0, 0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint32_t EXTENDED_10[18] = {
    10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint32_t s21_util_add(const uint32_t *value_1, const uint32_t *value_2,
                      uint32_t *result, int size);
uint32_t s21_util_mul_single(const uint32_t *value_1, uint32_t value_2,
                             uint32_t *result, int size);
void s21_util_mul(const uint32_t *value_1, const uint32_t *value_2,
                  uint32_t *result, int size);
uint32_t s21_util_mul_single_inplace(uint32_t *value_1, uint32_t value_2,
                                     int size);
void subtract(const uint32_t *value_1, const uint32_t *value_2,
              uint32_t *result, int size);
void shift_right(uint32_t *value, int size);
int shift_left(uint32_t *value, int size);
int compare(const uint32_t *value_1, const uint32_t *value_2, int size);
int compare_dec(s21_decimal value_1, s21_decimal value_2);
int s21_util_div(const uint32_t *value_1, const uint32_t *value_2,
                 uint32_t *quotient, uint32_t *remainder, int size);
int s21_util_get_exponent(s21_decimal value);
void s21_util_set_exponent(s21_decimal *value, int exponent);
void s21_util_extend(s21_decimal value, uint32_t *result);
int s21_util_max_extended(uint32_t *result, int size, int power);
int s21_util_shrink_aux(const uint32_t *value, s21_decimal *result, int size,
                        int power, int settings, const uint32_t *max_value,
                        const uint32_t *last_before_convert);
int s21_util_shrink(const uint32_t *value, s21_decimal *result, int size,
                    int power, int settings);
void s21_util_remove_trailing_zeroes(s21_decimal *value);
void s21_util_set_sign(s21_decimal *value, int sign);
int s21_util_get_sign(s21_decimal *value);
int invert_big_small_error(int error);
void s21_util_round_to_int(s21_decimal value, s21_decimal *result,
                           int settings);
void s21_print_decimal_bits(s21_decimal decimal);
void s21_print_bit(int number, int color);
void s21_print_bits(size_t const size, void const *const ptr, int color);
int is_zero(const s21_decimal *value);
int is_decimal_correct(s21_decimal value);

#endif
