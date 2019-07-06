![minidraw](http://dred.io/img/minidraw.png)

minidraw is a single file 2D graphics library. It's written in C89 (compilable as C++) and released
into the public domain.

This is still work-in-progress.


Features
========
- Path-based 2D drawing.
- Unicode based text itemization, shaping and placement.


Supported Platforms
===================
- GDI + Uniscribe (Windows XP+)
- Cairo + Pango


Building
========
Do the following in one source file:
```
#define MINIDRAW_IMPLEMENTATION
#include "minidraw.h"
```
Windows
-------
Just compile. There is no need to link to anything or specify any compiler flags.

Linux
-----
To enable the Cairo backend you will need to use the following compiler flags:
```
`pkg-config --cflags --libs cairo pango pangocairo` -ldl -lm
```