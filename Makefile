CC=g++

CFLAGS=-c -std=c++14

all: hw2_output.o main.o
	$(CC)  hw2_output.o main.o  -o hw2 -lpthread

main.o: main.cpp hw2_output.h
	$(CC) $(CFLAGS) main.cpp -o main.o

hw2_output.o: hw2_output.c hw2_output.h
	$(CC) $(CFLAGS) hw2_output.c -o hw2_output.o

clean:
	rm -f *.o
	rm hw2

sena:
	make all
	./mgame < input_example.txt
