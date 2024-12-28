CC=gcc
CC_FLAGS=-c -std=c11 -Wall -Werror -Wextra
BUILD_NAME=s21_decimal
GCOV=-fprofile-arcs -ftest-coverage

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

# for testing decimal functions
TEST_FLAGS=-lcheck -lsubunit -lrt -lm -lpthread
TEST_TARGET = test_s21_decimal_functions

# clang-format
CLANG_FORMAT = clang-format -i tests/*.c tests/*.h *.c *.h
CLANG_FORMAT_CHECK = clang-format -n tests/*.c tests/*.h *.c *.h

all: build test gcov_report

build: s21_decimal.a

#запуск теста
test: tests/${TEST_TARGET}.c tests/test_s21_add.c tests/test_s21_sub.c tests/test_s21_mul.c tests/test_s21_div.c tests/test_comparision.c tests/test_other.c tests/test_convertors.c s21_decimal.a
	${CC} tests/$(TEST_TARGET).c tests/test_s21_add.c tests/test_s21_sub.c tests/test_s21_mul.c tests/test_s21_div.c tests/test_comparision.c tests/test_convertors.c tests/test_other.c s21_decimal.a $(TEST_FLAGS) -o tests/${TEST_TARGET}
	./tests/$(TEST_TARGET)

rebuild: clean build

s21_decimal.a: s21_decimal.o s21_util.o
	ar rcs s21_decimal.a s21_decimal.o s21_util.o
	ranlib s21_decimal.a

s21_decimal.o: s21_decimal.c
	${CC} ${CC_FLAGS} ${BUILD_NAME}.c

s21_util.o: s21_util.c 
	${CC} ${CC_FLAGS} s21_util.c

gcov_report: s21_decimal.c s21_util.c tests/test_s21_add.c tests/test_s21_sub.c tests/test_s21_mul.c tests/test_s21_div.c tests/test_comparision.c tests/test_other.c tests/test_convertors.c tests/$(TEST_TARGET).c
	${CC} --coverage s21_decimal.c s21_util.c tests/test_s21_add.c tests/test_s21_sub.c tests/test_s21_mul.c tests/test_s21_div.c tests/test_comparision.c tests/test_other.c tests/test_convertors.c tests/$(TEST_TARGET).c ${TEST_FLAGS} -o tests/test_report
	./tests/test_report
	lcov --directory . --capture -o coverage.info
	genhtml --output-directory report --legend coverage.info
	rm ./tests/*.gcda
	rm ./tests/*.gcno
	rm coverage.info

clang_format: 
	${CLANG_FORMAT}
	${CLANG_FORMAT_CHECK}

clean:
	-rm -rf ./*.o
	-rm ./*.a
# -rm tests/test_s21_decimal_functions.c
	-rm ./tests/test_s21_decimal_functions
	-rm ./tests/test_report
	-rm -rf report