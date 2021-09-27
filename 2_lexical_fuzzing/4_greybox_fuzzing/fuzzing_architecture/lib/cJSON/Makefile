CC=gcc
TARGETS=monitor_example.c parse_test.c
LIBS=cJSON.c
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=monitor_example parse_test
GCOVS=*.gcno *.gcda *.gcov

all: $(TARGETS) $(LIBS)
	$(CC) -o monitor_example monitor_example.c $(LIBS) 
	$(CC) -o parse_test parse_test.c $(LIBS) $(CFLAGS)

monitor_example: monitor_example.c $(LIBS)
	$(CC) -o monitor_example monitor_example.c $(LIBS) 

parse_test: parse_test.c $(LIBS)
	$(CC) -o parse_test parse_test.c $(LIBS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
