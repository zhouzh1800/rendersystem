#ifndef	MYDX_H
#define MYDX_H

#include	<math.h>
#include	<stdio.h>
#include	<assert.h>
#include	<iostream.h>

#include	"stdafx.h"


#define	PI	 3.141526f

#define TBuffer_INC 1


#define TEMPLATE_WRITE	0
#define TEMPLATE_EQUA	1

class Vertex;
class Matrix;
class Triangle;
class Camera;
class VecVertex;
class VecTriangle;
class Light;
class ZBuffer;
class TBuffer;
class BackSystem;
class LightColor;

void SetBackSystem(const BackSystem & backSystem);

BackSystem & GetBackSystem();

void mextX0(long  * x, long * y, long * d, long incd0, long incd1, long dx);

void nextX1(long  * x, long * y, long * d, long incd0, long incd1, long dx);




/*
*-----------------------
*	class Vertex
*-----------------------
*/

typedef Vertex	Vector;

class Vertex
{
private:
	double	m_x;
	double	m_y;
	double	m_z;
	double	m_w;
public:
	Vertex();
	Vertex(double x, double y, double z, double w);
	Vertex(const Vertex & pVertex);

	void SetX(double x);
	void SetY(double y);
	void SetZ(double z);
	void SetW(double w);
	
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	double GetW() const;
	
	Vertex Project() const;
	Vertex Uniform() const;
	Vertex Identity() const;
	double Dot(const Vector & vector) const;
	Vector Cross(const Vector & vector) const;
	double Length() const;
	void Print() const;
	
	const Vertex operator - () const;
	const Vertex operator + (const Vertex & vertex) const;
	const Vertex operator - (const Vertex & vertex) const;
	const Vertex operator * (double doubleNumber) const;
	const Vertex operator / (double doubleNumber) const;
	~Vertex();
};



/*
*-----------------------
*	class Matrix
*-----------------------
*/

class Matrix
{
private:
	double 	m_f00, m_f01, m_f02, m_f03;
	double 	m_f10, m_f11, m_f12, m_f13;
	double 	m_f20, m_f21, m_f22, m_f23;
	double 	m_f30, m_f31, m_f32, m_f33;

public:
	Matrix();

	Matrix( double f00, double f01, double f02,double f03,
			double f10, double f11, double f12,double f13,
			double f20, double f21, double f22,double f23,
			double f30, double f31, double f32,double f33);
	
	Matrix(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2, const Vertex & vertex3);
	Matrix(const Matrix & pSrcMatrix); 

	void SetAll(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2, const Vertex & vertex3);
	void Translation(double tx, double ty, double tz);
	void RotationX(double angle);
	void RotationY(double angle);
	void RotationZ(double angle);
	Matrix Transpose()  const;
//	void Identity() const;
	void Print(char * buf) const;

	const Matrix operator - () const;
	const Matrix operator + (const Matrix & matrix) const;
	const Matrix operator * (const Matrix & matrix) const;
	const Vertex operator * (const Vertex & vertex) const;
	const VecVertex operator * (const VecVertex & vecVertex) const;
	const Triangle operator * (const Triangle & triangle) const;
	const VecTriangle operator * (const VecTriangle & vecTriangle) const;
	const Camera operator * (const Camera & camera) const;
	
	double & operator () (int i, int j);
	const double & operator () (int i, int j) const;
	~Matrix();
};










/*
*-----------------------
*	class Triangle
*-----------------------
*/

class Triangle
{
private:
	Vertex	m_vertex0;
	Vertex	m_vertex1;
	Vertex	m_vertex2;
	
public:
	Triangle();
	Triangle(const Triangle & triangle);
	Triangle(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2);
	void SetVertex(int index, const Vertex & vertex);
	Vertex & GetVertex(int index);
	const Vertex & GetVertex(int index) const;
	Triangle Project() const;
	void Render() const;
	~Triangle();
};


/*
*-----------------------
*	class Camera
*-----------------------
*/
class Camera
{
private:
	Vertex	m_position;
	Vertex	m_target;
	Vertex	m_up;
public:
	Camera();
	Camera(const Camera & camera);
	Camera(const Vertex & position, const Vector & direction, const Vector & Vector);
	
	
	void SetPosition(const Vertex & position);
	void SetTarget(const Vertex & target);
	void SetUp(const Vertex & up);
	
	
	Vertex & GetPosition();
	Vertex & GetTarget();
	Vertex & GetUp();

	Vector GetDirectionVector() const;
	Vector GetRightVector() const;
	Vector GetUpVector() const;

