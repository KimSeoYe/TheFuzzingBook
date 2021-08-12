TARGETS=cat_testdriver.c 
EXECS=cat_testdriver
OBJS=config.o fuzzer.o fuzz_input.o run.o 
SRCS=../src/config.c ../src/fuzzer.c ../src/fuzz_input.c ../src/run.c 

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c ../src/run.c 

cat_testdriver: cat_testdriver.c ../src/fuzzer.c ../src/input_fuzz.c ../src/run.c
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c ../src/run.c 

clean:
	rm -rf $(EXECS) $(OBJS)
