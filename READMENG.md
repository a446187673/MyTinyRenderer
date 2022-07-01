## Project Overview

This project is through my study [Wiki ](https://github.com/ssloy/tinyrenderer/wiki)create a renderer. This project can render files in obj format and use TGAimage file output framework. I wrote the code of rendering pipeline by myself, and constantly improved and added new functions.

## Description

This document reviews all important submissions for this project.
## Commit 5:TBN matrix and Phong Shading 
This submission is mainly to add Toon Shading,Flat Shading, and Phong Shading coloring methods. In the Phong Shading, the normals in the tangent space stored in the normal map are converted into normals represented by View coordinates through TBN matrix, so as to calculate the lighting intensity of the slice elements and then color them to make the rendering results look concave-convex texture and different reflective texture. <br />In addition, some problems in the previous submitted code were fixed, mainly the error of overloading the multiplication operator between the matrix and the vector. 

| File Update | Description  |
| --- | --- |
| shader.h/cpp  | - `class ToonShader : public Shader `Toon shader <br />- `class FlatShader : public Shader `Flat shader <br />- `class PhongShader : public Shader `Phong shader <br /> |
| model.h/cpp  | - `Vec3f norm(Vec2f uvf) `returns the normal value of the tangent space. <br /> |
| geometry.h/cpp  | - `Matrix operator*(const float& num);`<br />- `Vec4f operator*(Vec4f& vec);`<br />- `Vec3f operator*(Vec3f& vec);`<br />- The multiplication calculation between matrix and vector and floating-point number is updated and corrected.<br /> |

In this submission, the calculation method of TBN matrix is referred [tangent space of OpenGL core technology](https://haiyang.blog.csdn.net/article/details/58671953?spm=1001.2014.3001.5506).<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/TBN1.png)<br />The relationship between tangent, secondary tangent and triangle edge can be seen from this figure, and then expressed by matrix.<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/TBN2.png)<br />Then deform and find the inverse.<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/TBN3.png)<br />The final Phong Shading results are as follows:<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/Phong%20Shading.png)<br />In addition, the results obtained by Flat Shading and Toon Shading are displayed respectively.<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/Flat%20Shading.png)![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/Toon%20Shading.png)
## Commit 4: pivoting correction and Gouraud Shading 
Some codes are reconstructed and shader.h/cpp is added to simulate vertex shaders and slice shaders. The perspective interpolation is corrected in the rasterization process, and GouraudShader is added for Gouraud Shading.

| New File  | Description  |
| --- | --- |
| shader.h/cpp | `virtual Vec4f vertex(int iface,int jvertex) `vertex shader <br />- `virtual bool fragment(Vec3f bc_screen, TGAColor &amp;color) `element shader <br />- the four matrices used for View transformation are now stored in the shader<br /> |

| File Update | Description  |
| --- | --- |
| geometry.h/cpp |- `Template <typenameT>struct Vec4 `added a four-dimensional vector and overloaded its operator multiplied by the matrix. <br /> |
| tgaimage.h |- Store the zbuffer value as an image <br /> |
| rasterizer.h/cpp |- in the process of rasterization, the slice shader is used for coloring. <br />- In the future, we will use the bounding box method for rasterization.<br /> |

As for the perspective correction, the previous program did not consider the distance of the object on the world coordinates when calculating the centroid coordinates, so now the perspective interpolation is corrected. <br />Before correction: <br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/0.png)<br />after correction: <br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/1.png)<br />the following figure is used results obtained by rendering Gouraud Shading <br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/Gouraud%20Shading.png)
## Commit 3: camera transformation 
The model transformation and camera transformation matrix are added to simulate the whole process of visual Port transformation in rendering pipelines, and some methods are optimized.

| File Update | Description  |
| --- | --- |
| rasterizer.h/cpp | - `Matrix get_model_matrix(char c,float rotation);`modeling transformation<br />- `Matrix get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up);`camera transformation<br />- `Matrix get_projection_matrix(Vec3f camera, Vec3f center);`projection transformation<br />- `Matrix get_viewport_matrix(int width, int height, int depth);`viewport transformation<br /> |
| geometry.h/cpp | - `template <> Vec3<float>::Vec3(Matrix m)`<br />- `Matrix::Matrix(Vec3f v) : m(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.0f))), rows(4), cols(1)`method of adding vector matrix interchange in Constructor<br /> |

Set the camera position to (2,1,3), focus to (0,0,1), and rotate 20 degrees around the Y axis.<br />![avatar](https://raw.githubusercontent.com/a446187673/MyTinyRenderer/master/picture/Y20.png)

## Commit 2: rasterizer

Encapsulate the rendering method into rasterizer.h/cpp, and add the initialization process of zbuffer to the constructor of the TGAimage class. In addition, a method for mutual conversion between vectors and matrices is added for geometry.h/cpp. 

| New File  | Description  |
| --- | --- |
| rasterizer.h/cpp  | - `void line(int x0, int y0, int x1, int y1, TGAImage &amp;image, TGAColor color); `draw a straight line from point (x0,y0) to (x1,y1) <br />- `Vec3f barycentric(Vec3f A, Vec3f B ,Vec3f C, Vec3f P); `calculate the centroid coordinates of point P for triangle ABC <br />- `void triangle_line(Vec3f* pts, Vec2i* uv, Model* model, TGAImage&amp; image, float intensity, float* zbuffer); `draw a triangle by drawing lines <br />- `void triangle_box(Vec3f* pts, Vec2i* uv, Model* model, TGAImage&amp; image, float intensity, float* zbuffer); `draw a triangle by using a bounding box<br /> |

| File Update | Description  |
| --- | --- |
| tgaimage.h/cpp  | Initialization of zbuffer  |
| geometry.h/cpp  | - `static Vec3f m2v(Matrix m); `vector turns to matrix <br />- `static Matrix v2m(Vec3f v); `matrix turns to vector <br />- `static Matrix viewport(int x, int y, int w, int h, int depth); `visual angle matrix<br /> |

Output zbuffer picture<br />![avatar](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/zbuffer.png)

## Commit 1: code refactoring

Reconstructed geometry.h, Added comments for some code.

| File Update | Description                                                                                                                                                                                                                                                                               |
| ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| geometry.h  | - `inline Vec3 <T>operator `, `inline Vec2 <T>operator `the overloaded operators are inline. <br />- `T&amp; operator[](const int I) `: adds the overloading of access vectors through subscripts, and adds the error reporting function when subscripts cross the boundary.<br /> |
## Commit 0:obj resources and the rudiment of the renderer
| New File | Description |
| --- | --- |
| main.cpp | Main parts of rendering pipeline |
| tgaimage.h/cpp | TGAimage file output framework |
| model.h/cpp | obj file loading |
| geometry.h/cpp | Define the structure Vec2f(float) / Vec2i(int) / Vec3f / Vec3i (vector) and Matrix (Matrix), and provide methods such as cross multiplication, point multiplication, normalization and Matrix initialization of vectors. |
| obj folder | Save obj files and uv maps |
| . exeFile folder | Save executable files |
| . tgaFile folder | Save output TGA format file |
| .vscode folder | Vscode debug file |


The rendering result obtained by removing the back through zbuffer and rendering the uv texture through linear interpolation.

![](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/output.png?raw=true#crop=0&crop=0&crop=1&crop=1&id=GUEiF&originHeight=792&originWidth=798&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=)
