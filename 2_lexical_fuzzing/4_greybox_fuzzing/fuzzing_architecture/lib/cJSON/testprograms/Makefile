CC=gcc
TARGETS=JI_json_parser.c JY_cjson_test.c MJ_testCJSON.c SY_json_test.c WS_cjson_test.c 
LIBS=cJSON.c
OBJS=cJSON.o
CFLAGS=--coverage
EXECS=JI_json_parser JY_cjson_test MJ_testCJSON SY_json_test WS_cjson_test 
GCOVS=*.gcno *.gcda *.gcov

all: $(TARGETS) $(LIBS)
	$(CC) -o JI_json_parser JI_json_parser.c $(LIBS) $(CFLAGS)
	$(CC) -o JY_cjson_test JY_cjson_test.c $(LIBS) $(CFLAGS)
	$(CC) -o MJ_testCJSON MJ_testCJSON.c $(LIBS) $(CFLAGS)
	$(CC) -o SY_json_test SY_json_test.c $(LIBS) $(CFLAGS)
	$(CC) -o WS_cjson_test WS_cjson_test.c $(LIBS) $(CFLAGS)

JI_json_parser: JI_json_parser.c $(LIBS)
	$(CC) -o JI_json_parser JI_json_parser.c $(LIBS) $(CFLAGS)

JY_cjson_test: JY_cjson_test.c $(LIBS)
	$(CC) -o JY_cjson_test JY_cjson_test.c $(LIBS) $(CFLAGS)

MJ_testCJSON: MJ_testCJSON.c $(LIBS)
	$(CC) -o MJ_testCJSON MJ_testCJSON.c $(LIBS) $(CFLAGS)

SY_json_test: SY_json_test.c $(LIBS)
	$(CC) -o SY_json_test SY_json_test.c $(LIBS) $(CFLAGS)

WS_cjson_test: WS_cjson_test.c $(LIBS)
	$(CC) -o WS_cjson_test WS_cjson_test.c $(LIBS) $(CFLAGS)
clean:
	rm -rf $(EXECS) $(OBJS) $(GCOVS)
