all: my_sqrt_test.c exercise2.c exercise3.c ../src/my_sqrt.c ../src/shellsort.c ../src/quadratic_solver.c
	gcc -o my_sqrt_test my_sqrt_test.c ../src/my_sqrt.c
	gcc -o exercise2 exercise2.c ../src/shellsort.c
	gcc -o exercise3 exercise3.c ../src/quadratic_solver.c

my_sqrt_test:
	gcc -o my_sqrt_test my_sqrt_test.c ../src/my_sqrt.c

exercise2: exercise2.c
	gcc -o exercise2 exercise2.c ../src/shellsort.c

exercise3: exercise3.c
	gcc -o exercise3 exercise3.c ../src/quadratic_solver.c 

clean:
	rm -rf my_sqrt_test exercise2 exercise3 my_sqrt.o shellsort.o quadratic_solver.o