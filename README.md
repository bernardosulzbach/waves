# Waves

Geometric waves using C and SDL.

A simple `bash run.sh` suffices right now.

## Requirements

You will need the SDL 2.0 development library in order to compile the program,
assuming you already have GCC and its friends installed.

> In Fedora 23 64-bits, that is `SDL2-devel-2.0.3-7.fc23.x86_64`

## Commands

The keys 1 through 8 select one of the eight possible oscillators. If that
oscillator is not active, it is created and added at the origin.

The arrow keys move the selected oscillator around.

Pressing `Delete` will delete the currently selected oscillator.
