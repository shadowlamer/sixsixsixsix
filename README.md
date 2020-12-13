# 6.6.6.6

This is the second attempt to make a game for ZX Spectrum.

Long-long ago, between the world of darkness and the world of living people,
it was built - a line of defense, a trap for dark forces. The Net.
But from the blows of the creatures of darkness, cracks are already scattering across it.
If the defenses collapse, the world will end. The only hope is to repair the cliffs
and restore the fallen areas before the entire Network collapses.

But who can handle this task?

You?

The hero of the game is a rookie in the army of Light, protecting the sparkling The Net
from the intrigues of the Dark Forces, a fighter against darkness and chaos,
a technical support employee who first went to work. evil, restart the routers,
and turn off and then turn on the frozen equipment. Each task consists of two stages:
a bike ride, where you need to overcome a winding path in the allotted time,
and the answer to a question, with a choice of one of three options.
Five attempts, and the attempt is wasted both in the wrong answer to the question
and in the fall from the bike.

Let the Evil tremble! Our hero is already close!

P.S. Beware of puddles on the road. "

![Title image](assets/src/intro.jpg)
![Screenshot](screenshot.png)

## Prerequisities

1. [nodejs and npm](https://nodejs.org/en/download/) because assets preparing scripts written in JavaScript.
2. bin2c
3. [zmakebas](https://github.com/z00m128/zmakebas)
4. cmake
5. [sdcc](http://sdcc.sourceforge.net/)
6. [Fuse emulator](http://fuse-emulator.sourceforge.net/)

## Building

### Prepare assets

In a project root directory:
```
./assets.h
```

### Compile binary

In a project root directory:
```
mkdir build
cd build
cmake ..
make
```

### Compose tape file

In a project root directory:
```
./maketape.sh build
```
This will create tape file sixsixsixsix.tap in a project root directory.

## Playing

In a project root directory:
```
./emulate.sh
```

## Tools

There are some useful tools in tools directory. These tools are used to compile assets to C headers
and are called by assets.sh.

### Sprites extractor

Sprites are extracted from .scr files to C header files. Extractor can extract sprites from 
several files. Empty sprites are ignored.

Usage: 
```
sprites_extractor [options] file.scr [file2.scr ...]
```

Options:

| Short | Long      | Description             | Default   |
|-------|-----------|-------------------------|-----------|
|-o     | --outfile | out file                | sprites.h |
|-n     | --name    | array name              | sprites   |
|-p     | --prefix  | guard prefix            | _         |
|-w     | --width   | sprite width in bytes   | 2         |
|-h     | --height  | sprite height in bytes  | 16        |
|-?     | --help    | print usage information | false     |


### Tracks converter

Track data are extracted from .svg files. Just create simple .svg with one <path>, 
give a path name and run converter.

Usage: 

```
track_converter [options] file.svg [file2.svg ...]
```
Options:

| Short | Long        | Description             | Default   |
|-------|-------------|-------------------------|-----------|
|-d     | --directory | Output directory        | .         |
|-p     | --prefix    | Guard prefix            | __TRACK   |
|-h     | --help      | print usage information | false     |


### Tape composer

This tool actually has own [repo](https://github.com/shadowlamer/jsbin2tap) 
and npm [package](https://www.npmjs.com/package/jsbin2tap)

