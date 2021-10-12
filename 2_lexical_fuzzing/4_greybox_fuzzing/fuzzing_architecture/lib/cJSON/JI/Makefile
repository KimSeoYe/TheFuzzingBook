CC=gcc
TARGETS=JI_json_parser.c 
LIBS=cJSON.c 
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=JI_json_parser
GCOVS=*.gcno *.gcda *.gcov 

all: $(TARGETS) $(LIBS)
	$(CC) -o $(EXECS) $(TARGETS) $(LIBS) $(CFLAGS)

clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
