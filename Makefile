program:
	gcc -Wall -Werror -Wextra -pedantic -O4 -o program code.c
program_asan:
	gcc -Wall -Werror -Wextra -pedantic -g -O0 -fsanitize=address -o program code.c
clean:
	rm program
