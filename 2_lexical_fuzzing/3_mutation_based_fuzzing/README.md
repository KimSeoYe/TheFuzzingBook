# Mutation-Based Fuzzing
Mutation-Based Fuzzing Architecture

### src/mutate.c
- mutation operators
  ```c
  int delete_random_character (char * dst, char * seed, int seed_len) ;
  int insert_random_character (char * dst, char * seed, int seed_len) ;
  int flip_random_character (char * dst, char * seed, int seed_len) ;
  ```
- mutate function
  ```c
  int mutate (char * dst, char * seed, int seed_len) ;
  ```
  : It randomly chooses a mutation operator.
- implementation to fuzzer
  ```c
  int mutate_input (char * dst, fuzarg_t * fuzargs, char * seed_filename) ;
  ```

## New target programs in lib\urlparse.c
It simply parses the given url and checks if it is valid url (w/ http | https).

## Testdrivers usage
```
$ ./[testdriver] -m [seed_dir] -t [trials]
```
When you give -m option, the fuzzer reads seeds from seed_dir (using Round-Robin algorithm), mutates it, and uses it as a input.

Example)
```
$ ./urlparse_testdriver -m seed_urlparse -t 3

File '/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/3_mutation_based_fuzzing/fuzzing_architecture/lib/urlparse.c'
Lines executed:0.00% of 42
Branches executed:0.00% of 12
Taken at least once:0.00% of 12
No calls
Creating 'urlparse.c.gcov'

...

(CompletedProcess(target='/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/3_mutation_based_fuzzing/fuzzing_architecture/lib/urlparse', args='', line_coverage='34', branch_coverage='12', returncode='0', input='htts://www.fuzzingbook.org/html', stdout='INPUT: htts://www.fuzzingbook.o', stderr='http_program: scheme must be on', result='PASS'))
(CompletedProcess(target='/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/3_mutation_based_fuzzing/fuzzing_architecture/lib/urlparse', args='', line_coverage='32', branch_coverage='10', returncode='0', input='http://www.naver.cYom', stdout='INPUT: http://www.naver.cYom
', stderr='', result='PASS'))
(CompletedProcess(target='/Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/3_mutation_based_fuzzing/fuzzing_architecture/lib/urlparse', args='', line_coverage='32', branch_coverage='10', returncode='0', input='http://www.google.com/search?=f', stdout='INPUT: http://www.google.com/se', stderr='', result='PASS'))


=======================================================
TOTAL SUMMARY
=======================================================
# TRIALS : 3
# EXEC TIME : 4233 ms
# LINE COVERED : 10 / 42 = 23.8%
# BRANCH COVERED : 12 / 12 = 100.0%
# PASS : 3
# FAIL : 0
# UNRESOLVED : 0
=======================================================
```

