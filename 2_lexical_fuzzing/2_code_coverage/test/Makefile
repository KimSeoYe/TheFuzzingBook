TARGETS=manual_test.c
EXECS=manual_test
OBJS=cgi_decode.o
SRCS=../src/cgi_decode.c

all: $(TARGETS) $(SRCS)
	gcc -o manual_test manual_test.c ../src/cgi_decode.c

manual_test: manual_test.c ../src/cgi_decode.c
	gcc -o manual_test manual_test.c ../src/cgi_decode.c

clean:
	rm -rf $(EXECS) $(OBJS)