	const Vertex & GetPosition() const;
	const Vertex & GetTarget() const;
	const Vertex & GetUp() const;

	

	void Identity();
	Matrix GetVectorCoordMatrix() const;
	Matrix GetCameraMatrix() const;
};




/*
*-----------------------
*	class VecVertex
*-----------------------
*/
class VecVertex
{
private:
	Vertex	m_position;
	Vector	m_vector;

	double	m_tx;
	double	m_ty;

public:
	VecVertex();
	VecVertex(const VecVertex & vecVertex);
	VecVertex(const Vertex & position, const  Vector & vector, double tx, double ty);

	void SetPosition(Vertex & position);
	void SetVector(Vector & vector);
	void SetTx(double tx);
	void SetTy(double ty);
	void SetTxy(double tx, double ty);
	void SetAll(const Vertex & position, const  Vector & vector, double tx, double ty);
	void SetAll(const VecVertex & vecVertex);

	Vertex & GetPosition();
	Vertex & GetVector();
    const Vertex & GetPosition() const;
	const Vertex & GetVector() const;
	double GetTx() const;
	double GetTy() const;
	
	VecVertex Project() const;

	VecVertex operator * (double number) const;
	VecVertex operator + (const VecVertex & vecVertex) const;

	~VecVertex();
};




class VecTriangle
{
private:
	VecVertex	m_vecVertex0;
	VecVertex	m_vecVertex1;
	VecVertex	m_vecVertex2;
public:
	VecTriangle();
	VecTriangle(const VecTriangle & vecTriangle);
	VecTriangle(const VecVertex & vecVertex0, const VecVertex & vecVertex1, const VecVertex & vecVertex2);
	void SetVecVertex(int index, const VecVertex & vecVertex);
	VecVertex & GetVecVertex(int index);
	const VecVertex & GetVecVertex(int index) const;

	Vector GetVector() const;
	Vertex GetMiddleVertex() const;
	int GetMaxYIndex() const;
	void ExchangeVecVertex(int index1, int index2);

	VecTriangle VectorUniform();
	void Sort();
	void SetSort(int sort[]);

	VecTriangle Project() const;
	void Render() const;

	friend class BackSystem;
	~VecTriangle();
};

class LightColor
{
private:
	unsigned int	m_red;
	unsigned int	m_green;
	unsigned int	m_blue;
public:
	LightColor();
	LightColor(const LightColor & lightColor);
	LightColor(unsigned int red, unsigned int green, unsigned int blue);
	LightColor(unsigned int color);
	void SetRed(unsigned int red);
	void SetGreen(unsigned int green);
	void SetBlue(unsigned int blue);
	void SetAll(unsigned int red, unsigned int green, unsigned int blue);

	unsigned int GetRed() const;
	unsigned int GetGreen() const;
	unsigned int GetBlue() const;
	
	LightColor Mul(const LightColor & lightColor);

	LightColor Uniform();
	unsigned int ToUInt();
	LightColor operator + (const LightColor & lightColor);
};

class Material
{
private:
	LightColor		m_lightColor;
public:
	Material();
	Material(int red, int green, int blue);
	LightColor & GetLightColor();
	const LightColor & GetLightColor() const;
};



class EnvironmentLight
{
private:
	LightColor	m_lightColor;
public:
	EnvironmentLight();
	EnvironmentLight(const EnvironmentLight & environmentLight);
	EnvironmentLight(const LightColor & lightColor);

	void SetLightColor(const LightColor & lightColor);
	LightColor & GetLightColor();
	const LightColor & GetLightColor() const;
	LightColor VecVertexColor();
};

class DirectionLight 
{
private:
	LightColor	m_lightColor;
	Vector		m_direction;
public:
	DirectionLight();
	DirectionLight(const DirectionLight & directionLight);
	DirectionLight(const LightColor & lightColor, const Vector & direction);

	void SetDirection(const Vector & direction);
	void SetLightColor(const LightColor & lightColor);
	LightColor & GetLightColor();
	Vector & GetDirection();	
	const LightColor & GetLightColor() const;
	const Vector & GetDirection() const;
	
	LightColor VecVertexColor(const Vector & vector);
};

class DotLight 
{
private:
	LightColor	m_lightColor;
	Vertex		m_position;
	double		m_c1;
	double		m_c2;
	double		m_c3;
public:
	DotLight();
	DotLight(const DotLight & dotLight);
	DotLight(const LightColor & lightColor, const Vertex & poistion, double c1, double c2, double c3);

