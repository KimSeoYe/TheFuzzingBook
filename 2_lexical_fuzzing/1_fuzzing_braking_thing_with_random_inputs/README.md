# Fuzzing: Breaking Things with Random Inputs

## Simple fuzzer

### Creating Input Files

#### src/fileio.c & include/fileio.h
It has some functions for I/O executions. 
#### src/fuzzer.c & include/fuzzer.h
It has a simple fuzzer.
<!-- TODO. add a detailed description -->

#### test/0_fuzzer_test.c
It creates a random string using fuzzer and write to the file `input.txt` under a unique temporary directory.

## Fuzzing External Programs with a Simple Fuzzer

### Invoking External Program
#### test/1_invoking_extern_prog.c
It is for the chapter *Invoking External Programs* and *Long Running Fuzzing*.
The function `invoking_exter_prog()` simply invoke `bc`.
The most important is `long_running_fuzzing()`.
- It executes `bc` 100 times using the simple fuzzer, and checks the results.
- It creates one input file and one output file for each execution (total: 100 input files & 100 ouput files).
  - Because we can find and use meaningful input later on.
  - Thus, it pass an argument to `bc` as an input file.
- After each execution, it sorts the results (stdout, stderr, exitcode non-zero(fault)), counts them, and saves thier indexes as an array.


## Bugs Fuzzers Find
#### test/2_bug_fuzzer_find.c
It lists the bugs that fuzzers could find, and simulates that scenarios.
1. Buffer overflow
2. Missing error checks
3. Rogue numbers


## Catching Errors

### Generic Checkers
#### test/3_checking_mem_access.c
It is a simple program to practice complilling with memory sanitizer to check out-of-bound memory access.
```
$ clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c
$ ./checking_mem_access 99; echo $?
$ ./checking_mem_access 110
```

#### test/4_information_leaks.c
It simulates a simple Heartbleed-Bug like scenario.<br>
The program to be tested is `heartbeats` in `src/heartbeats.c` and `include/heartbeats.h`, which takes its name from the SSL *heartbeat* service. It gets a messege to reply to the caller, replace the front part of its fake memory, then returns a string as long as requested.<br>
Thus, it tests `heartbeats()` manually and randomly with a fuzzer.


### Program-Specific Checkers

#### src/airport_code.c & include/airport_code.h

#### test/5_program_specific_chekers.c