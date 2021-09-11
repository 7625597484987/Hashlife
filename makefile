COMPILE = gcc -Wall
SDLLIBS = `sdl2-config --cflags --libs` -lSDL2 -lSDL2_ttf

functions.o: code/functions.c code/functions.h
	$(COMPILE) -c code/functions.c $@

display.o: code/display.c code/display.h code/functions.o
	$(COMPILE) -c code/display.c -o $@ `sdl2-config --cflags --libs`

hashlife.o: hashlife.c code/display.o code/functions.o
	$(COMPILE) -c hashlife.c -o $@

hashlife: hashlife.o code/display.o code/functions.o
	$(COMPILE) hashlife.o code/display.o code/functions.o -o $@ $(SDLLIBS)

clean: 
	rm -f hashlife hashlife.o