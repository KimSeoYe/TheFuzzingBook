CC=gcc
CFLAGS=-lm
TARGETS=parse_testdriver.c cjson_testdriver.c testdriver_JI.c testdriver_MJ.c testdriver_JY.c testdriver_WS.c testdriver_SY.c
EXECS=parse_testdriver cjson_testdriver testdriver_JI testdriver_MJ testdriver_JY testdriver_WS testdriver_SY
OBJS=config.o fuzzer.o fuzz_input.o gcov_runner.o mutate.o
SRCS= ../../src/config.c  ../../src/fuzzer.c  ../../src/fuzz_input.c  ../../src/gcov_runner.c  ../../src/mutate.c

all: $(TARGETS) $(SRCS)
	$(CC) -o testdriver_SY testdriver_SY.c $(SRCS) $(CFLAGS)
	$(CC) -o testdriver_JI testdriver_JI.c $(SRCS) $(CFLAGS)
	$(CC) -o testdriver_MJ testdriver_MJ.c $(SRCS) $(CFLAGS)
	$(CC) -o testdriver_JY testdriver_JY.c $(SRCS) $(CFLAGS)
	$(CC) -o testdriver_WS testdriver_WS.c $(SRCS) $(CFLAGS)

testdriver_SY: testdriver_SY.c $(SRCS)
	$(CC) -o testdriver_SY testdriver_SY.c $(SRCS) $(CFLAGS)

testdriver_JI: testdriver_JI.c $(SRCS)
	$(CC) -o testdriver_JI testdriver_JI.c $(SRCS) $(CFLAGS)

testdriver_MJ: testdriver_MJ.c $(SRCS)
	$(CC) -o testdriver_MJ testdriver_MJ.c $(SRCS) $(CFLAGS)

testdriver_JY: testdriver_JY.c $(SRCS)
	$(CC) -o testdriver_JY testdriver_JY.c $(SRCS) $(CFLAGS)

testdriver_WS: testdriver_WS.c $(SRCS)
	$(CC) -o testdriver_WS testdriver_WS.c $(SRCS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) *.csv tmp.* 
	rm -rf seed_cjson/mutated_* testprogram_seeds/seed_SY/mutated_* testprogram_seeds/seed_JI/mutated_* testprogram_seeds/seed_JY/mutated_* testprogram_seeds/seed_MJ/mutated_* testprogram_seeds/seed_WS/mutated_*
