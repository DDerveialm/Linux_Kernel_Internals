test1: mergesort.c
	gcc -o test1 mergesort.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

test2: power_of_2.c
	gcc -o test2 power_of_2.c #-Wall -Wextra -Wshadow -g -fsanitize=address,undefined

test3: bitcpy.c
	gcc -o test3 bitcpy.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

test4: cstr.h cstr.c str_intern.c
	gcc -c cstr.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined
	gcc -c str_intern.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined
	gcc -o test4 cstr.o str_intern.o -Wall -Wextra -Wshadow -g -fsanitize=address,undefined


clean:
	rm test1 test2 test3 test4 *.o
