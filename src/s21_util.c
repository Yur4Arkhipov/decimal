#include "s21_util.h"

#include "s21_decimal.h"

// value_1, value_2 and result's size should be the same
uint32_t s21_util_add(const uint32_t *value_1, const uint32_t *value_2,
                      uint32_t *result, int size) {
  uint32_t overflow = 0;
  for (int i = 0; i < size; i++) {
    uint64_t value_1_64 = value_1[i];
    uint64_t value_2_64 = value_2[i];
    uint64_t result_64 = value_1_64 + value_2_64 + overflow;
    result[i] = result_64;
    overflow = result_64 >> 32;
  }
  return overflow;
}

uint32_t s21_util_mul_single(const uint32_t *value_1, uint32_t value_2,
                             uint32_t *result, int size) {
  uint32_t overflow = 0;
  for (int i = 0; i < size; i++) {
    uint64_t value_1_64 = value_1[i];
    uint64_t value_2_64 = value_2;
    uint64_t result_64 = value_1_64 * value_2_64 + overflow;
    result[i] = result_64;
    overflow = result_64 >> 32;
  }
  return overflow;
}

// size of result should be twice the *size*
void s21_util_mul(const uint32_t *value_1, const uint32_t *value_2,
                  uint32_t *result, int size) {
  memset(result, 0, size * 2 * sizeof(uint32_t));
  for (int i = 0; i < size; i++) {
    uint32_t value_1_temp[size * 2];
    memset(value_1_temp, 0, size * 2 * sizeof(uint32_t));
    memcpy(value_1_temp + i, value_1, size * sizeof(uint32_t));
    uint32_t result_temp[size * 2];
    s21_util_mul_single(value_1_temp, value_2[i], result_temp, size * 2);
    s21_util_add(result, result_temp, result, size * 2);
  }
}

uint32_t s21_util_mul_single_inplace(uint32_t *value_1, uint32_t value_2,
                                     int size) {
  uint32_t result[size];
  uint32_t overflow = s21_util_mul_single(value_1, value_2, result, size);
  memcpy(value_1, result, size * sizeof(uint32_t));
  return overflow;
}

// Функция вычитания двух чисел: result = value_1 - value_2
void subtract(const uint32_t *value_1, const uint32_t *value_2,
              uint32_t *result, int size) {
  int borrow = 0;
  for (int i = 0; i < size; i++) {
    int64_t diff = (int64_t)value_1[i] - value_2[i] - borrow;
    if (diff < 0) {
      diff += ((int64_t)1 << 32);
      borrow = 1;
    } else {
      borrow = 0;
    }
    result[i] = (uint32_t)diff;
  }
}

// function shifts bits to the right
void shift_right(uint32_t *value, int size) {
  // carry_bit - переносной бит
  int carry_bit = 0;
  for (int i = size - 1; i >= 0; i--) {
    int next_carry_bit = value[i] & 1 ? 0x80000000 : 0;
    value[i] = (value[i] >> 1) | carry_bit;
    carry_bit = next_carry_bit;
  }
}

// function shifts bits to the left
int shift_left(uint32_t *value, int size) {
  // carry_bit - переносной бит
  int carry_bit = 0;
  for (int i = 0; i < size; i++) {
    int next_carry_bit = value[i] & 0x80000000 ? 1 : 0;
    value[i] = (value[i] << 1) | carry_bit;
    carry_bit = next_carry_bit;
  }

  return carry_bit;
}

// function comparing two numbers: returns -1 if value_1 < value_2, 0 if value_1
// == value_2, and 1 if value_1 > value_2
int compare(const uint32_t *value_1, const uint32_t *value_2, int size) {
  for (int i = size - 1; i >= 0; i--) {
    if (value_1[i] > value_2[i]) {
      return 1;
    } else if (value_1[i] < value_2[i]) {
      return -1;
    }
  }
  return 0;
}

