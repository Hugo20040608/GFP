all:
	gcc main.c fiction.c toml.c -lSDL2 -lSDL2_image -lSDL2_ttf -o game
run:
	./game
clean:
	rm game