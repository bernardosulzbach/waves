# Waves

Geometric waves using C and SDL.

## Building Waves

```bash
$ ./autogen.sh
$ ./configure
$ make
```

## Requirements

You will need the SDL 2.0 development library in order to compile the program,
assuming you already have GCC and its friends installed.

> In Fedora 23 64-bits, that is `SDL2-devel-2.0.3-7.fc23.x86_64`

## Commands

### Selecting an oscillator

The keys `1`, `2`, `3`, `4`, `5`, `6`, `7`, and `8` select one of the eight
possible oscillators. If that oscillator is not active, it is created and added
at the origin.

### Moving an oscillator

The arrow keys move the selected oscillator around.

### Deleting oscillators

Pressing `Delete` will delete the currently selected oscillator.

### Changing the intensity of an oscillator

Pressing `+` and `-` will increase and decrease the intensity
of the selected oscillator, respectively.

> **Note**: this value has upper and lower bounds, so holding the key forever won't
> help much. *This is not a bug*.

### Dissipation model

Pressing `d` toggles the dissipation model.

In the terminal window, if the process is not detached, the dissipation model
changes will be printed.

> **Warning**: these dissipation models are visual approximations not meant to be
> physically accurate.

### Toggling recalculation

Pressing `r` toggles recalculation of the waves. This may be used to make the
program more responsive when one needs to move a lot of oscillators around.
