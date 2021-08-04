all: fuzzer_test.c invoking_extern_prog.c fileio.c fuzzer.c
	gcc -o fuzzer_test fuzzer_test.c fileio.c fuzzer.c
	gcc -o invoking_extern_prog invoking_extern_prog.c fileio.c fuzzer.c

fuzzer_test: fuzzer_test.c fileio.c fuzzer.c
	gcc -o fuzzer_test fuzzer_test.c fileio.c fuzzer.c

invoking_extern_prog: invoking_extern_prog.c fileio.c fuzzer.c
	gcc -o invoking_extern_prog invoking_extern_prog.c fileio.c fuzzer.c

clean:
	rm -rf fuzzer_test invoking_extern_prog fuzzer.o fileio.o