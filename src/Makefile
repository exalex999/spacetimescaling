CC=gcc

all:spacetimescaling

main.o: main.c
	$(CC) -c main.c -lm
dataio.o: dataio.c
	$(CC) -c dataio.c `sdl2-config --cflags` -lSDL2_image -lm
data_and_interpolation.o: data_and_interpolation.c
	$(CC) -c data_and_interpolation.c `sdl2-config --cflags` -lSDL2_image -lm
spacetimescaling: main.o dataio.o data_and_interpolation.o
	$(CC) main.o dataio.o data_and_interpolation.o -o spacetimescaling `sdl2-config --libs` -lSDL2_image -lm
clean: 
	rm *.o
