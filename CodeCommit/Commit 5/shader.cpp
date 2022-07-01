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
        //再旋转 该旋转矩阵的逆矩阵就是它本身
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
//Gouraud Shading---------------------------------------------------------------------------------------------------
//顶点着色器，接收两个变量（面序号，顶点序号）
 Vec4f GouraudShader::vertex(int iface,int jvertex)
 {
    //世界坐标
    world_coords[jvertex] = model->vert(iface,jvertex);
    //转四维向量
    Vec4f gl_Vertex(world_coords[jvertex].x,world_coords[jvertex].y,world_coords[jvertex].z,1);
    //uv坐标数组
    uv[jvertex] = model->uv(iface,jvertex);
    //屏幕坐标 = 视角变换矩阵*投影变换矩阵*摄像机变换矩阵*模型变换矩阵 即视口变换
    gl_Vertex =viewport_matrix * projection_matrix * camera_matrix * model_matrix * gl_Vertex;

    //Gouraud Shading所需的当前面各点的光照强度
    intensity[jvertex] = std::max(0.0f, model->norm(iface,jvertex)*light_dir);
    return gl_Vertex;
 }
//片元着色器，接收两个变量（质心坐标，像素点颜色）
 bool GouraudShader::fragment(Vec3f bc_screen, TGAColor &color)
 {
    //计算P点的颜色
    Vec2f uvP = uv[0] * bc_screen.x + uv[1] * bc_screen.y + uv[2] * bc_screen.z;
    TGAColor P_color = model->diffuse(uvP);
    //计算P点的光照强度(三个顶点的光照强度 * 对应位置的质心坐标)
    float P_intensity = intensity * bc_screen;
    //着色
    color = P_color*P_intensity;
    return false;

 }
 //Toon Shading------------------------------------------------------------------------------------------------------
 Vec4f ToonShader::vertex(int iface,int jvertex)
 {
    //世界坐标
    world_coords[jvertex] = model->vert(iface,jvertex);
    //转四维向量
    Vec4f gl_Vertex(world_coords[jvertex].x,world_coords[jvertex].y,world_coords[jvertex].z,1);
    //屏幕坐标 = 视角变换矩阵*投影变换矩阵*摄像机变换矩阵*模型变换矩阵 即视口变换
    gl_Vertex =viewport_matrix * projection_matrix * camera_matrix * model_matrix * gl_Vertex;
    //计算各顶点的光照强度
    intensity[jvertex] = model->norm(iface,jvertex)*light_dir;
    return gl_Vertex;
 }
bool ToonShader::fragment(Vec3f bc_screen, TGAColor &color)
 {
    float P_intensity = intensity * bc_screen;
    //将一定阈值内的光照强度替换为一种
    if      (P_intensity > 0.85) P_intensity =    1;
    else if (P_intensity > 0.60) P_intensity = 0.80;
    else if (P_intensity > 0.45) P_intensity = 0.60;
    else if (P_intensity > 0.30) P_intensity = 0.45;
    else if (P_intensity > 0.15) P_intensity = 0.30;
    //底色为粉色
    color = TGAColor(242,156,177) * P_intensity;
    return false;
 }
  //Flat Shading------------------------------------------------------------------------------------------------------
Vec4f FlatShader::vertex(int iface,int jvertex)
{
    //世界坐标
    world_coords[jvertex] = model->vert(iface,jvertex);
    //转四维向量
    Vec4f gl_Vertex(world_coords[jvertex].x,world_coords[jvertex].y,world_coords[jvertex].z,1);
    //投影变换矩阵*摄像机变换矩阵*模型变换矩阵
    gl_Vertex = projection_matrix * camera_matrix * model_matrix * gl_Vertex;
    //变换后的顶点坐标
    varying_tri[jvertex] = Vec3f(gl_Vertex.x/gl_Vertex.w, gl_Vertex.y/gl_Vertex.w, gl_Vertex.z/gl_Vertex.w);
    //做最后的视角变换
    gl_Vertex = viewport_matrix * gl_Vertex;
    return gl_Vertex;
}
bool FlatShader::fragment(Vec3f bc_screen, TGAColor &color)
{
    Vec3f n = ( (varying_tri[1] - varying_tri[0])^(varying_tri[2] - varying_tri[0]) ).normalize();
    //计算P点光照强度
    float P_intensity = n * light_dir;
    //统一着色为白色
    color = TGAColor(255,255,255) * P_intensity;
    return false;
}
//Phong Shading-----------------------------------------------------------------------------------------------------
Vec4f PhongShader::vertex(int iface,int jvertex)
{
    //世界坐标
    world_coords[jvertex] = model->vert(iface,jvertex);
    //转四维向量
    Vec4f gl_Vertex(world_coords[jvertex].x,world_coords[jvertex].y,world_coords[jvertex].z,1);
    //uv坐标数组
    uv[jvertex] = model->uv(iface,jvertex);
    //顶点法向量
    normal[jvertex] = model->norm(iface,jvertex);
    //屏幕坐标 = 视角变换矩阵*投影变换矩阵*摄像机变换矩阵*模型变换矩阵 即视口变换
    gl_Vertex = viewport_matrix * projection_matrix * camera_matrix * model_matrix * gl_Vertex;
    return gl_Vertex;

}
bool PhongShader::fragment(Vec3f bc_screen, TGAColor &color)
{
    //计算P点的颜色
    Vec2f uvP = uv[0] * bc_screen.x + uv[1] * bc_screen.y + uv[2] * bc_screen.z;
    TGAColor P_color = model->diffuse(uvP);
    //计算P点光照强度
    Matrix TBN = get_TBN(bc_screen);
    Vec3f n = model->norm(uvP);
    n.normalize();
    n = TBN * n;
    float P_intensity = std::max(0.0f,n.normalize() * light_dir);
    //着色
    color = P_color*P_intensity;
    return false;
}
Matrix PhongShader::get_TBN(Vec3f bc_screen)
{
    float u1 = uv[0].x;
    float v1 = uv[0].y;
    float u2 = uv[1].x;
    float v2 = uv[1].y;
    float u3 = uv[2].x;
    float v3 = uv[2].y;
    float deltaU1 = u2 - u1;
    float deltaU2 = u3 - u1;
    float deltaV1 = v2 - v1;
    float deltaV2 = v3 - v1;
    Matrix E(2,3);
    E<<std::vector<float>{
        world_coords[1].x - world_coords[0].x, world_coords[1].y - world_coords[0].y, world_coords[1].z - world_coords[0].z,
        world_coords[2].x - world_coords[0].x, world_coords[2].y - world_coords[0].y, world_coords[2].z - world_coords[0].z
    };
    // 1/矩阵的行列式 * deltaUV组成的矩阵的逆矩阵
    Matrix UV_inverse(2,2);
    UV_inverse<<std::vector<float>{
         deltaV2, -deltaV1,
        -deltaU2,  deltaU1
    };
    UV_inverse = UV_inverse * (1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1) );
    Matrix TB(2,3);
    TB = UV_inverse * E;
    //计算切线Tangent 副切线Bitangent
    Vec3f T(TB[0][0], TB[0][1], TB[0][2]);
    Vec3f B(TB[1][0], TB[1][1], TB[1][2]);
    T.normalize();
    B.normalize();
    //计算法线Normal
    Vec3f N = normal[0] * bc_screen.x + normal[1] * bc_screen.y  +  normal[2] * bc_screen.z;
    N.normalize();
    Matrix TBN(3,3);
        TBN << std::vector<float>{
        T[0], B[0], N[0],
        T[1], B[1], N[1],
        T[2], B[2], N[2]
    };
    return TBN;
}