This is a [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) language interpreter, developed by Joseph Weisbecker back in mid-1970s.

You may find samples games taken from this [repo](https://github.com/netpro2k/Chip8/blob/master/games/Pong.ch8), in order to run a game:

1. Build the interpreter

```sh
cmake -B build && cmake --build build/
```

2. Choose a game and pass it to the interpreter

```sh
./build/chip_8 <path_to_game>
```
