main: server3.o
	gcc -Wall server3.o -o main -lpthread -lm


main1: server1.o
	gcc -Wall server1.o -o main1 -lpthread -lm

main2: server2.o
	gcc -Wall server2.o -o main2 -lpthread -lm

main3: server.o
	gcc -Wall server.o -o main3 -lpthread -lm


server.o: server.c
	gcc -c server.c

server1.o: server1.c
	gcc -c server1.c

server2.o: server2.c
	gcc -c server2.c

server3.o: server3.c
	gcc -c server3.c

clean:
	rm *.o main main1 main2 main3

all: main main1 main2 main3

