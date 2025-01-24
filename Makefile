clean-build-run: clean build run

build:
	gcc -Wall -Werror -std=c99 ./src/*.c -o renderer -lSDL2 -lm

run:
	./renderer

clean:
	rm -f renderer

