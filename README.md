<h1 align="center">
    <img src="http://dred.io/img/minidraw.png" alt="minidraw" width="1280">
    <br>
</h1>

<h4 align="center">A single file path based 2D graphics library.</h4>

<p align="center">
    <a href="https://discord.gg/9vpqbjU"><img src="https://img.shields.io/discord/712952679415939085?label=discord&logo=discord" alt="discord"></a>
    <a href="https://twitter.com/mackron"><img src="https://img.shields.io/twitter/follow/mackron?style=flat&label=twitter&color=1da1f2&logo=twitter" alt="twitter"></a>
</p>

This is still work-in-progress. There's a lot of missing functionality and inconsistencies between
backends. Typography is also largely untested for scripts other than Latin.

If you think this library seems interesting, I'd love to hear some feedback on what you think could
be improved and what new features you might find useful.


Features
========
- Path-based 2D drawing.
- Unicode based text itemization, shaping and placement.


Supported Platforms
===================
- GDI + Uniscribe (Windows XP+)
- Cairo + Pango

Linux platforms are currently restricted the Cairo/Pango backend, but this will be improved later as
the API stabilizes and other backend's come online.


Future Plans
============
- Software backend
- Direct2D + DirectWrite backend
- Core Graphics + Core Text backend (Apple)
- PDF output
- SVG output
- Support for complex scripts such as Arabic


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


Examples
========
See the examples and tests folder for examples on how to use the API. It is based largely on other
path based APIs such as Cairo and GDI.