// function comparing two decimals: returns -1 if value_1 < value_2, 0 if
// value_1 == value_2, and 1 if value_1 > value_2
int compare_dec(s21_decimal value_1, s21_decimal value_2) {
  uint32_t extended_value_1[12];
  uint32_t extended_value_2[12];
  memset(extended_value_1, 0, 12 * sizeof(uint32_t));
  memset(extended_value_2, 0, 12 * sizeof(uint32_t));
  s21_util_extend(value_1, extended_value_1);
  s21_util_extend(value_2, extended_value_2);

  int sign_1 = s21_util_get_sign(&value_1);
  int sign_2 = s21_util_get_sign(&value_2);

  if (sign_1 != sign_2) {
    if (is_zero(&value_1) == 1 && is_zero(&value_2) == 1) return 0;
    if (sign_1 == 0) {
      return 1;
    } else {
      return -1;
    }
  }

  for (int i = 11; i >= 0; i--) {
    if (extended_value_1[i] > extended_value_2[i]) {
      return (sign_1 == 0) ? 1 : -1;
    } else if (extended_value_1[i] < extended_value_2[i]) {
      return (sign_1 == 0) ? -1 : 1;
    }
  }

  return 0;
}

int is_zero(const s21_decimal *value) {
  int is_zero = 1;  // true
  for (size_t i = 0; i < 3; i++) {
    if (value->bits[i] != 0) {
      is_zero = 0;  // false
      break;
    }
  }

  return is_zero;
}

int s21_util_div(const uint32_t *value_1, const uint32_t *value_2,
                 uint32_t *quotient, uint32_t *remainder, int size) {
  uint32_t divisor[size];  // делитель

  memset(quotient, 0, size * sizeof(uint32_t));
  memcpy(remainder, value_1, size * sizeof(uint32_t));
  memcpy(divisor, value_2, size * sizeof(uint32_t));

  int shift_count = 0;
  int overflow = 0;

  // find the maximum offset(shift)
  while (compare(remainder, divisor, size) >= 0 && overflow != 1) {
    overflow = shift_left(divisor, size);
    shift_count++;
  }

  while (shift_count > 0) {
    shift_right(divisor, size);
    shift_count--;

    if (overflow == 1) {
      divisor[size - 1] |= 0x80000000;
      overflow = 0;
    }

    shift_left(quotient, size);

    if (compare(remainder, divisor, size) >= 0) {
      subtract(remainder, divisor, remainder, size);
      quotient[0] |= 1;  // set the low bit
    }
  }

  return 0;
}

int s21_util_get_exponent(s21_decimal value) {
  return (value.bits[3] >> 16) & 0xFF;
}

void s21_util_set_exponent(s21_decimal *value, int exponent) {
  int sign = s21_util_get_sign(value);
  int temp = 0;
  if (sign == 1) {
    temp = 0x80000000;
  }
  // exponent zeroing
  uint8_t zeros = 1;
  value->bits[3] &= zeros;
  // set value
  value->bits[3] |= (exponent << 16);
  value->bits[3] |= temp;
}

// result should be size 6
void s21_util_extend(s21_decimal value, uint32_t *result) {
  int exponent = s21_util_get_exponent(value);
  int power = 28 - exponent;
  memset(result, 0, 6 * sizeof(uint32_t));
  memcpy(result, value.bits, 3 * sizeof(uint32_t));
  for (int i = 0; i < power; i++) {
    s21_util_mul_single_inplace(result, 10, 6);
  }
}

void last_shrink_div(const uint32_t *value, uint32_t *divisor, uint32_t *tmp,
                     int size, int settings) {
  uint32_t quotient[size];
  uint32_t remainder[size];
  uint32_t zero[size];
  memset(zero, 0, size * sizeof(uint32_t));
  s21_util_div(value, divisor, quotient, remainder, size);
  shift_right(divisor, size);
  int even = quotient[0] % 2 == 0;
  int comparison = compare(remainder, divisor, size);
  int add_one = 0;
  if (settings & ROUND) {
    add_one = comparison >= 0;
  } else if (settings & FLOOR) {
    add_one = (settings & NEGATIVE) && compare(remainder, zero, size) > 0;
  } else if (!(settings & TRUNCATE)) {
    add_one = ((!even && comparison == 0) || comparison > 0);
  }
  if (add_one) {
    s21_util_add(quotient, EXTENDED_1, tmp, size);
  } else {
    memcpy(tmp, quotient, size * sizeof(uint32_t));
  }
}

