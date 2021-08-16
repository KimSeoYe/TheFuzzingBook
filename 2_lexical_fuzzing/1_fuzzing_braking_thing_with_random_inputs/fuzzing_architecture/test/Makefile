TARGETS=cat_testdriver.c ../program/troff_bug_simulate.c troff_test_driver.c
EXECS=cat_testdriver ../program/troff_bug_simulate troff_test_driver
OBJS=config.o fuzzer.o fuzz_input.o
SRCS=../src/config.c ../src/fuzzer.c ../src/fuzz_input.c

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

cat_testdriver: cat_testdriver.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

troff_bug_simulate: ../program/troff_bug_simulate.c 
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 

troff_test_driver: ../program/troff_bug_simulate.c troff_test_driver.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

clean:
	rm -rf $(EXECS) $(OBJS)
