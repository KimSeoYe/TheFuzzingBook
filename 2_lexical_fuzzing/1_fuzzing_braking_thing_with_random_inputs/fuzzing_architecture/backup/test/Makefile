TARGETS=runner_test.c 
EXECS=runner_test
OBJS=Runner.o PrintRunner.o ProgramRunner.o Fuzzer.o RandomFuzzer.o 
RUNNERS=../runner/Runner.c ../runner/PrintRunner.c ../runner/ProgramRunner.c
FUZZERS=../fuzzer/Fuzzer.c ../fuzzer/RandomFuzzer.c 

all: $(TARGETS) $(RUNNERS) $(FUZZERS)
	gcc -o runner_test runner_test.c ../runner/Runner.c ../runner/PrintRunner.c 

runner_test: runner_test.c ../runner/Runner.c ../runner/PrintRunner.c 
	gcc -o runner_test runner_test.c ../runner/Runner.c ../runner/PrintRunner.c 

clean: 
	rm -rf $(EXECS) $(OBJS)