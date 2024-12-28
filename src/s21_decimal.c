#include "s21_decimal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "s21_util.h"

// ARITHMETIC
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  uint32_t extended_value_1[6];
  uint32_t extended_value_2[6];

  s21_util_extend(value_1, extended_value_1);
  s21_util_extend(value_2, extended_value_2);

  int sign_1 = s21_util_get_sign(&value_1);
  int sign_2 = s21_util_get_sign(&value_2);

  uint32_t temp_res[6];
  int operation_result = OK;
  int set_sign;

  if (sign_1 == sign_2) {
    s21_util_add(extended_value_1, extended_value_2, temp_res, 6);
    operation_result = s21_util_shrink(temp_res, result, 6, 28, 0);
    set_sign = sign_1;
  } else {
    if (compare(extended_value_1, extended_value_2, 6) >= 0) {
      subtract(extended_value_1, extended_value_2, temp_res, 6);
      operation_result = s21_util_shrink(temp_res, result, 6, 28, 0);
      set_sign = sign_1;
    } else {
      subtract(extended_value_2, extended_value_1, temp_res, 6);
      operation_result = s21_util_shrink(temp_res, result, 6, 28, 0);
      set_sign = sign_2;
    }
  }

  s21_util_set_sign(result, set_sign);
  if (set_sign) {
    operation_result = invert_big_small_error(operation_result);
  }
  return operation_result;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int sign = s21_util_get_sign(&value_2);
  s21_util_set_sign(&value_2, !sign);
  int operation_result = s21_add(value_1, value_2, result);
  return operation_result;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  uint32_t extended_value_1[6];
  uint32_t extended_value_2[6];

  s21_util_extend(value_1, extended_value_1);
  s21_util_extend(value_2, extended_value_2);

  uint32_t temp_res[12];
  s21_util_mul(extended_value_1, extended_value_2, temp_res, 6);

  int operation_result = s21_util_shrink(temp_res, result, 12, 56, 0);

  int sign = s21_util_get_sign(&value_1) ^ s21_util_get_sign(&value_2);
  s21_util_set_sign(result, sign);

  if (sign) {
    operation_result = invert_big_small_error(operation_result);
  }

  return operation_result;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (is_zero(&value_2) == 1) {
    return DIV_BY_ZERO;
  }
  uint32_t extended_value_1[18];
  uint32_t extended_value_2[18];

  memset(extended_value_1, 0, 18 * sizeof(uint32_t));
  memset(extended_value_2, 0, 18 * sizeof(uint32_t));

  s21_util_extend(value_1, extended_value_1);
  s21_util_extend(value_2, extended_value_2);

  for (int i = 0; i < 56; i++) {
    s21_util_mul_single_inplace(extended_value_1, 10, 18);
  }

  uint32_t quotient[18];
  uint32_t remainder[18];
  s21_util_div(extended_value_1, extended_value_2, quotient, remainder, 18);

  int operation_result = s21_util_shrink(quotient, result, 18, 56, 0);

  int sign = s21_util_get_sign(&value_1) ^ s21_util_get_sign(&value_2);
  s21_util_set_sign(result, sign);

  if (sign) {
    operation_result = invert_big_small_error(operation_result);
  }

  return operation_result;
}

// COMPARISONS

// return 1 if eq (true), 0 if neq (false)
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) == 0;
}

// return 1 if value_1 < value_2, 0 if false
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) == -1;
}

// return 1 if value_1 > value_2, 0 if false
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) == 1;
}

// return 1 if value_1 <= value_2, 0 if false
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) <= 0;
}

// return 1 if value_1 >= value_2, 0 if false
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) >= 0;
}

// return 1 if value_1 != value_2, 0 if false
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_dec(value_1, value_2) != 0;
}

// CONVERTORS

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int code = OK;

  if (!dst) {
    code = CONVERSION_ERROR;
  } else {
    memset(dst, 0, sizeof(s21_decimal));
    if (src < 0) {
      s21_util_set_sign(dst, 1);
      dst->bits[0] = -src;
    } else {
      dst->bits[0] = src;
    }
  }

  return code;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  if (src != src || src == INFINITY || src == -INFINITY ||
      fabsf(src) >= MAX_FLOAT) {
    return CONVERSION_ERROR;
  } else if (src != 0.0 && fabsf(src) < MIN_FLOAT) {
    memset(dst, 0, 4 * sizeof(int));
    return CONVERSION_ERROR;
  }

  int sign = src < 0.0;
  src = fabsf(src);
  char s[16];
  sprintf(s, "%.6e", src);
  int exp;
  sscanf(s + strlen(s) - 3, "%d", &exp);
  if (exp <= -23) {
    sprintf(s, "%#.*e", exp + 28, src);
    sscanf(s + strlen(s) - 3, "%d", &exp);
  }
  int integer_part;
  s[1] = s[0];
  sscanf(s + 1, "%d", &integer_part);
  if (exp == -27 && integer_part < 10) {
    integer_part *= 10;
  }
  uint32_t extended[6];
  memset(extended, 0, 6 * sizeof(uint32_t));
  extended[0] = integer_part;
  int iter = exp <= -23 ? 6 - exp : 28;
  for (int i = 0; i < iter; i++) {
    s21_util_mul_single_inplace(extended, 10, 6);
  }
  s21_util_shrink(extended, dst, 6, -exp + 6 + 28, ROUND);
  s21_util_set_sign(dst, sign);
  return OK;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int code = OK;

  if (!dst) {
    code = CONVERSION_ERROR;
  } else if (!is_decimal_correct(src)) {
    code = CONVERSION_ERROR;
  } else {
    *dst = 0;
    s21_decimal max_dec_for_int = {{0x7FFFFFFF, 0, 0, 0}};
    s21_decimal min_dec_for_int = {{0x80000000, 0, 0, 0x80000000}};
    s21_decimal truncate_value;
    s21_truncate(src, &truncate_value);

    if (compare_dec(truncate_value, max_dec_for_int) == 1 ||
        compare_dec(truncate_value, min_dec_for_int) == -1) {
      code = CONVERSION_ERROR;
    } else {
      *dst = truncate_value.bits[0];
      if (s21_util_get_sign(&truncate_value) == 1) *dst *= (-1);
    }
  }

  return code;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  const uint32_t max_value[3] = {9999999, 0, 0};
  const uint32_t last_before_convert[3] = {99999994, 0, 0};

  int sign = s21_util_get_sign(&src);
  int power = s21_util_get_exponent(src);
  s21_decimal result;
  int exp =
      s21_util_shrink_aux((uint32_t *)&src.bits, &result, 3, 0,
                          DEC_TO_FLT | ROUND, max_value, last_before_convert);
  exp = -exp - power + 6;
  int value = result.bits[0];
  char s[16];
  s[1] = '.';
  s[8] = 'e';
  sprintf(s + 9, "%+.2d", exp);
  for (int i = 7; i >= 0; i--) {
    if (i == 1) {
      continue;
    }
    s[i] = '0' + value % 10;
    value /= 10;
  }
  sscanf(s, "%e", dst);
  if (sign) {
    *dst = -*dst;
  }
  return OK;
}

// OTHER

int s21_floor(s21_decimal value, s21_decimal *result) {
  s21_util_round_to_int(value, result, FLOOR);
  return OK;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_util_round_to_int(value, result, ROUND);
  return OK;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_util_round_to_int(value, result, TRUNCATE);
  return OK;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  int sign = s21_util_get_sign(&value);
  s21_util_set_sign(&value, !sign);
  *result = value;
  return OK;
}
