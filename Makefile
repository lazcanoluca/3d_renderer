CC = gcc
CFLAGS = -Wall -Werror -std=c99 -I./lib
LDFLAGS = -L./lib -larray -lSDL2 -lm
SRC = ./src/*.c
TARGET = renderer

clean-build-run: clean build run

build: ./lib/libarray.a
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

./lib/libarray.a: ./lib/array.c
	$(CC) $(CFLAGS) -c $< -o ./lib/array.o
	ar rcs $@ ./lib/array.o

run:
	./$(TARGET)

clean:
	rm -f $(TARGET) ./lib/array.o ./lib/libarray.a


