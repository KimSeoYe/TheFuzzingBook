TARGETS=0_fuzzer_test.c 1_invoking_extern_prog.c 2_bug_fuzzers_find.c 4_information_leaks.c 5_program_specific_checkers.c 6_runner_class.c
EXECS=fuzzer_test invoking_extern_prog bug_fuzzers_find information_leaks program_specific_checkers runner_class
OBJS=filio.o simplefuzzer.o heartbeats.o airport_code.o runner.o program_runner.o
SRCS=../src/fileio.c ../src/simplefuzzer.c ../src/heartbeats.c ../src/airport_code.c ../src/runner.c ../src/program_runner.c

all: $(TARGETS) $(SRCS)
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/simplefuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/simplefuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/simplefuzzer.c
	# clang -fsanitize=address -g -o checking_mem_access 3_checking_mem_access.c
	gcc -o information_leaks 4_information_leaks.c ../src/simplefuzzer.c ../src/heartbeats.c
	gcc -o program_specific_checkers 5_program_specific_checkers.c ../src/airport_code.c
	gcc -o runner_class 6_runner_class.c ../src/runner.c ../src/program_runner.c

fuzzer_test: 0_fuzzer_test.c ../src/fileio.c ../src/simplefuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c ../src/fileio.c ../src/simplefuzzer.c

invoking_extern_prog: 1_invoking_extern_prog.c ../src/fileio.c ../src/simplefuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c ../src/fileio.c ../src/simplefuzzer.c

bug_fuzzers_find: 2_bug_fuzzers_find.c ../src/simplefuzzer.c
	gcc -o bug_fuzzers_find 2_bug_fuzzers_find.c ../src/simplefuzzer.c

information_leaks: 4_information_leaks.c ../src/simplefuzzer.c
	gcc -o information_leaks 4_information_leaks.c ../src/simplefuzzer.c ../src/heartbeats.c

program_specific_checkers: 5_program_specific_checkers.c ../src/airport_code.c
	gcc -o program_specific_checkers 5_program_specific_checkers.c ../src/airport_code.c

runner_class: 6_runner_class.c ../src/runner.c ../src/program_runner.c
	gcc -o runner_class 6_runner_class.c ../src/runner.c ../src/program_runner.c

clean:
	rm -rf $(EXECS) $(OBJS)