## 项目概述

本项目是我通过学习[Wiki](https://github.com/ssloy/tinyrenderer/wiki)制作的一个渲染器。该项目可以对obj格式的文件进行渲染，使用了TGAimage文件输出框架。由我自己编写了渲染管线的代码，并不断完善以及添加新功能。<br />Please click this [link](https://github.com/a446187673/MyTinyRenderer/blob/master/READMENG.md) for English documents.

## 描述

本文档回顾了对于本项目的所有重要提交
## 提交3:摄像机变换
增加了模型变换与摄像机变换矩阵，以模拟渲染管线中视口变换的全过程，并优化了一些方法

| 更新文件 | 描述 |
| --- | --- |
| rasterizer.h/cpp | <br />- `Matrix get_model_matrix(char c,float rotation);`模型变换矩阵<br />- `Matrix get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up);`摄像机变换矩阵<br />- `Matrix get_projection_matrix(Vec3f camera, Vec3f center);`投影变换矩阵<br />- `Matrix get_viewport_matrix(int width, int height, int depth);`视角变换矩阵<br /> |
| geometry.h/cpp | <br />- `template <> Vec3<float>::Vec3(Matrix m)`<br />- `Matrix::Matrix(Vec3f v) : m(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.0f))), rows(4), cols(1)`在构造函数中添加向量矩阵互换的方法<br /> |

设置摄像机位置为(2,1,3),焦点为(0,0,1)，并绕Y轴旋转20度所得的渲染结果<br />![avatar](http://baidu.com/pic/doge.png)

## 提交2:光栅化器

将渲染方法封装进rasterizer.h/cpp，并将zbuffer的初始化过程加入TGAimage类的构造函数中。另外为geometry.h/cpp添加了用于向量和矩阵间相互转换的方法。

| 新文件 | 描述 |
| --- | --- |
| rasterizer.h/cpp | <br />- `void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);`画一条从点(x0,y0)到(x1,y1)的直线<br />- `Vec3f barycentric(Vec3f A, Vec3f B,Vec3f C, Vec3f P);`计算P点对于三角形ABC的质心坐标<br />- `void triangle_line(Vec3f* pts, Vec2i* uv, Model* model, TGAImage& image, float intensity, float* zbuffer);`用画线填充的方法绘制三角形<br />- `void triangle_box(Vec3f* pts, Vec2i* uv, Model* model, TGAImage& image, float intensity, float* zbuffer);`用包围盒的方法绘制三角形<br /> |

| 更新文件 | 描述 |
| --- | --- |
| tgaimage.h/cpp | zbuffer的初始化 |
| geometry.h/cpp | <br />- `static Vec3f m2v(Matrix m);`向量->矩阵<br />- `static Matrix v2m(Vec3f v);`矩阵->向量<br />- `static Matrix viewport(int x, int y, int w, int h, int depth);`视角矩阵<br /> |

## 提交1:代码重构

重构了geometry.h，为部分代码增加了注释

| 更新文件       | 描述                                                                                                                                             |
| ---------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| geometry.h |  - `inline Vec3<T> operator` , `inline Vec2<T> operator`内联了重载后的运算符<br />- `T& operator[](const int i)`:增加通过下标访问向量的重载,并增加下标越界时的报错功能<br /> |
## 提交0:obj资源以及渲染器雏形
| 新文件 | 描述 |
| --- | --- |
| main.cpp | 渲染管线主要部分 |
| tgaimage.h/cpp | TGAimage文件输出框架 |
| model.h/cpp | obj格式的文件加载 |
| geometry.h/cpp | 定义结构Vec2f(float) / Vec2i(int) / Vec3f / Vec3i(向量)以及Matrix(矩阵),并提供了向量的叉乘、点乘、归一化以及矩阵的初始化等方法 |
| obj文件夹 | 储存obj格式文件以及uv贴图 |
| .exeFile文件夹 | 储存可执行文件 |
| .tgaFile文件 | 储存输出的tga格式文件 |
| .vscode | vscode调试文件 |

通过zbuffer进行背面剔除并通过线性插值绘制uv纹理后得到的渲染结果<br />![](https://github.com/a446187673/MyTinyRenderer/blob/master/picture/output.png?raw=true#crop=0&crop=0&crop=1&crop=1&id=ci1mb&originHeight=792&originWidth=798&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=)
