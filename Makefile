main:
	gcc src/*.c -I./include/ -o main -Werror -Wpedantic -Wall

clean:
	rm main
