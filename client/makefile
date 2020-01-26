all: jeu

debug: src/app.c game.o gameCell.o net.o
	gcc obj/*.o src/app.c -o bin/app.exe -lSDL2 -lpthread -D_DEBUG

game.o: src/game.c
	gcc -c src/game.c -o obj/game.o

gameCell.o: src/gameCell.c
	gcc -c src/gameCell.c -o obj/gameCell.o

net.o: src/net.c
	gcc -c src/net.c -o obj/net.o -lpthread

jeu: src/app.c game.o gameCell.o net.o
	gcc obj/*.o src/app.c -o bin/app.exe -lSDL2 -lpthread

static: src/app.c game.o gameCell.o net.o
	gcc obj/*.o src/app.c -o bin/app.st.exe -lpthread -lSDL2main -lSDL2 -lm -static-libgcc

clean:
	rm obj/*.o
	rm bin/*.exe