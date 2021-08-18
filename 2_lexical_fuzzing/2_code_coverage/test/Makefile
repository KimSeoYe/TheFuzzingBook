TARGETS=manual_testing.c blackbox_testing.c get_coverage.c ../lib/cgi_decode.c
EXECS=manual_testing blackbox_testing get_coverage ../lib/cgi_decode
OBJS=cgi_decode.o
SRCS=../src/cgi_decode.c

all: $(TARGETS) $(SRCS)
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c
	gcc -o blackbox_testing blackbox_testing.c ../src/cgi_decode.c
	gcc -o get_coverage get_coverage.c 

manual_testing: manual_testing.c ../src/cgi_decode.c
	gcc -o manual_testing manual_testing.c ../src/cgi_decode.c

blackbox_testing: blackbox_testing.c ../src/cgi_decode.c
	gcc -o blackbox_testing blackbox_testing.c ../src/cgi_decode.c

get_coverage: get_coverage.c 
	gcc -o get_coverage get_coverage.c 

clean:
	rm -rf $(EXECS) $(OBJS) ./cgi_decode.*