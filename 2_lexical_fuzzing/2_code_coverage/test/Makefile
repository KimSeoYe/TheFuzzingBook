TARGETS=manual_testing.c blackbox_testing.c whitebox_testing.c ../lib/cgi_decode.c
EXECS=manual_testing blackbox_testing whitebox_testing ../lib/cgi_decode
OBJS=cgi_decode.o
SRCS=../src/cgi_decode.c

all: $(TARGETS) $(SRCS)
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c
	gcc -o blackbox_testing blackbox_testing.c ../src/cgi_decode.c
	gcc -o whitebox_testing whitebox_testing.c 

manual_testing: manual_testing.c ../src/cgi_decode.c
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c

blackbox_testing: blackbox_testing.c ../src/cgi_decode.c
	gcc -o blackbox_testing blackbox_testing.c ../src/cgi_decode.c

whitebox_testing: whitebox_testing.c 
	gcc -o whitebox_testing whitebox_testing.c 

clean:
	rm -rf $(EXECS) $(OBJS) ./cgi_decode.*