	void SetLightColor(const LightColor & lightColor);
	void SetPosition(const Vertex & poistion);
	LightColor & GetLightColor();
	Vertex & GetPosition();
	const LightColor & GetLightColor() const;
	const Vertex & GetPosition() const;

	LightColor VecVertexColor(const Vertex & position, const Vector & vector);
};

class Light
{
private:
	EnvironmentLight	m_environmentLight;
	DirectionLight		m_directionLight;
	DotLight			m_dotLight;
public:
	Light();
	Light(const Light & light);
	Light(const EnvironmentLight & envrnmntLight, const DirectionLight & directionLight, const DotLight & dotLight);

	void SetEnvironmentLight(const EnvironmentLight & environmentLight);
	void SetDirectionLight(const DirectionLight & directionLight);
	void SetDotLight(const DotLight & dotLight);
	EnvironmentLight & GetEnvironmentLight();
	DirectionLight & GetDirectionLight();
	DotLight & GetDotLight();
	const EnvironmentLight & GetEnvironmentLight() const;
	const DirectionLight & GetDirectionLight() const;
	const DotLight & GetDotLight() const;

	LightColor VecVertexColor(const Vertex & position, const Vector & vector);
};


class ZBuffer
{
private:
	long		m_width;
	long		m_height;
	double *	m_buffer;
	
public:
	ZBuffer();
	ZBuffer(const ZBuffer & zBuffer);
	ZBuffer(long width, long height);
	
	void SetWidth(long width);
	void SetHegiht(long Height);
	void SetBuffer(double * buffer);
	void SetAll(const ZBuffer & zBuffer);

	long GetWidth() const;
	long GetHeight() const;
	double * GetBuffer() const;

	bool Compare(long x, long y, double z);
	void Write(long x, long y, double z);
	void Clear(double value);

	double & operator () (long x, long y);

	~ZBuffer();
};




class TBuffer
{
private:
	long		m_operateValue;
	long		m_width;
	long		m_height;
	int			m_way;
	int			m_value;
	int			* m_buffer;

public:
	TBuffer();
	TBuffer(long width, long height);
	TBuffer(TBuffer & TBuffer);

	void SetWidth(long width);
	void SetHegiht(long Height);
	void SetBuffer(int * buffer);
	void SetWay(int way);
	void SetValue(int value);
	void SetAll(const TBuffer & TBuffer);
	void SetOperatroValue(int operatorValue);

	long GetWidth() const;
	long GetHeight() const;
	int GetWay() const;
	int GetValue() const;
	int GetOperatroValue() const;
	int * GetBuffer() const;

	bool Compare(long x, long y);
	void Write(long x, long y);
	void Operate(int x, int y);

	void Clear(int value);

	int & operator () (long x, long y);

	~TBuffer();
};



class ColorBuffer
{
private:
	bool			m_blendEnable;
	LightColor		m_blendSrc;
	LightColor		m_blendDest;

	bool			m_drawStyle;
	HWND			m_hWnd;
	RECT			m_winRect;
	long			m_middleX;
	long			m_middleY;
	long			m_winWidth;
	long			m_winHeight;
	HDC				m_hdc;
	HDC				m_mdc;
public:
	ColorBuffer();
	ColorBuffer(const ColorBuffer & colorBuffer);

	void SetBlendEnable(bool isEnable);
	void SetBlendSrc(const LightColor & lightColor);
	void SetBlendDest(const LightColor & lightColor);
	void SetDrawStyle(bool style);
	void SetHWnd(const HWND & hWnd);
	void SetRect(const RECT & Rect);
	bool GetDrawStyle() const;
	long GetMiddleX();
	long GetMiddleY();
	
	bool GetBlendEnable();
	HWND GetHWnd();
	RECT & GetWinRect();
	HDC & GetHDC();
	
	const HWND GetHWnd() const;
	const RECT & GetWinRect() const;
	const HDC & GetHDC() const;

	void Write(long x, long y, int color);

	void DrawLine(int x1, int y1, int x2, int y2) const;

	void Clear(int color);
	void Present();
};



class Texture
{
private:
	BITMAP		m_bitmap;
	HBITMAP		m_hBitmap;
	HDC			m_hdc;
	HDC			m_mdc;

public:
	Texture();
	Texture(const char * fileName, const HDC hdc);
	void SetHdc(const HDC & hdc);
	void SetBitmap(const char * fileName);
	LightColor GetTextureColor(double x, double y);
};



