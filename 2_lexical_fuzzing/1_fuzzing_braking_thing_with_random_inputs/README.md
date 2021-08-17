# Fuzzing: Breaking Things with Random Inputs

<!-- How To Build and Run -->

## Simple fuzzer

### Creating Input Files

#### src/fileio.c & include/fileio.h
It has some functions for I/O executions. 
#### src/simplefuzzer.c & include/simplefuzzer.h
It has a simple fuzzer that generates a random string.<br>
You need to use `srand()` before using fuzzer.

#### test/0_fuzzer_test.c
It creates a random string using fuzzer and write to the file `input.txt` under a unique temporary directory.

## Fuzzing External Programs with a Simple Fuzzer

### Invoking External Program & Long Running Fuzzing

#### test/1_invoking_extern_prog.c

**invoking_exter_prog()** : simply invoke `bc`.

**long_running_fuzzing()**
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

#### src/heartbeats.c & include/heartbeats.h
It is a program to be tested, which takes its name from the SSL *heartbeat* service. It gets a messege to reply to the caller, replaces the front part of its fake memory, then returns a string as long as requested.

#### test/4_information_leaks.c
It simulates a simple HeartBleed-Bug like scenario.<br>
Thus, it tests `heartbeats()` manually, and also tests randomly with a fuzzer.


### Program-Specific Checkers

#### src/airport_code.c & include/airport_code.h
In airport_code.h, a structure `Node` for linked list is declared.<br>
It also has some functions to maintain and manipulate a linked list.<br>
Moreover, it has functions for checking its own consistency by checking important conditions.

#### test/5_program_specific_chekers.c
This is a program for testing if the `airport_code` works well, by executing its built-in functions.


## A Fuzzing Architecture

### fuzzing_architecture/

#### include/config.h & src/config.c
It has a structure `test_config` which has some information for running a fuzzer.<br>
Also, there is a function `init_config()` for test driver to set default values of `test_config`.

#### src/fuzzer.c
It is a generic random fuzzer. It has a main loop which includes input creation part, execution part, and result checking part.

#### test/cat_testdriver.c
It is a simple testdriver for specific program `cat`.

#### program/troff_bug_simulate.c & test/troff_test_driver.c
`troff_bug_simulate` is a simple program to simulate the bug scenario of early `troff`.<br>
`troff_test_driver.c` is a simplest test driver to test the `troff_bug_simulate`.

#### test/hunspell_test_driver.c
It is a simple test driver for [hunspell](https://github.com/hunspell/hunspell).