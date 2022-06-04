## Project Overview

This project is through my study [Wiki ](https://github.com/ssloy/tinyrenderer/wiki)create a renderer. This project can render files in obj format and use TGAimage file output framework. I wrote the code of rendering pipeline by myself, and constantly improved and added new functions.

## Description

This document reviews all important submissions for this project.

## commit 0:obj resources and the rudiment of the renderer

| New File         | Description                                                                                                                                                                                                              |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| main.cpp         | Main parts of rendering pipeline                                                                                                                                                                                         |
| tgaimage.h/cpp   | TGAimage file output framework                                                                                                                                                                                           |
| model.h/cpp      | obj file loading                                                                                                                                                                                                         |
| geometry.h/cpp   | Define the structure Vec2f(float) / Vec2i(int) / Vec3f / Vec3i (vector) and Matrix (Matrix), and provide methods such as cross multiplication, point multiplication, normalization and Matrix initialization of vectors. |
| obj folder       | Save obj files and uv maps                                                                                                                                                                                               |
| . exeFile folder | Save executable files                                                                                                                                                                                                    |
| . tgaFile folder | Save output TGA format file                                                                                                                                                                                              |
| .vscode folder   | Vscode debug file                                                                                                                                                                                                        |

The rendering result obtained by removing the back through zbuffer and rendering the uv texture through linear interpolation.<br />
<img src="https://github.com/a446187673/MyTinyRenderer/blob/master/picture/output.png?raw=true"></img>

## Commit 1: code refactoring

Reconstructed geometry.h, Added comments for some code.

| File Update | Description                                                                                                                                                                                                                                                                               |
| ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| geometry.h  | - `inline Vec3 <T>operator `, `inline Vec22 <T>operator `the overloaded operators are inline. <br />- `T&amp; operator[](const int I) `: adds the overloading of access vectors through subscripts, and adds the error reporting function when subscripts cross the boundary.<br /> |
