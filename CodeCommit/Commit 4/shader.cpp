#include "shader.h"
const float PI = 3.1415926;
//模型变换
Matrix Shader::get_model_matrix(char c,float rotation)
{
    Matrix mat = Matrix::identity(4);
    float angle = rotation * PI / 180.0f;
    if(c == 'X')
    {
        mat << std::vector<float>{
		1,0,                 0,               0,
		0, std::cos(angle), -std::sin(angle), 0,
		0, std::sin(angle),  std::cos(angle), 0,
		0, 0,                0,               1
		};
    }
    else if(c == 'Y')
    {
        mat << std::vector<float>{
		std::cos(angle),  0, std::sin(angle), 0,
		0,                1, 0,               0,
		-std::sin(angle), 0, std::cos(angle), 0,
		0,                0, 0,               1
		};
    }
    else if(c == 'Z')
    {
		mat << std::vector<float>{
		std::cos(angle), -std::sin(angle), 0, 0,
		std::sin(angle),  std::cos(angle), 0, 0,
		0,                0,               1, 0,
		0,                0,               0, 1
		};
    }
    return mat;
}
//摄像机变换
Matrix Shader::get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up)
{
        //先计算z，根据z和up算出x，再算出y
        Vec3f z = (camera - center).normalize();
        Vec3f x = (up ^ z).normalize();
        Vec3f y = (z ^ x).normalize();
        Matrix translation = Matrix::identity(4);
        Matrix rotation = Matrix::identity(4);
        //先平移
        for (int i = 0; i < 3; i++)
                translation[i][3] = -center[i];
        //再旋转
        for (int i = 0; i < 3; i++)
        {
                rotation[0][i] = x[i];
                rotation[1][i] = y[i];
                rotation[2][i] = z[i];
        }
        return rotation*translation;
}
//投影变换
Matrix Shader::get_projection_matrix(Vec3f camera, Vec3f center)
{
    Matrix mat = Matrix::identity(4);
    mat[3][2] = -1.0f/(camera - center).norm();
    return mat;
}
//视角变换
Matrix Shader::get_viewport_matrix(int width, int height, int depth)
{
    //创建一个4x4单位矩阵mat
    Matrix mat = Matrix::identity(4);
    //第四列表示平移信息
    //对角线表示缩放信息
    mat << std::vector<float>{
	3.0f*width/8.0f,0,                0,          width/2.0f,
	0,              3.0f*height/8.0f, 0,          height/2.0f,
	0,              0,                depth/2.0f, depth/2.0f,
	0,              0,                0,          1
	};
    return mat;
}
//顶点着色器，接收两个变量（面序号，顶点序号）
 Vec4f GouraudShader::vertex(int iface,int jvertex)
 {
    //世界坐标
    Vec3f world_coords = model->vert(iface,jvertex);
    //转四维向量
    Vec4f gl_Vertex(world_coords.x,world_coords.y,world_coords.z,1);
    //uv坐标数组
    uv[jvertex] = model->uv(iface,jvertex);
    //屏幕坐标 = 视角变换矩阵*投影变换矩阵*摄像机变换矩阵*模型变换矩阵 即视口变换
    gl_Vertex =viewport_matrix * projection_matrix * camera_matrix * model_matrix * gl_Vertex;

    //Gouraud Shading所需的当前面各点的光照强度
    //Vec3f normal = (model->norm(iface,jvertex)).normalize(); //当前点的法向量
    intensity[jvertex] = std::max(0.0f, model->norm(iface,jvertex)*light_dir);
    return gl_Vertex;
 }
//片元着色器，接收两个变量（质心坐标，像素点颜色）
 bool GouraudShader::fragment(Vec3f bc_screen, TGAColor &color)
 {
    //计算P点的颜色
    Vec2f uvP = uv[0] * bc_screen.x + uv[1] * bc_screen.y + uv[2] * bc_screen.z;
    TGAColor P_color = model->diffuse(uvP);
    //计算P点的光照强度
    float P_intensity = intensity * bc_screen;
    //着色
    color = P_color*P_intensity;
    return false;

 }