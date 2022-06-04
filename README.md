## 项目概述

本项目是我通过学习[Wiki](https://github.com/ssloy/tinyrenderer/wiki)制作的一个渲染器。该项目可以对obj格式的文件进行渲染，使用了TGAimage文件输出框架。由我自己编写了渲染管道的代码，并不断完善以及添加新功能。<br />英文文档请点击此[链接](https://github.com/a446187673/MyTinyRenderer/blob/master/READMENG.md)。

## 描述

本文档回顾了对于本项目的所有重要提交

## 提交0:obj资源以及渲染器雏形

| 新文件            | 描述                                                                                        |
| -------------- | ----------------------------------------------------------------------------------------- |
| main.cpp       | 渲染管线主要部分                                                                                  |
| tgaimage.h/cpp | TGAimage文件输出框架                                                                            |
| model.h/cpp    | obj格式的文件加载                                                                                |
| geometry.h/cpp | 定义结构Vec2f(float) / Vec2i(int) / Vec3f / Vec3i(向量)以及Matrix(矩阵),并提供了向量的叉乘、点乘、归一化以及矩阵的初始化等方法 |
| obj文件夹         | 储存obj格式文件以及uv贴图                                                                           |
| .exeFile文件夹    | 储存可执行文件                                                                                   |
| .tgaFile文件     | 储存输出的tga格式文件                                                                              |
| .vscode        | vscode调试文件                                                                                |

通过zbuffer进行背面剔除并通过线性插值绘制uv纹理后得到的渲染结果  
<img src="https://github.com/a446187673/MyTinyRenderer/blob/master/picture/output.png?raw=true"></img>  
## 提交1:代码重构

重构了geometry.h，为部分代码增加了注释

| 更新文件       | 描述                                                                                                                                             |
| ---------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| geometry.h |  -`inline Vec3<T> operator` , `inline Vec22<T> operator`内联了重载后的运算符<br />- `T& operator[](const int i)`:增加通过下标访问向量的重载,并增加下标越界时的报错功能<br /> |
