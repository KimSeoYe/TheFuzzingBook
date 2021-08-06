# 1. introduction to Software Testing

## Simple Testing

- src/my_sqrt.c
- test/my_sqrt_test.c

It generates manual tests and random tests for `my_sqrt` program.

## Exercise 2

- src/shellsort.c
- test/exercise2.c

It also generates manual tests and random tests for `shellsort` program.

## Exercise 3

- src/quadratic_solver.c
- test/exercise3.c

### Part1 & Part 2. Find Bug-Trigerring Inputs & Fix The Problem

1. a division by zero : if a == 0
2. violate the precondition of my_sqrt_fixed() : x(q) should be non-negative

### part3. Odds and Ends

The process of calculating the chances of discovering the error conditions with random inputs.<br>
It calculates how long would one have to wait on average until a bug gets triggered, assuming one can do a billion tests per second.
