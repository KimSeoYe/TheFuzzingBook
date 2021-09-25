TARGETS=cat_testdriver.c troff_testdriver.c hunspell_testdriver.c cgi_decode_testdriver.c urlparse_testdriver.c bc_testdriver.c
EXECS=cat_testdriver troff_testdriver hunspell_testdriver cgi_decode_testdriver urlparse_testdriver bc_testdriver
OBJS=config.o fuzzer.o fuzz_input.o gcov_runner.o mutate.o
SRCS=../src/config.c ../src/fuzzer.c ../src/fuzz_input.c ../src/gcov_runner.c ../src/mutate.c

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c $(SRCS)
	gcc -o troff_testdriver troff_testdriver.c $(SRCS)
	gcc -o hunspell_testdriver hunspell_testdriver.c $(SRCS)
	gcc -o cgi_decode_testdriver cgi_decode_testdriver.c $(SRCS)
	gcc -o urlparse_testdriver urlparse_testdriver.c $(SRCS)
	gcc -o bc_testdriver bc_testdriver.c $(SRCS)

cat_testdriver: cat_testdriver.c $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c $(SRCS)

troff_testdriver: troff_testdriver.c $(SRCS)
	gcc -o troff_testdriver troff_testdriver.c $(SRCS)

hunspell_testdriver: hunspell_testdriver.c $(SRCS)
	wget -O en_US.aff  https://cgit.freedesktop.org/libreoffice/dictionaries/plain/en/en_US.aff?id=a4473e06b56bfe35187e302754f6baaa8d75e54f
	wget -O en_US.dic https://cgit.freedesktop.org/libreoffice/dictionaries/plain/en/en_US.dic?id=a4473e06b56bfe35187e302754f6baaa8d75e54f
	gcc -o hunspell_testdriver hunspell_testdriver.c $(SRCS)

cgi_decode_testdriver: cgi_decode_testdriver.c $(SRCS)
	gcc -o cgi_decode_testdriver cgi_decode_testdriver.c $(SRCS)

urlparse_testdriver: urlparse_testdriver.c $(SRCS)
	gcc -o urlparse_testdriver urlparse_testdriver.c $(SRCS)

bc_testdriver: bc_testdriver.c $(SRCS)
	gcc -o bc_testdriver bc_testdriver.c $(SRCS)

clean:
	rm -rf $(EXECS) $(OBJS) en_US.*
