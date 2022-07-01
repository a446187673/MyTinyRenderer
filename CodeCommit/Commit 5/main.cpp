#include <vector>
#include <iostream>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "shader.h"
#include "rasterizer.h"

//定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
//定义模型
Model *model = NULL;
//定义宽度，高度,深度
const int width = 800;
const int height = 800;
const int depth = 255;
//设置光照方向
Vec3f light_dir = Vec3f(0,1,1);
//设置摄像机
Vec3f camera(1,1,2);
//设置焦点
Vec3f center(0,0,0);

int main(int argc, char **argv)
{
        if (2==argc) 
                model = new Model(argv[1]);  //命令行控制方式构造model
        else
                model = new Model("obj/african_head.obj"); //代码方式构造model

        //构造TGA图像
        TGAImage image(width,height, TGAImage::RGB);
        //构造zbuffer
        TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
        //实例化着色器和光栅化器
        //GouraudShader shader;
        //ToonShader shader;
        //FlatShader shader;
        PhongShader shader;
        Rasterizer ras;

        //模型变换矩阵
        shader.model_matrix = shader.get_model_matrix('X',0);
        //摄像机变换矩阵
        shader.camera_matrix = shader.get_camera_matrix(camera, center, Vec3f(0,1,0));
        //投影变换矩阵
        shader.projection_matrix = shader.get_projection_matrix(camera, center);
        //视角变换矩阵
        shader.viewport_matrix = shader.get_viewport_matrix(width,height,depth);
        //光照方向
        shader.light_dir = light_dir.normalize();

        //遍历所有三角形面
        for (int i = 0; i < model->nfaces(); i++)
        {
                //屏幕坐标
                Vec4f screen_coords[3];
                for (int j = 0; j < 3; j++)
                {
                        screen_coords[j] = shader.vertex(i,j);
                }

                ras.triangle_box(screen_coords, shader, image, zbuffer);
        }
        image.flip_vertically(); //翻转
        image.write_tga_file(".tgaFile/output.tga");
        zbuffer.flip_vertically();
        zbuffer.write_tga_file(".tgaFile/zbuffer.tga");//输出zbuffer

        //system("Pause");
        delete model;
        return 0;
}
