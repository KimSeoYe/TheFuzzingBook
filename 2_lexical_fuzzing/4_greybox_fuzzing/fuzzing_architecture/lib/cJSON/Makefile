CC=gcc
TARGETS=parse_test.c json_test.c 
LIBS=cJSON.c
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=parse_test json_test
GCOVS=*.gcno *.gcda *.gcov

all: $(TARGETS) $(LIBS)
	$(CC) -o parse_test parse_test.c $(LIBS) $(CFLAGS)
	$(CC) -o json_test json_test.c $(LIBS) $(CFLAGS)

parse_test: parse_test.c $(LIBS)
	$(CC) -o parse_test parse_test.c $(LIBS) $(CFLAGS)

json_test: json_test.c $(LIBS)
	$(CC) -o json_test json_test.c $(LIBS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
