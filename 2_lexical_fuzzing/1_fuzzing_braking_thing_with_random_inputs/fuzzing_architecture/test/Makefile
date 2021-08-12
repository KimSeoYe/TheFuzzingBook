TARGETS=cat_testdriver.c 
EXECS=cat_testdriver
OBJS=fuzzer.o input_fuzz.o run.o 
SRCS=../src/fuzzer.c ../src/input_fuzz.c ../src/run.c 

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c ../src/fuzzer.c ../src/input_fuzz.c ../src/run.c 

cat_testdriver: cat_testdriver.c ../src/fuzzer.c ../src/input_fuzz.c ../src/run.c
	gcc -o cat_testdriver cat_testdriver.c ../src/fuzzer.c ../src/input_fuzz.c ../src/run.c 

clean:
	rm -rf $(EXECS) $(OBJS)