int s21_util_shrink_aux(const uint32_t *value, s21_decimal *result, int size,
                        int power, int settings, const uint32_t *max_value,
                        const uint32_t *last_before_convert) {
  int exponent = power;
  int max_exponent = 28;
  if (settings & REQUIRE_INT) {
    max_exponent = 0;
  } else if (settings & DEC_TO_FLT) {
    max_exponent = 999;
  }

  uint32_t divisor[size];
  memset(divisor, 0, size * sizeof(uint32_t));
  divisor[0] = 1;

  uint32_t tmp[size];
  memcpy(tmp, value, size * sizeof(uint32_t));
  if (compare(value, max_value, size) > 0 || exponent > max_exponent) {
    while (1) {
      exponent--;
      s21_util_mul_single_inplace(divisor, 10, size);

      if (compare(tmp, last_before_convert, size) <= 0 &&
          exponent <= max_exponent) {
        last_shrink_div(value, divisor, tmp, size, settings);
        break;
      } else {
        uint32_t quotient[size];
        uint32_t remainder[size];
        s21_util_div(value, divisor, quotient, remainder, size);
        memcpy(tmp, quotient, size * sizeof(uint32_t));
      }
    }
  }
  int ret_value = OK;
  if (settings & DEC_TO_FLT) {
    memcpy(result->bits, tmp, 3 * sizeof(uint32_t));
    ret_value = exponent;
  } else if (exponent < 0) {
    ret_value = TOO_BIG;
  } else {
    memcpy(result->bits, tmp, 3 * sizeof(uint32_t));
    result->bits[3] = 0;
    s21_util_set_exponent(result, exponent);
    s21_util_remove_trailing_zeroes(result);
  }
  return ret_value;
}

int s21_util_shrink(const uint32_t *value, s21_decimal *result, int size,
                    int power, int settings) {
  return s21_util_shrink_aux(value, result, size, power, settings,
                             MAX_EXTENDED_FIT, LAST_BEFORE_CONVERT);
}

void s21_util_remove_trailing_zeroes(s21_decimal *value) {
  int exponent = s21_util_get_exponent(*value);

  if (exponent == 0) {
    return;
  }

  uint32_t tmp[3];
  memcpy(tmp, value->bits, 3 * sizeof(uint32_t));

  uint32_t remainder[3];
  uint32_t quotient[3];

  while (exponent > 0) {
    s21_util_div(tmp, EXTENDED_10, quotient, remainder, 3);

    if (remainder[0] != 0 || remainder[1] != 0 || remainder[2] != 0) break;

    memcpy(tmp, quotient, 3 * sizeof(uint32_t));
    exponent--;
  }

  memcpy(value->bits, tmp, 3 * sizeof(uint32_t));

  s21_util_set_exponent(value, exponent);
}

// 1 - negative, 0 - positive
void s21_util_set_sign(s21_decimal *value, int sign) {
  if (sign) {
    value->bits[3] |= 0x80000000;
  } else {
    value->bits[3] &= ~0x80000000;
  }
}

// 1 - negative, 0 - positive
int s21_util_get_sign(s21_decimal *value) {
  int sign = (value->bits[3] & 0x80000000) ? 1 : 0;
  return sign;
}

int invert_big_small_error(int error) {
  if (error == TOO_BIG) {
    error = TOO_SMALL;
  } else if (error == TOO_SMALL) {
    error = TOO_BIG;
  }
  return error;
}

void s21_util_round_to_int(s21_decimal value, s21_decimal *result,
                           int settings) {
  uint32_t extended_value[6];
  s21_util_extend(value, extended_value);
  int sign = s21_util_get_sign(&value);
  settings |= REQUIRE_INT | (sign ? NEGATIVE : 0);
  s21_util_shrink(extended_value, result, 6, 28, settings);
  s21_util_set_sign(result, sign);
}

int is_decimal_correct(s21_decimal value) {
  int is_correct = 1;
  // 0x7F00FFFF = 01111111000000001111111111111111
  if (s21_util_get_exponent(value) > 28 ||
      ((value.bits[3] & 0x7F00FFFF) != 0)) {
    is_correct = 0;
  }

  return is_correct;
}