TARGETS=cat_testdriver.c troff_test_driver.c
EXECS=cat_testdriver troff_test_driver
OBJS=config.o fuzzer.o fuzz_input.o
SRCS=../src/config.c ../src/fuzzer.c ../src/fuzz_input.c

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

cat_testdriver: cat_testdriver.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

troff_test_driver: troff_test_driver.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

clean:
	rm -rf $(EXECS) $(OBJS)
