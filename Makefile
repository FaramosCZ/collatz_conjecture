program:
	gcc -Wall -Werror -Wextra -pedantic -Ofast -funroll-loops -o program code.c
program_asan:
	gcc -Wall -Werror -Wextra -pedantic -g -O0 -fsanitize=address -o program code.c
clean:
	rm -f program
