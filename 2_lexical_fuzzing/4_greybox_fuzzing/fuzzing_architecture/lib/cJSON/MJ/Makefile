CC=gcc
TARGETS=MJ_testCJSON.c 
LIBS=cJSON.c 
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=MJ_testCJSON
GCOVS=*.gcno *.gcda *.gcov 

all: $(TARGETS) $(LIBS)
	$(CC) -o $(EXECS) $(TARGETS) $(LIBS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
