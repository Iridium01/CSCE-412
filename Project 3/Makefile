CC = g++
CFLAGS = -Wall -std=c++17

all: lb
lb: main.o
	$(CC) $(CFLAGS) -o lb main.o

main.o: main.cpp Switch.h ini.h
	$(CC) $(CFLAGS) -c main.cpp



clean:
	rm -f *.o lb *.txt