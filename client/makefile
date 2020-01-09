all: jeu

game.o: src/game.c
	gcc -c src/game.c -o obj/game.o

gameCell.o: src/gameCell.c
	gcc -c src/gameCell.c -o obj/gameCell.o

jeu: src/app.c game.o gameCell.o
	gcc obj/*.o src/app.c -o bin/app.exe -lSDL2

clean:
	rm obj/*.o
	rm bin/*.exe