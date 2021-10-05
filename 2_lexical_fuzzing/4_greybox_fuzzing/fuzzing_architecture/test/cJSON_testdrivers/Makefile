CC=gcc
TARGETS=parse_testdriver.c cjson_testdriver.c
EXECS=parse_testdriver cjson_testdriver
OBJS=config.o fuzzer.o fuzz_input.o gcov_runner.o mutate.o
SRCS= ../../src/config.c  ../../src/fuzzer.c  ../../src/fuzz_input.c  ../../src/gcov_runner.c  ../../src/mutate.c

all: $(TARGETS) $(SRCS)
	$(CC) -o parse_testdriver parse_testdriver.c $(SRCS)
	$(CC) -o cjson_testdriver cjson_testdriver.c $(SRCS)
	
parse_testdriver: parse_testdriver.c $(SRCS)
	$(CC) -o parse_testdriver parse_testdriver.c $(SRCS)

cjson_testdriver: cjson_testdriver.c $(SRCS)
	$(CC) -o cjson_testdriver cjson_testdriver.c $(SRCS)

clean:
	rm -rf $(EXECS) $(OBJS)
