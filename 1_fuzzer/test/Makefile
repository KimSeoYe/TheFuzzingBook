TARGETS=0_fuzzer_test.c 1_invoking_extern_prog.c 2_bug_fuzzers_find.c 4_information_leaks.c
EXECS=fuzzer_test invoking_extern_prog bug_fuzzers_find information_leaks

all: $(TARGETS) ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/fuzzer.c
	# clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c
	gcc -o information_leaks 4_information_leaks.c ../src/fuzzer.c

fuzzer_test: 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c

invoking_extern_prog: 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c

bug_fuzzers_find: 2_bug_fuzzers_find.c ../src/fuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/fuzzer.c

information_leaks: 4_information_leaks.c ../src/fuzzer.c
	gcc -o information_leaks 4_information_leaks.c ../src/fuzzer.c

clean:
	rm -rf $(EXECS) fuzzer.o fileio.o