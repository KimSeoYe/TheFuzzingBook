TARGETS=cat_testdriver.c ../program/troff_bug_simulate.c troff_test_driver.c hunspell_test_driver.c
EXECS=cat_testdriver ../program/troff_bug_simulate troff_test_driver hunspell_test_driver
OBJS=config.o fuzzer.o fuzz_input.o
SRCS=../src/config.c ../src/fuzzer.c ../src/fuzz_input.c

all: $(TARGETS) $(SRCS)
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 
	gcc -o hunspell_test_driver hunspell_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c

cat_testdriver: cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o cat_testdriver cat_testdriver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

troff_bug_simulate: ../program/troff_bug_simulate.c 
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 

troff_test_driver: ../program/troff_bug_simulate.c troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c
	gcc -o ../program/troff_bug_simulate ../program/troff_bug_simulate.c 
	gcc -o troff_test_driver troff_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c 

hunspell_test_driver: hunspell_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c
	wget -O en_US.aff  https://cgit.freedesktop.org/libreoffice/dictionaries/plain/en/en_US.aff?id=a4473e06b56bfe35187e302754f6baaa8d75e54f
	wget -O en_US.dic https://cgit.freedesktop.org/libreoffice/dictionaries/plain/en/en_US.dic?id=a4473e06b56bfe35187e302754f6baaa8d75e54f
	gcc -o hunspell_test_driver hunspell_test_driver.c ../src/config.c ../src/fuzzer.c ../src/fuzz_input.c

clean:
	rm -rf $(EXECS) $(OBJS) en_US.*
