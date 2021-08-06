TARGETS=0_fuzzer_test.c 1_invoking_extern_prog.c 2_bug_fuzzers_find.c 4_information_leaks.c 5_program_specific_checkers.c
EXECS=fuzzer_test invoking_extern_prog bug_fuzzers_find information_leaks program_specific_checkers
OBJS=filio.o fuzzer.o airport_code.o

all: $(TARGETS) ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/fuzzer.c
	# clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c
	gcc -o information_leaks 4_information_leaks.c ../src/fuzzer.c
	gcc -o program_specific_checkers 5_program_specific_checkers.c ../src/airport_code.c

fuzzer_test: 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/fuzzer.c

invoking_extern_prog: 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/fuzzer.c

bug_fuzzers_find: 2_bug_fuzzers_find.c ../src/fuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/fuzzer.c

information_leaks: 4_information_leaks.c ../src/fuzzer.c
	gcc -o information_leaks 4_information_leaks.c ../src/fuzzer.c

program_specific_checkers: 5_program_specific_checkers.c ../src/airport_code.c
	gcc -o program_specific_checkers 5_program_specific_checkers.c ../src/airport_code.c

clean:
	rm -rf $(EXECS) $(OBJS)