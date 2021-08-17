TARGETS=manual_testing.c black_box_testing.c white_box_testing.c
EXECS=manual_testing black_box_testing white_box_testing
OBJS=cgi_decode.o
SRCS=../src/cgi_decode.c

all: $(TARGETS) $(SRCS)
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c
	gcc -o black_box_testing black_box_testing.c ../src/cgi_decode.c
	gcc -o white_box_testing white_box_testing.c ../src/cgi_decode.c

manual_testing: manual_testing.c ../src/cgi_decode.c
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c

black_box_testing: black_box_testing.c ../src/cgi_decode.c
	gcc -o black_box_testing black_box_testing.c ../src/cgi_decode.c

white_box_testing: white_box_testing.c ../src/cgi_decode.c
	gcc -o white_box_testing white_box_testing.c ../src/cgi_decode.c

clean:
	rm -rf $(EXECS) $(OBJS)