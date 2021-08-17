TARGETS=manual_test.c black_box_testing.c
EXECS=manual_test black_box_testing
OBJS=cgi_decode.o
SRCS=../src/cgi_decode.c

all: $(TARGETS) $(SRCS)
	gcc -o manual_test manual_test.c ../src/cgi_decode.c
	gcc -o black_box_testing black_box_testing.c ../src/cgi_decode.c

manual_test: manual_test.c ../src/cgi_decode.c
	gcc -o manual_test manual_test.c ../src/cgi_decode.c

black_box_testing: black_box_testing.c ../src/cgi_decode.c
	gcc -o black_box_testing black_box_testing.c ../src/cgi_decode.c

clean:
	rm -rf $(EXECS) $(OBJS)