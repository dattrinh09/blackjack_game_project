project: server.c client.c
	gcc -o server server.c -pthread
	gcc -o client client.c
clear:
	rm -f *.o *~