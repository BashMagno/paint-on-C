all:
	gcc -I src/include -L src/lib -o paint paint.c -lmingw32 -lSDL2main -lSDL2
