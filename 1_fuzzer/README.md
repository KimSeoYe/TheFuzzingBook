# Fuzzing: Breaking Things with Random Inputs

## Fuzzing External Programs with a Simple Fuzzer

### src & include

`fileio` : It has some functions for I/O executions. <br>
`fuzzer` : It has a simple fuzzer

### test

**`fuzzer_test`** <br>
It creates a random string using fuzzer and write to the file `input.txt` under a unique temporary directory.

**`invoking_extern_prog`** <br>
The function `invoking_exter_prog()` simply invoke `bc`.
The most important is `long_running_fuzzing()`.
- It executes `bc` 100 times using the simple fuzzer, and checks the results.
- It creates one input file and one output file for each execution (total: 100 input files & 100 ouput files).
  - Because we can find and use meaningful input later on.
  - Thus, it pass an argument to `bc` as an input file.
- After each execution, it sorts the results (stdout, stderr, exitcode non-zero(fault)), counts them, and saves thier indexes as an array.