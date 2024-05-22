all:
	gcc main.c -lSDL2 -lSDL2_image -o game
run:
	./game
clean:
	rm game