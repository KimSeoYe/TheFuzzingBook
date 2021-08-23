# Code Coverage

## Fuzzing Architecture w/ Code Coverage

### include/config.h & src/config.c

- test_config.option : Fuzzing option : fuzz stdin | argument | file contents
- test_config.fuzzed_args_num : The number of arguments you want to fuzz
- test_config.is_source : It indicates if the target is c source file (0 or 1)
- test_config.source_path : If is_source is true, you need to pass the source path here

### src/fuzzer.h

- content_t : Now the fuzzer save input/output contents by CONTENTS_MAX bytes

### src/fuzzer.c & src/gcov_runner.c & include/gcov_runner.h

If you choose a c source file as a target, the fuzzer compiles it with `--coverage` and gets coverage summary (both line and branch).<br>
Below is an example of execution results for target `lib/cgi_decode.c`.

```
...
File '/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/2_code_coverage/fuzzing_architecture/lib/cgi_decode.c'
Lines executed:35.59% of 59
Branches executed:28.57% of 42
Taken at least once:16.67% of 42
No calls
Creating 'cgi_decode.c.gcov'
...
...
(CompletedProcess(target='/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/2_code_coverage/fuzzing_architecture/lib/cgi_decode', args='', line_coverage='21', branch_coverage='12', returncode='0', input = '��@t-��z�k��3����~��9zYB�ux�', stdout='��@t-��z�k��3����~��9zYB�ux�', stderr='', result='PASS'))
(CompletedProcess(target='/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/2_code_coverage/fuzzing_architecture/lib/cgi_decode', args='', line_coverage='25', branch_coverage='20', returncode='256', input = '�o��      �F�ۦ��]dw}�dk̏�x;d��%', stdout='', stderr='cgi_decode: Invalid encoding: U', result='FAIL'))
...

=======================================================
TOTAL SUMMARY
=======================================================
# TRIALS : 100
# EXEC TIME : 118503 ms
# LINE COVERED : 34 / 97 = 35%
# BRANCH COVERED : 28 / 42 = 67%
# PASS : 72
# FAIL : 28
# UNRESOLVED : 0
=======================================================
```