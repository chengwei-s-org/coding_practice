tetris:tetris.c
	gcc -o tetris tetris.c -lrt

.PHONY:clean
clean:
	rm -rf tetris