class BackSystem
{
private:
	HWND			m_hWnd;
	RECT			m_winRect;
	RECT			m_viewportRect;
	Matrix			m_scaleMatrix;
	Matrix			m_viewport;
	Matrix			m_projection;
	Matrix			m_worldMatrix;

	Camera			m_camera;
	Light			m_light;
	Material		m_material;
	bool			m_textureEnable;
	Texture			m_texture;
	ColorBuffer		m_colorBuffer;
	bool			m_zBufferEnable;
	ZBuffer			m_zBuffer;
	bool			m_tBufferEnable;
	TBuffer	m_tBuffer;
	
	bool			m_deasil;
public:
	BackSystem();
	BackSystem(const BackSystem & BackSystem);

	void SetWorldMatrix(const Matrix & worldMatrix);
	void SetViewport(long x, long y, long width, long height, long minZ, long maxZ);
	void SetProjection(double fov, double Aspect, double zn, double zf);
	void SetZBufferEnable(bool isEnable);
	void SetTBufferEnable(bool isEnable);
	void SetTextureEnable(bool isEnable);
	void SetCamera(const Camera & camera);
	void SetLight(const Light & light);
	void SetTexture(const char * namefile);
	void SetMaterial(const LightColor & lightColor);
	void SetZBuffer(const ZBuffer & zBuffer);
	void SetTBuffer(const TBuffer & TBuffer);
	void SetColorBuffer(const ColorBuffer & colorBuffer);
	void SetHWnd(const HWND & hWnd);
	void SetDeasil(bool isDeasil);

	Matrix & BackSystem::GetScaleMatrix();
	bool GetTextureEnable();
	bool GetZBufferEnable();
	bool GetTBufferEnable();
	Matrix & GetWorldMatrix();
	Matrix & GetViewport();
	Matrix & GetProjection();
	Camera & GetCamera();
	Light & GetLight();
	Material & GetMaterial();
	Texture & GetTextrue();
	ColorBuffer & GetColorBuffer();
	ZBuffer & GetZBuffer();
	TBuffer & GetTBuffer();
	bool GetDeasil();
	const Matrix & BackSystem::GetScaleMatrix() const;
	const Matrix & GetWorldMatrix() const;
	const Matrix & GetViewport() const;
	const Matrix & GetProjection() const;
	const Camera & GetCamera() const;
	const Light & GetLight() const;
	const Material & GetMaterial() const;
	const Texture & GetTexture() const;
	const ColorBuffer & GetColorBuffer() const;
	const ZBuffer & GetZBuffer() const;
	const TBuffer & GetTBuffer() const;
	
	bool isVisible(const VecTriangle & vecTriangle);

	void Render(const VecTriangle & vecTriangle);
	void Line3D(const Vertex & v1, const Vertex & v2);
	void Pixel(int x, int y, double z, int color);
};



class Cube
{
private:
	double		m_edgeLength;
	VecTriangle	m_vecTriangle[12];
public:
	Cube();
	Cube(double edgeLength);

	void VectorUniform();
	void Render();
};



class Sphere
{
private:
	double			m_radius;
	int				m_vertexNum;
	VecTriangle		* m_pVecTriangle;
public:
	Sphere();
	Sphere(double radius, int vertexNum);
	
	void VectorUniform();
	void Render();
	~Sphere();
};



class Column
{
private:
	double		m_radius;
	double		m_high;
	int			m_vertexNum;
	VecTriangle	* m_pVecTriangle;

public:
	Column();
	Column(double radius, double high, int vertexNum);
	
	void VectorUniform();
	void Render();
	~Column();
};




class Cirque
{
private:
	double			m_insideRadius;
	double			m_outsideRadius;
	int				m_circleNum;
	int				m_vertexNum;
	VecTriangle *	m_pVecTriangle;
public:
	Cirque();
	Cirque(double insideRadius, double outsideRadius, int circleNum, int vertexNum);
	
	void VectorUniform();
	void Render();

	~Cirque();
};




class Space
{
private:
	Vector	m_vector;
	Vertex	m_vertex;
public:
	Space();
	Space(const Vector & vector, const Vertex & vertex);
	Matrix Shadow(const Vector & vector) const;
	Matrix Reflect() const;
};
class Surface
{
private:
	VecTriangle m_vecTriangle0;
	VecTriangle m_vecTriangle1;
public:
	Surface();
	Surface(const Vertex & Vertex0, const Vertex & Vertex1, const Vertex & Vertex2);
	void Render();
};




#endif