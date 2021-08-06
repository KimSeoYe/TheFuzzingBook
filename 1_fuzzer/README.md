# Fuzzing: Breaking Things with Random Inputs

### src & include

`fileio` : It has some functions for I/O executions. <br>
`fuzzer` : It has a simple fuzzer


## Fuzzing External Programs with a Simple Fuzzer

### Creating Input Files
#### 0_fuzzer_test.c
It is for the chpater *Simple Fuzzer* .
It creates a random string using fuzzer and write to the file `input.txt` under a unique temporary directory.

### Invoking External Program
#### 1_invoking_extern_prog.c
It is for the chapter *Invoking External Programs* and *Long Running Fuzzing*.
The function `invoking_exter_prog()` simply invoke `bc`.
The most important is `long_running_fuzzing()`.
- It executes `bc` 100 times using the simple fuzzer, and checks the results.
- It creates one input file and one output file for each execution (total: 100 input files & 100 ouput files).
  - Because we can find and use meaningful input later on.
  - Thus, it pass an argument to `bc` as an input file.
- After each execution, it sorts the results (stdout, stderr, exitcode non-zero(fault)), counts them, and saves thier indexes as an array.


## Bugs Fuzzers Find
#### 2_bug_fuzzer_find.c
It lists the bugs that fuzzers could find, and simulates that scenarios.
1. Buffer overflow
2. Missing error checks
3. Rogue numbers


## Catching Errors

### Generic Checkers
#### 3_checking_mem_access.c
It is a simple program to practice complilling with memory sanitizer and check the result.
```
$ clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c
$ ./checking_mem_access 99; echo $?
$ ./checking_mem_access 110
```

#### 4_information_leaks.c
