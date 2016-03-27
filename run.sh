gcc -o waves.o -Wall waves.c `sdl2-config --cflags --libs` -lm
./waves.o
rm waves.o
