CC=gcc
TARGETS=WS_cjson_test.c 
LIBS=cJSON.c 
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=WS_cjson_test
GCOVS=*.gcno *.gcda *.gcov 

all: $(TARGETS) $(LIBS)
	$(CC) -o $(EXECS) $(TARGETS) $(LIBS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
