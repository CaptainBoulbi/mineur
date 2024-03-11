all:
	gcc -Wall -Wextra -ggdb main.c -o mineur -lraylib

run: all
	./mineur

clean:
	rm -f mineur
