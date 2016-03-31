gcc -D_GNU_SOURCE -o waves.o -Wall -O3 waves.c `sdl2-config --cflags --libs` -lm
./waves.o
rm waves.o
