TARGETS=url_test.c
EXECS=url_test
OBJS=urlparse.o mutating_input.o
SRCS=../src/mutating_input.c
LIBS=../lib/urlparse.c

all: $(TARGET) $(SRCS) $(LIBS)
	gcc -o url_test url_test.c ../src/mutating_input.c ../lib/urlparse.c

clean:
	rm -rf $(EXECS) $(OBJS)