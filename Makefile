main:
	gcc src/*.c -I./include/ -o main #-Wpedantic -Wall #-Werror temp disabled due to pointer of type 'void *' used in arithmetic error

clean:
	rm main
