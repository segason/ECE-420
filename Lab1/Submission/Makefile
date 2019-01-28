
main: main.o lab1_IO.o
	gcc -Wall main.o lab1_IO.o -o main -lpthread -lm

main.o: main.c
	gcc -c main.c

lab1_IO.o: lab1_IO.c
	gcc -c lab1_IO.c

clean:
	rm *.o main