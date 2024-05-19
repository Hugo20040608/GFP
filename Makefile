all:
	gcc main.c -lSDL2 -o game
run:
	./game
clean:
	rm game