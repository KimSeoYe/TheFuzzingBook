TARGETS=0_fuzzer_test.c 1_invoking_extern_prog.c 2_bug_fuzzers_find.c
EXECS=fuzzer_test invoking_extern_prog bug_fuzzers_find

all: $(TARGETS) ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/fuzzer.c
	# clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c

fuzzer_test: 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c

invoking_extern_prog: 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c

clean:
	rm -rf $(EXECS) fuzzer.o fileio.o