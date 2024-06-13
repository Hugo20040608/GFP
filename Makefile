all:
	gcc main.c fiction.c toml.c music.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o game
run:
	./game
clean:
	rm game