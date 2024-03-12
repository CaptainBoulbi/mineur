all:
	gcc -Wall -Wextra -ggdb main.c -o mineur -lraylib

release:
	gcc -Wall -Wextra -O3 main.c -o mineur -lraylib

run: all
	./mineur

clean:
	rm -f mineur
