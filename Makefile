all: advance check winnable game

game: part4.c
	gcc -I/share/cs327/include -o game part4.c /share/cs327/lib/libtermbox.a

winnable: part3.c
	gcc -o winnable part3.c

advance: part2.c
	gcc -o advance part2.c

check: part1.c
	gcc -o check part1.c

clean:
	echo "Cleaning."; rm advance part2.o