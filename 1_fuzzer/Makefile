all: 0_fuzzer_test.c 1_invoking_extern_prog.c fileio.c fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c fileio.c fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c fileio.c fuzzer.c

fuzzer_test: 0_fuzzer_test.c fileio.c fuzzer.c
	gcc -o fuzzer_test 0_fuzzer_test.c fileio.c fuzzer.c

invoking_extern_prog: 1_invoking_extern_prog.c fileio.c fuzzer.c
	gcc -o invoking_extern_prog 1_invoking_extern_prog.c fileio.c fuzzer.c

clean:
	rm -rf fuzzer_test invoking_extern_prog fuzzer.o fileio.o