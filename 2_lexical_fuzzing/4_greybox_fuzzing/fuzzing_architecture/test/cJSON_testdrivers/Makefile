CC=gcc
TARGETS=parse_testdriver.c cjson_testdriver.c JI_testdriver.c MJ_testdriver.c JY_testdriver.c WS_testdriver.c
EXECS=parse_testdriver cjson_testdriver JI_testdriver MJ_testdriver JY_testdriver WS_testdriver
OBJS=config.o fuzzer.o fuzz_input.o gcov_runner.o mutate.o
SRCS= ../../src/config.c  ../../src/fuzzer.c  ../../src/fuzz_input.c  ../../src/gcov_runner.c  ../../src/mutate.c

all: $(TARGETS) $(SRCS)
	$(CC) -o cjson_testdriver cjson_testdriver.c $(SRCS)
	$(CC) -o JI_testdriver JI_testdriver.c $(SRCS)
	$(CC) -o MJ_testdriver MJ_testdriver.c $(SRCS)

cjson_testdriver: cjson_testdriver.c $(SRCS)
	$(CC) -o cjson_testdriver cjson_testdriver.c $(SRCS)

JI_testdriver: JI_testdriver.c $(SRCS)
	$(CC) -o JI_testdriver JI_testdriver.c $(SRCS)

MJ_testdriver: MJ_testdriver.c $(SRCS)
	$(CC) -o MJ_testdriver MJ_testdriver.c $(SRCS)

JY_testdriver: JY_testdriver.c $(SRCS)
	$(CC) -o JY_testdriver JY_testdriver.c $(SRCS)

WS_testdriver: WS_testdriver.c $(SRCS)
	$(CC) -o WS_testdriver WS_testdriver.c $(SRCS)

clean:
	rm -rf $(EXECS) $(OBJS) *.csv tmp.* seed_cjson/mutated_* seed_JI/mutated_* seed_JY/mutated_* seed_MJ/mutated_* seed_WS/mutated_*
