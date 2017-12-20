# Polygon triangulation visual

A C++ visual polygon triangulation algorithm implementation.
Team project for the Computational Geometry course.

![](sample.png?raw=true "GC-proj")

## Getting Started

### Prerequisites

You will need to download or build your own version of the [SFML](http://www.sfml-dev.org/download.php)
*(Simple and Fast Multimedia Library)*.

### Installing

Simply place the SFML build under *SFML/* in the same directory as *src/* and run the build.

- Mac:
```
clang++ -std=c++11 src/main.cpp src/Renderer.cpp src/Logic.cpp -o application -DSFML_STATIC -I SFML/include -L SFML/lib -lsfml-graphics -lsfml-window -lsfml-system
```
- Linux:
```
g++ ...
```

## Usage

- Left click to add a new point to the polygon.
- Right click to move the sampling point. The sampling point highlights the triangle that it is part of whenever inside the polygon.
- Press "C" to clear the canvas.