
#include "myDx.h"


BackSystem	g_backSystem;

 
void nextX0(long  * x, long * y, long * d, long incd0, long incd1, long dx)
{
	while((*d) <= 0)
	{
		(*d) += incd0;
		(*x) += dx;
	}
 
	(*d) += incd1;
	(*x) += dx;
	(*y)--;
}
 
void nextX1(long  * x, long * y, long * d, long incd0, long incd1, long dx)
{
	if((*d) < 0)
	{
		(*d) += incd0;
	}
	else
	{
		(*d) += incd1;
		(*x) += dx;
	}
	(*y)--;
}






Material::Material():m_lightColor()
{}

Material::Material(int red, int green, int blue):m_lightColor(red, green, blue)
{}
 
LightColor & Material::GetLightColor()
{
	return m_lightColor;
}

const LightColor & Material::GetLightColor() const
{
	return m_lightColor;
}






Texture::Texture()
{}

Texture::Texture(const char * fileName, const HDC hdc)
{
	m_hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); 
	GetObject(m_hBitmap,   sizeof(BITMAP),   &m_bitmap);

	m_hdc = hdc;
	m_mdc = CreateCompatibleDC(m_hdc);
	SelectObject(m_mdc, m_hBitmap);
}

void Texture::SetHdc(const HDC & hdc)
{
	m_hdc = hdc;
	m_mdc = CreateCompatibleDC(m_hdc);
}

void Texture::SetBitmap(const char * fileName)
{
	m_hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); 
	GetObject(m_hBitmap,   sizeof(BITMAP),   &m_bitmap);
	SelectObject(m_mdc, m_hBitmap);
}

LightColor Texture::GetTextureColor(double x, double y)
{
	int color = GetPixel(m_mdc, (int)(x * m_bitmap.bmWidth + 0.5f), (int)(y * m_bitmap.bmHeight + 0.5f));
	return LightColor(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff);
}







ColorBuffer::ColorBuffer()
{
	m_blendEnable = false;
}

ColorBuffer::ColorBuffer(const ColorBuffer & colorBuffer)
{
	SetHWnd(colorBuffer.GetHWnd());
}

void ColorBuffer::SetBlendEnable(bool isEnable)
{
	m_blendEnable = isEnable;
}

void ColorBuffer::SetBlendSrc(const LightColor & lightColor)
{
	m_blendSrc = lightColor;
}

void ColorBuffer::SetBlendDest(const LightColor & lightColor)
{
	m_blendDest = lightColor;
}

void ColorBuffer::SetDrawStyle(bool style)
{
	m_drawStyle = style;
}


void ColorBuffer::SetHWnd(const HWND & hWnd)
{
	m_hWnd = hWnd;
	m_hdc = GetDC(m_hWnd);
	GetClientRect(m_hWnd, &m_winRect);

	m_winWidth = m_winRect.right - m_winRect.left;
	m_winHeight = m_winRect.bottom - m_winRect.top;
	m_middleX = m_winWidth / 2;
	m_middleY = m_winHeight / 2;

	m_mdc = CreateCompatibleDC(m_hdc);
	HBITMAP hbmp = (HBITMAP)LoadImage(NULL, "ColorBuffer.bmp", IMAGE_BITMAP, m_winWidth, m_winHeight, LR_LOADFROMFILE);
	
	SelectObject(m_mdc, hbmp);
}

void ColorBuffer::SetRect(const RECT & Rect)
{
	this->m_winRect = Rect;
}

bool ColorBuffer::GetDrawStyle() const
{
	return m_drawStyle;
}

bool ColorBuffer::GetBlendEnable()
{
	return m_blendEnable;
}

HWND ColorBuffer::GetHWnd()
{
	return m_hWnd;
}

RECT & ColorBuffer::GetWinRect()
{
	return m_winRect;
}

HDC & ColorBuffer::GetHDC()
{
	return m_hdc;
}

long ColorBuffer::GetMiddleX()
{
	return m_middleX;
}

long ColorBuffer::GetMiddleY()
{
	return m_middleY;
}


const HWND ColorBuffer::GetHWnd() const
{
	return m_hWnd;
}


const RECT & ColorBuffer::GetWinRect() const
{
	return m_winRect;
}

const HDC & ColorBuffer::GetHDC() const
{
	return m_hdc;
}

void ColorBuffer::Write(long x, long y, int color)
{
	if(m_blendEnable)
	{
		LightColor src(color);
		LightColor dest(GetPixel(m_mdc, x, y));
		SetPixel(m_mdc, x, y, (src.Mul(m_blendSrc) + dest.Mul(m_blendDest)).ToUInt());
	}
	else
	{
		SetPixel(m_mdc, x, y, color);
	}
}

void ColorBuffer::DrawLine(int x1, int y1, int x2, int y2) const
{
	MoveToEx(m_mdc, x1, y1, NULL);
	LineTo(m_mdc, x2, y2);
}

void ColorBuffer::Clear(int color)
{
	HBRUSH * temp = (HBRUSH *)SelectObject(m_mdc, CreateSolidBrush(color));
	Rectangle(m_mdc, m_winRect.left, m_winRect.top, m_winRect.right, m_winRect.bottom);
	SelectObject(m_mdc, temp);
}

void ColorBuffer::Present()
{
	BitBlt(m_hdc, m_winRect.left, m_winRect.top, m_winWidth, m_winHeight, m_mdc, m_winRect.left, m_winRect.top, SRCCOPY);
}












BackSystem::BackSystem()
{
	m_deasil = true;
}

BackSystem::BackSystem(const BackSystem & backSystem)
{
	m_projection = backSystem.GetProjection();
	m_camera = backSystem.GetCamera();
	m_light = backSystem.GetLight();
	m_zBuffer = backSystem.GetZBuffer();
	m_tBuffer = backSystem.GetTBuffer();
}

void BackSystem::SetWorldMatrix(const Matrix & worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

void BackSystem::SetViewport(long x, long y, long width, long height, long minZ, long maxZ)
{
	long halfOfWidth = width / 2;
	long halfOfheight = height / 2;

	m_viewportRect.left = x;
	m_viewportRect.top = y;
	m_viewportRect.right = x + width;
	m_viewportRect.bottom = y + height;

	m_colorBuffer.SetRect(m_viewportRect);

	m_viewport = Matrix(halfOfWidth,		 0.0f,		  0.0f,  x + halfOfWidth,
						0.0f,		-halfOfheight,		  0.0f, y + halfOfheight,
						0.0f,				 0.0f, maxZ - minZ,				minZ,
						0.0f,				 0.0f,		  0.0f,				1.0f);
}

bool BackSystem::isVisible(const VecTriangle & vecTriangle)
{
	const Vector & CrossVector = vecTriangle.GetVector();
	const Vertex & middleVertex = vecTriangle.GetMiddleVertex();
	const Vector & DifferenceTom_camera = middleVertex - m_camera.GetPosition();

	if(!(DifferenceTom_camera.Dot(CrossVector) < 0.0f)) return !m_deasil;
	return m_deasil;
}

void BackSystem::SetDeasil(bool isDeasil)
{
	m_deasil = isDeasil;
}

bool BackSystem::GetDeasil()
{
	return m_deasil;
}

void BackSystem::SetProjection(double fov, double Aspect, double zn, double zf)
{
	double fovTan = tan(fov / 2.0f);
	Matrix temp0(1.0f / fovTan, 0.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f,
						  0.0f, 0.0f, 0.0f, 1.0f);

	Matrix temp1(1.0f,				  0.0f, 0.0f, 0.0f,
				 0.0f, (Aspect) / (fovTan), 0.0f, 0.0f,
				 0.0f,				  0.0f, 1.0f, 0.0f,
				 0.0f,				  0.0f, 0.0f, 1.0f);

	double t = (1.0f / zf);

	Matrix temp2(	t,	0.0f, 0.0f, 0.0f,
				 0.0f,	   t, 0.0f, 0.0f,
				 0.0f,  0.0f,	 t, 0.0f,
				 0.0f,	0.0f, 0.0f, 1.0f);

	m_scaleMatrix = temp2 * temp1 * temp0;

	zn = (zn / zf);
	zf = 1.0f;

	m_projection = Matrix(1.0f, 0.0f,				0.0f,			 0.0f,
						  0.0f, 1.0f,	 			0.0f,			 0.0f,
						  0.0f, 0.0f,  -1.0f / (zn - zf),  zn / (zn - zf),
						  0.0f, 0.0f,		   1.0f / zf,			 0.0f);
}

void BackSystem::SetTextureEnable(bool isEnable)
{
	m_textureEnable = isEnable;
}

void BackSystem::SetZBufferEnable(bool isEnable)
{
	m_zBufferEnable = isEnable;
}

void BackSystem::SetTBufferEnable(bool isEnable)
{
	m_tBufferEnable = isEnable;
}

void BackSystem::SetCamera(const Camera & camera)
{
	m_camera = camera;
}

void BackSystem::SetLight(const Light & light)
{
	m_light = light;
}

void BackSystem::SetMaterial(const LightColor & lightColor)
{
	m_material.GetLightColor().SetAll(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue());
}

void BackSystem::SetTexture(const char * filename)
{
	m_texture.SetBitmap(filename);
}

Texture & BackSystem::GetTextrue()
{
	return m_texture;
}

void BackSystem::SetColorBuffer(const ColorBuffer & colorBuffer)
{
	 m_colorBuffer = colorBuffer;
}

void BackSystem::SetZBuffer(const ZBuffer & zBuffer)
{
	m_zBuffer.SetAll(zBuffer);
}

void BackSystem::SetTBuffer(const TBuffer & TBuffer)
{
	m_tBuffer.SetAll(TBuffer);
}

void BackSystem::SetHWnd(const HWND & hWnd)
{
	m_hWnd = hWnd;
	
	GetClientRect(m_hWnd, &m_winRect);

	long width = m_winRect.right - m_winRect.left;
	long height = m_winRect.bottom - m_winRect.top;

	m_colorBuffer.SetHWnd(m_hWnd);
	
	m_zBuffer.SetAll(ZBuffer(width, height));

	m_tBuffer.SetAll(TBuffer(width, height));
	
	m_texture.SetHdc(GetDC(hWnd));
}

bool BackSystem::GetZBufferEnable()
{
	return m_zBufferEnable;
}

bool BackSystem::GetTBufferEnable()
{
	return m_tBufferEnable;
}

Material & BackSystem::GetMaterial()
{
	return m_material;
}

const Material & BackSystem::GetMaterial() const
{
	return m_material;
}

bool BackSystem::GetTextureEnable()
{
	return m_textureEnable;
}

Matrix & BackSystem::GetScaleMatrix()
{
	return m_scaleMatrix;
}

Matrix & BackSystem::GetProjection()
{
	return m_projection;
}

Matrix & BackSystem::GetViewport()
{
	return m_viewport;
}

Matrix & BackSystem::GetWorldMatrix()
{
	return m_worldMatrix;
}


Camera & BackSystem::GetCamera()
{
	return m_camera;
}

Light & BackSystem::GetLight()
{
	return m_light;
}

ColorBuffer & BackSystem::GetColorBuffer()
{
	return m_colorBuffer;
}

ZBuffer & BackSystem::GetZBuffer()
{
	return m_zBuffer;
}

TBuffer & BackSystem::GetTBuffer()
{
	return m_tBuffer;
}

const Matrix & BackSystem::GetScaleMatrix() const
{
	return m_scaleMatrix;
}

const Matrix & BackSystem::GetProjection() const
{
	return m_projection;
}

const Camera & BackSystem::GetCamera() const
{
	return m_camera;
}

const Light & BackSystem::GetLight() const
{
	return m_light;
}

const Texture & BackSystem::GetTexture() const
{
	return m_texture;
}

const ColorBuffer & BackSystem::GetColorBuffer() const
{
	return m_colorBuffer;
}

const ZBuffer & BackSystem::GetZBuffer() const
{
	return m_zBuffer;
}

const TBuffer & BackSystem::GetTBuffer() const
{
	return m_tBuffer;
}




void BackSystem::Render(const VecTriangle & vecTriangle)
{
	VecTriangle	triangleOfNotProject = m_worldMatrix * vecTriangle;

	if(!isVisible(triangleOfNotProject)) return;

	VecTriangle triangleOfHadProject = vecTriangle.Project();

	if(m_colorBuffer.GetDrawStyle())
	{
		Line3D(triangleOfHadProject.GetVecVertex(0).GetPosition(), triangleOfHadProject.GetVecVertex(1).GetPosition());
		Line3D(triangleOfHadProject.GetVecVertex(1).GetPosition(), triangleOfHadProject.GetVecVertex(2).GetPosition());
		Line3D(triangleOfHadProject.GetVecVertex(2).GetPosition(), triangleOfHadProject.GetVecVertex(0).GetPosition());
		return;
	}

	int index = triangleOfHadProject.GetMaxYIndex();
	if(0 != index)
	{
		triangleOfHadProject.ExchangeVecVertex(0, index);
		triangleOfNotProject.ExchangeVecVertex(0, index);
	}
	
	if(triangleOfHadProject.GetVecVertex(1).GetPosition().GetY() < triangleOfHadProject.GetVecVertex(2).GetPosition().GetY())
	{
		triangleOfHadProject.ExchangeVecVertex(1, 2);
		triangleOfNotProject.ExchangeVecVertex(1, 2);
	}

	Vector v0 = triangleOfNotProject.GetVecVertex(0).GetVector().Identity();
	Vector v1 = triangleOfNotProject.GetVecVertex(1).GetVector().Identity();
	Vector v2 = triangleOfNotProject.GetVecVertex(2).GetVector().Identity();
	
	double x[3];
	double y[3];
	double z[3];

	x[0] = triangleOfHadProject.GetVecVertex(0).GetPosition().GetX();
	y[0] = triangleOfHadProject.GetVecVertex(0).GetPosition().GetY();
	z[0] = triangleOfHadProject.GetVecVertex(0).GetPosition().GetZ();

	x[1] = triangleOfHadProject.GetVecVertex(1).GetPosition().GetX();
	y[1] = triangleOfHadProject.GetVecVertex(1).GetPosition().GetY();
	z[1] = triangleOfHadProject.GetVecVertex(1).GetPosition().GetZ();

	x[2] = triangleOfHadProject.GetVecVertex(2).GetPosition().GetX();
	y[2] = triangleOfHadProject.GetVecVertex(2).GetPosition().GetY();
	z[2] = triangleOfHadProject.GetVecVertex(2).GetPosition().GetZ();

	double nx[3];
	double ny[3];
	double nz[3];
	
	nx[0] = triangleOfNotProject.GetVecVertex(0).GetPosition().GetX();
	ny[0] = triangleOfNotProject.GetVecVertex(0).GetPosition().GetY();
	nz[0] = triangleOfNotProject.GetVecVertex(0).GetPosition().GetZ();

	nx[1] = triangleOfNotProject.GetVecVertex(1).GetPosition().GetX();
	ny[1] = triangleOfNotProject.GetVecVertex(1).GetPosition().GetY();
	nz[1] = triangleOfNotProject.GetVecVertex(1).GetPosition().GetZ();

	nx[2] = triangleOfNotProject.GetVecVertex(2).GetPosition().GetX();
	ny[2] = triangleOfNotProject.GetVecVertex(2).GetPosition().GetY();
	nz[2] = triangleOfNotProject.GetVecVertex(2).GetPosition().GetZ();

	long x0 = (long)triangleOfHadProject.GetVecVertex(0).GetPosition().GetX();
	long y0 = (long)triangleOfHadProject.GetVecVertex(0).GetPosition().GetY();
	long x1 = (long)triangleOfHadProject.GetVecVertex(0).GetPosition().GetX();
	long y1 = (long)triangleOfHadProject.GetVecVertex(0).GetPosition().GetY();
	
	long x2 = (long)triangleOfHadProject.GetVecVertex(1).GetPosition().GetX();
	long y2 = (long)triangleOfHadProject.GetVecVertex(1).GetPosition().GetY();
	long x3 = (long)triangleOfHadProject.GetVecVertex(2).GetPosition().GetX();
	long y3 = (long)triangleOfHadProject.GetVecVertex(2).GetPosition().GetY();

	long dx0 = x2 - x0;
	long dy0 = y2 - y0;
	long absdx0 = abs(dx0);
    long absdy0 = abs(dy0);
	dx0 = dx0 > 0 ? 1 : -1;

	long d0;
	long incd01;
	long incd02;

    if(absdy0 < absdx0)
    {
		d0 = 2 * absdy0 - absdx0;
		incd01 = 2 * absdy0;
		incd02 = 2 * ( absdy0 - absdx0);
    }
    else
    {
		d0 = 2 * absdx0 - absdy0;
		incd01 = 2 * absdx0;
		incd02 = 2 * (absdx0 - absdy0);
    }

	long dx1 = x3 - x1;
	long dy1 = y3 - y1;
	long absdx1 = abs(dx1);
    long absdy1 = abs(dy1);
	dx1 = dx1 > 0 ? 1 : -1;

	long d1;
	long incd11;
	long incd12;

    if(absdy1 < absdx1)
    {
		d1 = 2 * absdy1 - absdx1;
		incd11 = 2 * absdy1;
		incd12 = 2 * ( absdy1 - absdx1);
    }
    else
    {
		d1 = 2 * absdx1 - absdy1;
		incd11 = 2 * absdx1;
		incd12 = 2 * (absdx1 - absdy1);
    }
	
	
	long tx0;
	long tx1;



	
	
	double t1,t2,t3;
	double z1,z2,z3;


	LightColor c0,c1,c2;

	long top =  max(m_viewportRect.top, m_winRect.top);
	long bottom = min(m_viewportRect.bottom, m_winRect.bottom);
	long left = max(m_viewportRect.left, m_winRect.left);
	long right = min(m_viewportRect.right, m_winRect.right);

	while(y1 > y2)
	{
		if(absdx0 > absdy0)
		{
			nextX0(&x0, &y0, &d0, incd01, incd02, dx0);
		}
		else
		{
			nextX1(&x0, &y0, &d0, incd01, incd02, dx0);
		}

		if(absdx1 > absdy1)
		{
			nextX0(&x1, &y1, &d1, incd11, incd12, dx1);
		}
		else
		{
 			nextX1(&x1, &y1, &d1, incd11, incd12, dx1);
		}

		t1 = sqrt(pow(x0 - x[0],2) + pow(y0 - y[0], 2)) / sqrt(pow(x[1] - x[0],2) + pow(y[1] - y[0], 2));
		z1 = z[0] * (1 - t1) + z[1] * t1;
		VecVertex temp0 = triangleOfNotProject.GetVecVertex(0) * (1 - t1) + triangleOfNotProject.GetVecVertex(1) * t1;

		t2 = sqrt(pow(x1 - x[0],2) + pow(y1 - y[0], 2)) / sqrt(pow(x[2] - x[0],2) + pow(y[2] - y[0], 2));
		z2 = z[0] * (1 - t2) + z[2] * t2;
		VecVertex temp1 = triangleOfNotProject.GetVecVertex(0) * (1 - t2) + triangleOfNotProject.GetVecVertex(2) * t2;

		tx0 = x0;
		tx1 = x1;

		if(tx0 > tx1)
		{
			tx0 = x1; tx1 = x0;
			
			double tz;
			tz = z1;
			z1 = z2;
			z2 = tz;

			VecVertex temp;
			temp = temp0;
			temp0 = temp1;
			temp1 = temp;
		}

		if(	y1 < top || 
			y1 >= bottom || 
			tx1 < left ||
			tx0 >= right)
		{
			continue;
		}

		for(int i = (tx0 < left) ? left : tx0; i < ((tx1 < right) ? tx1 : right); i++)
		{
			t3 = ((double)(i - tx0)) / (tx1 - tx0);
			z3 = (z1 * (1 - t3)) + (z2 * t3);

			if(!(!m_zBufferEnable || (m_zBuffer.Compare(i, y1, z3) && m_zBufferEnable))) continue;

			VecVertex temp2 = (temp0 * (1 - t3)) + (temp1 * t3);

			c0 = m_texture.GetTextureColor(temp2.GetTx(), temp2.GetTy());
			c1 = m_light.VecVertexColor(temp2.GetPosition(), temp2.GetVector().Identity());

			if(GetBackSystem().GetTextureEnable())
			{
				c2 = c1.Mul(c0);
			}
			else
			{
				c2 = c1.Mul(m_material.GetLightColor());
			}
			Pixel(i, y0, z3, c2.ToUInt());
		}
	}
	
	x0 = x2;
  
	dx0 = x3 - x2;
    dy0 = y3 - y2;
    absdx0 = abs(dx0);
	absdy0 = abs(dy0);
	dx0 = dx0 > 0 ? 1 : -1;
    
    if(absdx0 > absdy0)
	{
		d0 = 2 * absdy0 - absdx0;
		incd01=2 * absdy0;
		incd02=2 * (absdy0 - absdx0);
	}
    else
    {
		d0 = 2 * absdx0 - absdy0;
		incd01 = 2 * absdx0;
		incd02 = 2 * (absdx0 - absdy0);
    }

	while(y1 > y3)
	{
		if(absdx0 > absdy0)
		{
			nextX0(&x0, &y0, &d0, incd01, incd02, dx0);
		}
		else
		{
 			nextX1(&x0, &y0, &d0, incd01, incd02, dx0);
		}

		if(absdx1 > absdy1)
		{
			nextX0(&x1, &y1, &d1, incd11, incd12, dx1);
		}
		else
		{
 			nextX1(&x1, &y1, &d1, incd11, incd12, dx1);
		}

		t1 = sqrt(pow(x0 - x[1],2) + pow(y0 - y[1], 2)) / sqrt(pow(x[2] - x[1],2) + pow(y[2] - y[1], 2));
		z1 = z[1] * (1 - t1) + z[2] * t1;
		VecVertex temp0 = triangleOfNotProject.GetVecVertex(1) * (1 - t1) + triangleOfNotProject.GetVecVertex(2) * t1;

		t2 = sqrt(pow(x1 - x[0],2) + pow(y1 - y[0], 2)) / sqrt(pow(x[2] - x[0],2) + pow(y[2] - y[0], 2));
		z2 = z[0] * (1 - t2) + z[2] * t2;
		VecVertex temp1 = triangleOfNotProject.GetVecVertex(0) * (1 - t2) + triangleOfNotProject.GetVecVertex(2) * t2;

		tx0 = x0;
		tx1 = x1;

		if(tx0 > tx1)
		{
			tx0 = x1; tx1 = x0;
			
			double tz;
			tz = z1;
			z1 = z2;
			z2 = tz;

			VecVertex temp;
			temp = temp0;
			temp0 = temp1;
			temp1 = temp;
		}

		if(	y1 < top ||
			y1 >= bottom ||
			tx1 < left ||
			tx0 >= right)
		{
			continue;
		}

		for(int i = (tx0 < left) ? left : tx0; i < ((tx1 < right) ? tx1 : right); i++)
		{
			t3 = ((double)(i - tx0)) / (tx1 - tx0);

			z3 = z1 * (1 - t3) + z2 * t3;

			if(!(!m_zBufferEnable || (m_zBuffer.Compare(i, y1, z3) && m_zBufferEnable))) continue;

			VecVertex temp2 = (temp0 * (1 - t3)) + (temp1 * t3);

			c0 = m_texture.GetTextureColor(temp2.GetTx(), temp2.GetTy());
			c1 = m_light.VecVertexColor(temp2.GetPosition(), temp2.GetVector().Identity());

			if(GetBackSystem().GetTextureEnable())
			{
				c2 = c1.Mul(c0);
			}
			else
			{
				c2 = c1.Mul(m_material.GetLightColor());
			}
			Pixel(i, y1, z3, c2.ToUInt());
		}
	}
}



void BackSystem::Line3D(const Vertex & v1, const Vertex & v2)
{	
	m_colorBuffer.DrawLine((int)v1.GetX(), (int)v1.GetY(), (int)v2.GetX(), (int)v2.GetY());
}

void BackSystem::Pixel(int x, int y, double z, int color)
{
	long tx = x;
	long ty = y;

	if((tx > m_colorBuffer.GetWinRect().left) &&
	   (tx < m_colorBuffer.GetWinRect().right) &&
	   (ty > m_colorBuffer.GetWinRect().top) &&
	   (ty < m_colorBuffer.GetWinRect().bottom) &&
	   (z > 0.0f)
	   )
	{
		bool testTBuf = m_tBuffer.Compare(tx, ty);
		bool testZBuf = m_zBuffer.Compare(tx, ty, z);

		if(m_tBufferEnable)
		{
			if(m_zBufferEnable)
			{
				if(testTBuf)
				{
					if(testZBuf)
					{
						m_tBuffer.Write(tx, ty);
						m_zBuffer.Write(tx, ty, z);
						m_colorBuffer.Write(tx, ty, color);
					}
				}
			}
			else
			{
				if(testTBuf)
				{
					m_tBuffer.Write(tx, ty);
					m_colorBuffer.Write(tx, ty, color);
				}
			}
		}
		else
		{
			if(m_zBufferEnable)
			{
				if(testZBuf)
				{
					m_zBuffer.Write(tx, ty, z);
					m_colorBuffer.Write(tx, ty, color);
				}
			}
			else
			{
				m_colorBuffer.Write(tx, ty, color);
			}
		}
	}
}








TBuffer::TBuffer()
{
	m_width = 0;
	m_height = 0;
	m_way = 0;
	m_value = 0;
	m_buffer = NULL;
}

TBuffer::TBuffer(long width, long height)
{
	m_width = width;
	m_height = height;
	m_way = 0;
	m_value = 0;
	m_buffer = new int[m_width * m_height];;
}

TBuffer::TBuffer(TBuffer & TBuffer)
{
	m_width = TBuffer.GetWidth();
	m_height = TBuffer.GetHeight();
	m_way = TBuffer.GetWay();
	m_value = TBuffer.GetValue();
	m_buffer = new int[m_width * m_height];;
}

void TBuffer::SetWidth(long width)
{
	m_width = width;
}

void TBuffer::SetHegiht(long height)
{
	m_height = height;
}

void TBuffer::SetWay(int way)
{
	m_way = way;
}

void TBuffer::SetValue(int value)
{
	m_value = value;
}

void TBuffer::SetBuffer(int * buffer)
{
	m_buffer = buffer;
}

void TBuffer::SetAll(const TBuffer & TBuffer)
{
	m_width = TBuffer.GetWidth();
	m_height = TBuffer.GetHeight();
	m_way = TBuffer.GetWay();
	m_value = TBuffer.GetValue();
	m_buffer = new int[m_width * m_height];
}

void TBuffer::SetOperatroValue(int operateValue)
{
	m_operateValue = operateValue;
}

long TBuffer::GetWidth() const
{
	return m_width;
}

long TBuffer::GetHeight() const
{
	return m_height;
}

int TBuffer::GetValue() const
{
	return m_value;
}



int TBuffer::GetWay() const
{
	return m_way;
}

int TBuffer::GetOperatroValue() const
{
	return m_operateValue;
}

int * TBuffer::GetBuffer() const
{
	return m_buffer;
}

bool TBuffer::Compare(long x, long y)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
	switch(m_way)
	{
	case TEMPLATE_WRITE:
		return true;
		break;
	case TEMPLATE_EQUA:
		if((*this)(x, y) == m_value)
		{
			return true;
		}
		return false;
		break;
	default:
		break;
	}
	return false;
}

void TBuffer::Write(long x, long y)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
	(*this)(x, y) = m_value;
	Operate(x,y);
}


void TBuffer::Operate(int x, int y)
{
	switch(m_operateValue)
	{
	case TBuffer_INC:
		{
			(*this)(x, y) ++;
		}
		break;
	default:
		break;
	}
}

void TBuffer::Clear(int value)
{     
	for(int i = 0; i < m_width * m_height; i++)
	{
		m_buffer[i] = value;
	}
}

int & TBuffer::operator () (long x, long y)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
	return m_buffer[y * m_width + x];
}

TBuffer::~TBuffer()
{
	if(NULL == m_buffer) return;
	delete [] m_buffer;
	m_buffer = NULL;
}






ZBuffer::ZBuffer()
{
	m_width = 0;
	m_height = 0;
	m_buffer = NULL;
}

ZBuffer::ZBuffer(const ZBuffer & zBuffer)
{
	m_width = zBuffer.GetWidth();
	m_height = zBuffer.GetHeight();
	m_buffer = new double[m_width * m_height];;
}

ZBuffer::ZBuffer(long width, long height)
{
	m_width = width;
	m_height = height;
	m_buffer = new double[m_width * m_height];
}

void ZBuffer::SetWidth(long width)
{
	m_width = width;
}

void ZBuffer::SetHegiht(long height)
{
	m_height = height;
}

void ZBuffer::SetBuffer(double * buffer)
{
	m_buffer = buffer;
}

void ZBuffer::SetAll(const ZBuffer & zBuffer)
{
	m_width = zBuffer.GetWidth();
	m_height = zBuffer.GetHeight();
	m_buffer = new double[m_width * m_height];
}

long ZBuffer::GetWidth() const
{
	return m_width;
}

long ZBuffer::GetHeight() const
{
	return m_height;
}

double * ZBuffer::GetBuffer() const
{
	return m_buffer;
}

bool ZBuffer::Compare(long x, long y, double z)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height && z > 0.0f);
	if((*this)(x, y) > z)
	{
		return true;
	}
	return false;
}

void ZBuffer::Write(long x, long y, double z)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height && z > 0.0f);
	(*this)(x, y) = z;
}

void ZBuffer::Clear(double value)
{     
	for(int i = 0; i < m_width * m_height; i++)
	{
		m_buffer[i] = value;
	}
}       


double & ZBuffer::operator () (long x, long y)
{
	assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
	return m_buffer[y * m_width + x];
}

ZBuffer::~ZBuffer()
{
	if(NULL == m_buffer) return;
	delete [] m_buffer;
	m_buffer = NULL;
}


LightColor::LightColor()
{
	m_red = 0;
	m_green = 0;
	m_blue = 0;
}

LightColor::LightColor(const LightColor & lightColor)
{
	SetAll(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue());
}

LightColor::LightColor(unsigned int red, unsigned int green, unsigned int blue)
{
	SetAll(red, green, blue);
	*this = Uniform();
}

LightColor::LightColor(unsigned int color)
{
	SetAll(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff);
}

LightColor LightColor::operator + (const LightColor & lightColor)
{
	return LightColor(m_red + lightColor.GetRed(), m_green + lightColor.GetGreen(), m_blue + lightColor.GetBlue());
}

void LightColor::SetRed(unsigned int red)
{
	m_red = red;
}

void LightColor::SetGreen(unsigned int green)
{
	m_green = green;
}

void LightColor::SetBlue(unsigned int blue)
{
	m_blue = blue;
}

void LightColor::SetAll(unsigned int red, unsigned int green, unsigned int blue)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
}

unsigned int LightColor::GetRed() const
{
	return m_red;
}

unsigned int LightColor::GetGreen() const
{
	return m_green;
}

unsigned int LightColor::GetBlue() const
{
	return m_blue;
}

LightColor LightColor::Uniform()
{
	LightColor temp;
	temp = *this;
	
	if(m_red > 0xff) temp.SetRed(0xff);
	if(m_green > 0xff) temp.SetGreen(0xff);
	if(m_blue > 0xff) temp.SetBlue(0xff);

	return temp;
}

unsigned int LightColor::ToUInt()
{
	LightColor temp = Uniform();
	return ((temp.GetBlue() & 0xff) << 16) | ((temp.GetGreen() & 0xff) << 8) | (temp.GetRed() & 0xff);
}

LightColor LightColor::Mul(const LightColor & lightColor)
{
	return LightColor(m_red * lightColor.GetRed() / 0xff, m_green * lightColor.GetGreen() / 0xff, m_blue * lightColor.GetBlue() / 0xff);
}



EnvironmentLight::EnvironmentLight()
{
	m_lightColor = LightColor(0, 0, 0);
}

EnvironmentLight::EnvironmentLight(const EnvironmentLight & environmentLight)
{
	m_lightColor = environmentLight.GetLightColor();
}

EnvironmentLight::EnvironmentLight(const LightColor & lightColor)
{
	m_lightColor = lightColor;
}

void EnvironmentLight::SetLightColor(const LightColor & lightColor)
{
	m_lightColor = lightColor;
}

LightColor & EnvironmentLight::GetLightColor()
{
	return m_lightColor;
}

const LightColor & EnvironmentLight::GetLightColor() const
{
	return m_lightColor;
}

LightColor EnvironmentLight::VecVertexColor()
{
	LightColor temp;
	double redColor = m_lightColor.GetRed();
	double greenColor = m_lightColor.GetGreen();
	double blueColor = m_lightColor.GetBlue();

	temp.SetAll((unsigned int)redColor, (unsigned int)greenColor, (unsigned int)blueColor);
	temp = temp.Uniform();

	return temp;
}





DirectionLight::DirectionLight()
{
	m_lightColor = LightColor(0, 0, 0);
	SetDirection(Vector(0.0f, 0.0f, 0.0f, 0.0f));
}

DirectionLight::DirectionLight(const DirectionLight & directionLight)
{
	m_lightColor = directionLight.GetLightColor();
	m_direction = directionLight.GetDirection();
}

DirectionLight::DirectionLight(const LightColor & lightColor, const Vector & direction)
{
	m_lightColor = lightColor;
	m_direction = direction;
}

void DirectionLight::SetLightColor(const LightColor & lightColor)
{
	m_lightColor = lightColor;
}

void DirectionLight::SetDirection(const Vector & direction)
{
	m_direction = direction;
}

LightColor & DirectionLight::GetLightColor()
{
	return m_lightColor;
}

Vector & DirectionLight::GetDirection()
{
	return m_direction;
}

const LightColor & DirectionLight::GetLightColor() const
{
	return m_lightColor;
}

const Vector & DirectionLight::GetDirection() const
{
	return m_direction;
}

LightColor DirectionLight::VecVertexColor(const Vector & vector)
{
	LightColor temp;
	double cs = (m_direction.Identity().Dot(vector.Identity()));

	if(cs < 0.0f) cs = 0.0f;
	
	temp.SetAll((unsigned int)(m_lightColor.GetRed() * cs), (unsigned int)(m_lightColor.GetGreen() * cs), (unsigned int)(m_lightColor.GetBlue() * cs));
	temp = temp.Uniform();

	return temp;
}




DotLight::DotLight()
{
	m_lightColor = LightColor(0, 0, 0);
	m_position = Vertex(0.0f, 0.0f, 0.0f, 0.0f);
	m_c1 = 1.0f;
	m_c2 = 0.0f;
	m_c3 = 0.0f;
}

DotLight::DotLight(const DotLight & dotLight)
{
	m_lightColor = dotLight.GetLightColor();
	m_position = dotLight.GetPosition();
}

DotLight::DotLight(const LightColor & lightColor, const Vertex & position, double c1, double c2, double c3)
{
	m_lightColor = lightColor;
	m_position = position;
	m_c1 = c1;
	m_c2 = c2;
	m_c3 = c3;
}

void DotLight::SetLightColor(const LightColor & lightColor)
{
	m_lightColor = lightColor;
}

void DotLight::SetPosition(const Vertex & position)
{
	m_position = position;
}

LightColor & DotLight::GetLightColor()
{
	return m_lightColor;
}

Vertex & DotLight::GetPosition()
{
	return m_position;
}

const LightColor & DotLight::GetLightColor() const
{
	return m_lightColor;
}

const Vertex & DotLight::GetPosition() const
{
	return m_position;
}

LightColor DotLight::VecVertexColor(const Vertex & position, const Vector & vector)
{
	LightColor temp;
	double cs = ((m_position - position).Identity().Dot(vector.Identity()));
	double distance = (m_position - position).Length();
	double coefficient = 1 / (m_c1 + (m_c2 * distance) + (m_c3 * distance * distance));

	if(cs < 0.0f) cs = 0.0f;

	double ccs = coefficient * cs;
	temp.SetAll((unsigned int)(ccs * m_lightColor.GetRed()), (unsigned int)(ccs * m_lightColor.GetGreen()), (unsigned int)(ccs * m_lightColor.GetBlue()));
	temp = temp.Uniform();

	return temp;
}








Light::Light()
{
	m_environmentLight = EnvironmentLight(LightColor(0,0,0));
	m_directionLight = DirectionLight(LightColor(0,0,0), Vector(0.0f, 0.0f, 0.0f, 0.0f));
	m_dotLight = DotLight(LightColor(0,0,0), Vertex(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0.0f, 0.0f);
}

Light::Light(const Light & light)
{
	m_environmentLight = light.GetEnvironmentLight();
	m_directionLight = light.GetDirectionLight();
	m_dotLight = light.GetDotLight();
}

Light::Light(const EnvironmentLight & envrnmntLight, const DirectionLight & directionLight, const DotLight & dotLight)
{
	m_environmentLight = envrnmntLight;
	m_directionLight = directionLight;
	m_dotLight = dotLight;
}

void Light::SetEnvironmentLight(const EnvironmentLight & environmentLight)
{
	m_environmentLight = environmentLight;
}

void Light::SetDirectionLight(const DirectionLight & directionLight)
{
	m_directionLight = directionLight;
}

void Light::SetDotLight(const DotLight & dotLight)
{
	m_dotLight = dotLight;
}

EnvironmentLight & Light::GetEnvironmentLight()
{
	return m_environmentLight;
}

DirectionLight & Light::GetDirectionLight()
{
	return m_directionLight;
}

DotLight & Light::GetDotLight()
{
	return m_dotLight;
}

const EnvironmentLight & Light::GetEnvironmentLight() const
{
	return m_environmentLight;
}

const DirectionLight & Light::GetDirectionLight() const
{
	return m_directionLight;
}

const DotLight & Light::GetDotLight() const
{
	return m_dotLight;
}

LightColor Light::VecVertexColor(const Vertex & position, const Vector & vector)
{
	LightColor temp = m_environmentLight.VecVertexColor() + m_directionLight.VecVertexColor(vector) + m_dotLight.VecVertexColor(position, vector);
	temp = temp.Uniform();

	return temp;
}













/*
*-----------------------
*	Class VecVertex function
*-----------------------
*/

 
VecVertex::VecVertex()
{
	SetAll(Vertex(0.0f, 0.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
}

VecVertex::VecVertex(const VecVertex & vecVertex)
{
	m_position = vecVertex.GetPosition();
	m_vector = vecVertex.GetVector();
	m_tx = vecVertex.GetTx();
	m_ty = vecVertex.GetTy();
}

VecVertex::VecVertex(const Vertex & position, const  Vector & vector, double tx, double ty ) 
{
	SetAll(position, vector, tx, ty);
}

void VecVertex::SetPosition(Vertex & position)
{
	m_position = position;
}

void VecVertex::SetVector(Vector & vector)
{
	m_vector = vector;
}

void VecVertex::SetTx(double tx)
{
	m_tx = tx;
}
void VecVertex::SetTy(double ty)
{
	m_ty = ty;
}

void VecVertex::SetTxy(double tx, double ty)
{
	m_tx = tx;
	m_ty = ty;
}

void VecVertex::SetAll(const Vertex & position, const  Vector & vector, double tx, double ty)
{
	m_position = position;
	m_vector = vector;
	m_tx = tx;
	m_ty = ty;
}

void VecVertex::SetAll(const VecVertex & vecVertex)
{
	SetAll(vecVertex.GetPosition(), vecVertex.GetVector(), vecVertex.GetTx(), vecVertex.GetTy());
}

Vertex & VecVertex::GetPosition()
{
	return m_position;
}

Vertex & VecVertex::GetVector()
{
	return m_vector;
}

const Vertex & VecVertex::GetPosition() const
{
	return m_position;
}

const Vertex & VecVertex::GetVector() const
{
	return m_vector;
}

double VecVertex::GetTx() const
{
	return m_tx;
}

double VecVertex::GetTy() const
{
	return m_ty;
}

VecVertex VecVertex::Project() const
{
	VecVertex temp = VecVertex(m_position.Project(), m_vector.Project(), m_tx, m_ty);
	return temp;
}

VecVertex VecVertex::operator * (double number) const
{
	return VecVertex(m_position * number, m_vector * number, m_tx * number, m_ty * number);
}

VecVertex VecVertex::operator + (const VecVertex & vecVertex) const
{
	return VecVertex(m_position + vecVertex.GetPosition(), m_vector + vecVertex.GetVector(), m_tx + vecVertex.GetTx(), m_ty + vecVertex.GetTy());
}

VecVertex::~VecVertex()
{
	
}




/*
*-----------------------
*	Class VecTriangle function
*-----------------------
*/

VecTriangle::VecTriangle()
{
	VecVertex temp = VecVertex(Vertex(0.0f, 0.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
	m_vecVertex0.SetAll(temp);
	m_vecVertex1.SetAll(temp);
	m_vecVertex2.SetAll(temp);
}

VecTriangle::VecTriangle(const VecTriangle & vecTriangle)
{
	m_vecVertex0 = vecTriangle.GetVecVertex(0);
	m_vecVertex1 = vecTriangle.GetVecVertex(1);
	m_vecVertex2 = vecTriangle.GetVecVertex(2);
}

VecTriangle::VecTriangle(const VecVertex & vecVertex0, const VecVertex & vecVertex1, const VecVertex & vecVertex2)
{
	m_vecVertex0.SetAll(vecVertex0);
	m_vecVertex1.SetAll(vecVertex1);
	m_vecVertex2.SetAll(vecVertex2);
}

void VecTriangle::SetVecVertex(int index, const VecVertex & vecVertex)
{
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			m_vecVertex0.SetAll(vecVertex);
			break;
		case 1:
			m_vecVertex1.SetAll(vecVertex);
			break;
		case 2:
			m_vecVertex2.SetAll(vecVertex);
			break;
		default:
			break;
	}
}

VecVertex & VecTriangle::GetVecVertex(int index)
{
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			return m_vecVertex0;
			break;
		case 1:
			return m_vecVertex1;
			break;
		case 2:
			return m_vecVertex2;
			break;
		default:
			break;
	}
	return m_vecVertex0;
}

const VecVertex & VecTriangle::GetVecVertex(int index) const
{
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			return m_vecVertex0;
			break;
		case 1:
			return m_vecVertex1;
			break;
		case 2:
			return m_vecVertex2;
			break;
		default:
			break;
	}
	return m_vecVertex0;
}


int VecTriangle::GetMaxYIndex() const
{
	double vecVertexY[3] = {m_vecVertex0.GetPosition().GetY(), m_vecVertex1.GetPosition().GetY(), m_vecVertex2.GetPosition().GetY()};

	if(vecVertexY[0] > vecVertexY[1] && vecVertexY[0] > vecVertexY[2]) return 0;
	else if(vecVertexY[1] > vecVertexY[2]) return 1;
	else return 2;
}

void VecTriangle::ExchangeVecVertex(int index0, int index1)
{
	VecVertex	temp;
	VecVertex * addrOfVecVertex[3] = {&m_vecVertex0, &m_vecVertex1, &m_vecVertex2};
	assert(index0 >= 0 && index0 < 3 && index1 >= 0 && index1 < 3 && index0 != index1);
	
	temp = *addrOfVecVertex[index0];
	*addrOfVecVertex[index0] = *addrOfVecVertex[index1];
	*addrOfVecVertex[index1] = temp;
}

VecTriangle VecTriangle::VectorUniform()
{
	VecTriangle temp = *this;
	Vector vector = GetVector();
	temp.GetVecVertex(0).SetVector(vector);
	temp.GetVecVertex(1).SetVector(vector);
	temp.GetVecVertex(2).SetVector(vector);
	return temp;
}

VecTriangle VecTriangle::Project() const
{
	VecTriangle temp = VecTriangle(m_vecVertex0.Project(), m_vecVertex1.Project(), m_vecVertex2.Project());
	return temp;
}





void VecTriangle::Render() const
{
	GetBackSystem().Render(*this);
	return;
}

Vector VecTriangle::GetVector() const
{
	Vector v0 = m_vecVertex1.GetPosition() - m_vecVertex0.GetPosition();
	Vector v1 = m_vecVertex2.GetPosition() - m_vecVertex0.GetPosition();
	return v0.Cross(v1);

} 

Vertex VecTriangle::GetMiddleVertex() const
{
	return (m_vecVertex0.GetPosition() + m_vecVertex1.GetPosition() + m_vecVertex2.GetPosition()) / 3.0f;
}

VecTriangle::~VecTriangle()
{

}




/*
*-----------------------
*	class Vertex function
*-----------------------
*/

Vertex::Vertex()
{
	m_x = m_y = m_z = 0.0f;
	m_w = 1.0f;
}

Vertex::Vertex(double x, double y, double z ,double w)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

Vertex::Vertex(const Vertex & vertex)
{
	*this = vertex;
}

void Vertex::SetX(double x)
{
	m_x = x;
}
 
void Vertex::SetY(double y)
{
	m_y = y;
}

void Vertex::SetZ(double z)
{
	m_z = z;
}

void Vertex::SetW(double w)
{
	m_w = w;
}

double Vertex::GetX() const
{
	return m_x;
}

double Vertex::GetY() const
{
	return m_y;
}

double Vertex::GetZ() const
{
	return m_z;
}

double Vertex::GetW() const
{
	return m_w;
}

Vertex Vertex::Uniform() const
{
	double t = 1.0f / m_w;

	return Vertex(m_x * t,  m_y * t, m_z * t, 1.0f);
}

Vertex Vertex::Project() const
{
	BackSystem & backSystem = GetBackSystem();
/*
	Matrix temp = backSystem.GetProjection();

	Vertex t = backSystem.GetScaleMatrix() * 
			backSystem.GetCamera().GetCameraMatrix() * 
			backSystem.GetWorldMatrix() * *this;

	temp(2, 2) = temp(2, 2) * t.GetZ();
	
	return (backSystem.GetViewport() * 
			temp * t).Uniform();
*/
	
	return (backSystem.GetViewport() * 
			backSystem.GetProjection() * 
			backSystem.GetScaleMatrix() * 
			backSystem.GetCamera().GetCameraMatrix() * 
			backSystem.GetWorldMatrix() * 
			(*this)).Uniform();
}

Vertex Vertex::Identity() const
{
	double len = sqrt(pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2));

	return Vertex(m_x / len, m_y / len, m_z / len, m_w);
}

double Vertex::Dot(const Vector & vector) const
{
	return  m_x * vector.GetX() + m_y * vector.GetY() + m_z * vector.GetZ();
}

Vector Vertex::Cross(const Vector & vector) const
{
	return Vertex(m_y * vector.GetZ() - m_z * vector.GetY(),
				  m_z * vector.GetX() - m_x * vector.GetZ(),
				  m_x * vector.GetY() - m_y * vector.GetX(), 0.0f);
}

double Vertex::Length() const
{
	return sqrt(pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2));
}

const Vertex Vertex::operator - () const
{
	return Vertex(-m_x, -m_y, -m_z, -m_w);
}

const Vertex Vertex::operator + (const Vertex & vertex) const
{
	return Vertex(m_x + vertex.GetX(), m_y + vertex.GetY(), m_z + vertex.GetZ(), (m_w + vertex.GetW()));
}
 
const Vertex Vertex::operator - (const Vertex & vertex) const
{
	return Vertex(m_x - vertex.GetX(), m_y - vertex.GetY(), m_z - vertex.GetZ(), m_w - vertex.GetW());
}

const Vertex Vertex::operator * (double doubleNumber) const
{
	return Vertex(m_x * doubleNumber, m_y * doubleNumber, m_z * doubleNumber, m_w);
}

const Vertex Vertex::operator / (double doubleNumber) const
{
	return Vertex(m_x / doubleNumber, m_y / doubleNumber, m_z / doubleNumber, m_w);
}


void Vertex::Print() const
{
	printf("%f	%f	%f	%f\n", m_x, m_y, m_z, m_w);
}

Vertex::~Vertex()
{
	printf("Delete Vertex\n");
}

















/*
*-----------------------
*	class Triangle function
*-----------------------
*/


Triangle::Triangle()
{
	m_vertex0 = Vertex(0.0f, 0.0f, 1.0f, 1.0f);
	m_vertex1 = Vertex(0.0f, 0.0f, 1.0f, 1.0f);
	m_vertex2 = Vertex(0.0f, 0.0f, 1.0f, 1.0f);
}

Triangle::Triangle(const Triangle & triangle)
{
	m_vertex0 = triangle.GetVertex(0);
	m_vertex1 = triangle.GetVertex(1);
	m_vertex2 = triangle.GetVertex(2);
}

Triangle::Triangle(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2)
{
	m_vertex0 = vertex0;
	m_vertex1 = vertex1;
	m_vertex2 = vertex2;
}

void Triangle::SetVertex(int index, const Vertex & vertex)
{	
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			m_vertex0 = vertex;
			break;
		case 1:
			m_vertex1 = vertex;
			break;
		case 2:
			m_vertex2 = vertex;
			break;
		default:
			break;
	}
}

Vertex & Triangle::GetVertex(int index)
{
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			return m_vertex0;
			break;
		case 1:
			return m_vertex1;
			break;
		case 2:
			return m_vertex2;
			break;
		default:
			break;
	}
	return m_vertex0;
}

const Vertex & Triangle::GetVertex(int index) const
{
	assert(index >= 0 && index < 3);

	switch(index)
	{
		case 0:
			return m_vertex0;
			break;
		case 1:
			return m_vertex1;
			break;
		case 2:
			return m_vertex2;
			break;
		default:
			break;
	}
	return m_vertex0;
}


Triangle Triangle::Project() const
{
	return Triangle(m_vertex0.Project(), m_vertex1.Project(), m_vertex2.Project());
}
 
void Triangle::Render() const
{
	Triangle temp = Project();
	
	Camera camera = GetBackSystem().GetCamera();
	Vector DifferenceVector0 = Vector(m_vertex1 - m_vertex0);
	Vector DifferenceVector1 = Vector(m_vertex2 - m_vertex0);
	
	Vector CrossVector = DifferenceVector0.Cross(DifferenceVector1);
		
	Vertex middleVertex = (m_vertex0 + m_vertex1 + m_vertex2) / 3.0f;

	Vector DifferenceToCamera = middleVertex - camera.GetPosition();
	
	if(!(DifferenceToCamera.Dot(CrossVector) < 0.0f)) return ;

	GetBackSystem().Line3D(temp.GetVertex(0), temp.GetVertex(1));
	GetBackSystem().Line3D(temp.GetVertex(1), temp.GetVertex(2));
	GetBackSystem().Line3D(temp.GetVertex(2), temp.GetVertex(0));
}

Triangle::~Triangle()
{
	printf("Delete Triangle\n");
}

/*
*-----------------------
*	class Matrix function
*-----------------------
*/

Matrix::Matrix()
{
	*this = Matrix(	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix::Matrix(	double f00, double f01, double f02,double f03,
				double f10, double f11, double f12,double f13,
				double f20, double f21, double f22,double f23,
				double f30, double f31, double f32,double f33)
{
	m_f00 = f00; m_f01 = f01; m_f02 = f02; m_f03 = f03;
	m_f10 = f10; m_f11 = f11; m_f12 = f12; m_f13 = f13;
	m_f20 = f20; m_f21 = f21; m_f22 = f22; m_f23 = f23;
	m_f30 = f30; m_f31 = f31; m_f32 = f32; m_f33 = f33;
}

Matrix::Matrix(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2, const Vertex & vertex3)
{
	SetAll(vertex0, vertex1, vertex2, vertex3);
}

Matrix::Matrix(const Matrix & pSrcMatrix)
{
	*this = Matrix(	pSrcMatrix(0,0), pSrcMatrix(0,1), pSrcMatrix(0,2), pSrcMatrix(0,3),
					pSrcMatrix(1,0), pSrcMatrix(1,1), pSrcMatrix(1,2), pSrcMatrix(1,3),
					pSrcMatrix(2,0), pSrcMatrix(2,1), pSrcMatrix(2,2), pSrcMatrix(2,3),
					pSrcMatrix(3,0), pSrcMatrix(3,1), pSrcMatrix(3,2), pSrcMatrix(3,3));
}



void Matrix::SetAll(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2, const Vertex & vertex3)
{
	*this = Matrix(	vertex0.GetX(), vertex0.GetY(), vertex0.GetZ(), vertex0.GetW(), 
					vertex1.GetX(), vertex1.GetY(), vertex1.GetZ(), vertex1.GetW(), 
					vertex2.GetX(), vertex2.GetY(), vertex2.GetZ(), vertex2.GetW(), 
					vertex3.GetX(), vertex3.GetY(), vertex3.GetZ(), vertex3.GetW());
}


void Matrix::Translation(double tx, double ty, double tz)
{
	*this = Matrix(	1, 0, 0, tx,
					0, 1, 0, ty,
					0, 0, 1, tz,
					0, 0, 0, 1);
}



void Matrix::RotationX(double angle)
{
	double sn,cs;
	sn=(double)sin(angle);
	cs=(double)cos(angle);

	*this = Matrix(	1,  0,  0, 0,
					0, cs,-sn, 0,
					0, sn, cs, 0,
					0,  0,  0, 1);
}


void Matrix::RotationY(double angle)
{
	double sn,cs;
	sn=(double)sin(angle);
	cs=(double)cos(angle);

	*this = Matrix(cs,  0, sn,  0,
					0,  1,  0,  0,
				  -sn,  0, cs,  0,
					0,  0,  0,  1);
}


void Matrix::RotationZ(double angle)
{
	double sn,cs;
	sn=(double)sin(angle);
	cs=(double)cos(angle);

	*this = Matrix(	cs,-sn,  0,  0,
					sn, cs,  0,  0,
					0 ,  0,  1,  0,
					0 ,  0,  0,  1);
}

Matrix Matrix::Transpose() const
{
	return Matrix(	m_f00, m_f10, m_f20, m_f30,
					m_f01, m_f11, m_f21, m_f31,
					m_f02, m_f12, m_f22, m_f32,
					m_f03, m_f13, m_f23, m_f33);
}

double & Matrix::operator () (int i, int j)
{
	assert((i>=0 && i < 4) && (j>=0 && j < 4));

	double * temp[4][4] = {	&m_f00, &m_f01, &m_f02, &m_f03, 
							&m_f10, &m_f11, &m_f12, &m_f13,
							&m_f20, &m_f21, &m_f22, &m_f23, 
							&m_f30, &m_f31, &m_f32, &m_f33};
	
	return *temp[i][j];
}

const double & Matrix::operator () (int i, int j) const
{
	assert((i>=0 && i < 4) && (j>=0 && j < 4));

	const double * temp[4][4] = {&m_f00, &m_f01, &m_f02, &m_f03, 
								 &m_f10, &m_f11, &m_f12, &m_f13,
								 &m_f20, &m_f21, &m_f22, &m_f23, 
								 &m_f30, &m_f31, &m_f32, &m_f33};
	return *temp[i][j];
}

const Matrix Matrix::operator + (const Matrix & matrix) const
{
	Matrix temp;
	int i = 0;
	int j = 0;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			temp(i, j) = (*this)(i, j) + matrix(i, j);
		}
	}
	return temp;
}

const Matrix Matrix::operator * (const Matrix &  matrix) const
{
	int i = 0;
	int j = 0;
	int k = 0;
	Matrix temp = Matrix(0.0f, 0.0f, 0.0f, 0.0f, 
						 0.0f, 0.0f, 0.0f, 0.0f, 
						 0.0f, 0.0f, 0.0f, 0.0f, 
						 0.0f, 0.0f, 0.0f, 0.0f);

 	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			for(k = 0; k < 4; k++)
			{
				temp(i, j) = temp(i, j) + (*this)(i, k) * matrix(k, j);
			}
		}
	}
	return temp;
}


const Vertex Matrix::operator * (const Vertex & vertex) const
{	
	return Vertex((*this)(0,0) * vertex.GetX() + (*this)(0,1) * vertex.GetY() + (*this)(0,2) * vertex.GetZ() + (*this)(0,3) * vertex.GetW(),
				  (*this)(1,0) * vertex.GetX() + (*this)(1,1) * vertex.GetY() + (*this)(1,2) * vertex.GetZ() + (*this)(1,3) * vertex.GetW(),
				  (*this)(2,0) * vertex.GetX() + (*this)(2,1) * vertex.GetY() + (*this)(2,2) * vertex.GetZ() + (*this)(2,3) * vertex.GetW(),
				  (*this)(3,0) * vertex.GetX() + (*this)(3,1) * vertex.GetY() + (*this)(3,2) * vertex.GetZ() + (*this)(3,3) * vertex.GetW());
}

const VecVertex Matrix::operator * (const VecVertex & vecVertex) const
{
	return VecVertex((*this) * vecVertex.GetPosition(), (*this) * vecVertex.GetVector(), vecVertex.GetTx(), vecVertex.GetTy());
}

const Triangle Matrix::operator * (const Triangle & triangle) const
{
	return Triangle((*this) * triangle.GetVertex(0), (*this) * triangle.GetVertex(1), (*this) * triangle.GetVertex(2));
}

const VecTriangle Matrix::operator * (const VecTriangle & vecTriangle) const
{
	return VecTriangle((*this) * vecTriangle.GetVecVertex(0), (*this) * vecTriangle.GetVecVertex(1), (*this) * vecTriangle.GetVecVertex(2));
}

const Camera Matrix::operator * (const Camera & camera) const
{
	Camera temp;
	temp.SetPosition((*this) * camera.GetPosition());
	temp.SetTarget((*this) * camera.GetTarget());
	temp.SetUp((*this) * camera.GetUp());
	return temp;
}

const Matrix Matrix::operator - () const
{
	return Matrix(-m_f00, -m_f01, -m_f02, -m_f03,
				  -m_f10, -m_f11, -m_f12, -m_f13,
				  -m_f20, -m_f21, -m_f22, -m_f23,
				  -m_f30, -m_f31, -m_f32, -m_f33);
}

void Matrix::Print(char * buf) const
{
//	wsprintf(buf,"%d %d %d %d\n%d %d %d %d\n%d %d %d %d\n%d %d %d %d\n", m_f00, m_f01, m_f02, m_f03 ,m_f10, m_f11, m_f12, m_f13,m_f20, m_f21, m_f22, m_f23,m_f30, m_f31, m_f32, m_f33);
}

Matrix::~Matrix()
{
	printf("Delete Matrix\n");
}






/*
*-----------------------
*	class Camera function
*-----------------------
*/
Camera::Camera()
{
	m_position = Vertex(0.0f, 0.0f, 0.0f, 1.0f);
	m_target = Vertex(0.0f, 0.0f, 0.0f, 1.0f);
	m_up = Vertex(0.0f, 0.0f, 0.0f, 1.0f);
}

Camera::Camera(const Camera & camera)
{
	m_position = camera.GetPosition();
	m_target = camera.GetTarget();
	m_up = camera.GetUp();
	Identity();
}

Camera::Camera(const Vertex & position, const Vector & target, const Vector & up)
{
	m_position = position;
	m_target = target;
	m_up = up;

	Identity();
}


void Camera::Identity()
{
	/*
	m_direction = m_direction.Identity();
	m_rightVector = (m_upVector.Cross(m_direction)).Identity();
	m_upVector = (m_direction.Cross(m_rightVector)).Identity();
	*/
}

void Camera::SetPosition(const Vertex & position) 
{
	m_position = position;
}

void Camera::SetTarget(const Vector & target) 
{
	m_target = target;
}

void Camera::SetUp(const Vector & up) 
{
	m_up = up;
}


Vertex & Camera::GetPosition()
{
	return m_position;
}

Vertex & Camera::GetTarget()
{
	return m_target;
}

Vertex & Camera::GetUp()
{
	return m_up;
}

Vector Camera::GetDirectionVector() const
{
	return m_target - m_position;
}

Vector Camera::GetRightVector() const
{
	return (m_up - m_position).Cross(m_target - m_position);
} 

Vector Camera::GetUpVector() const
{
	Vector temp0(m_target - m_position);
	Vector temp1(m_up - m_position);
	
	return temp0.Cross(temp1.Cross(temp0));
}

const Vertex & Camera::GetPosition() const
{
	return m_position;
}

const Vertex & Camera::GetTarget() const
{
	return m_target;
}

const Vertex & Camera::GetUp() const
{
	return m_up;
}

Matrix Camera::GetVectorCoordMatrix() const
{
	Matrix temp;
	temp.SetAll(GetRightVector().Identity(), GetUpVector().Identity(), GetDirectionVector().Identity(), Vertex(0.0f, 0.0f, 0.0f, 1.0f));
	return temp;
}

Matrix Camera::GetCameraMatrix() const
{
	Matrix temp;

	temp.Translation(-m_position.GetX(), -m_position.GetY(), -m_position.GetZ());
	return (GetVectorCoordMatrix() * temp);
}


/*
*-----------------------
*	global function
*-----------------------
*/



void SetBackSystem(const BackSystem & backSystem)
{
	g_backSystem = backSystem;
}

BackSystem & GetBackSystem()
{
	return g_backSystem;
}




Cube::Cube(){}
Cube::Cube(double edgeLength)
{
	VecVertex vertex[8];
	m_edgeLength = edgeLength;
	double HalfOfLength = m_edgeLength / 2;
	vertex[0].SetAll(Vertex( HalfOfLength,  HalfOfLength , HalfOfLength, 1.0f), Vector( 1.0f,  1.0f, 1.0f, 0.0f), 0.0f, 0.0f);
	vertex[1].SetAll(Vertex(-HalfOfLength,  HalfOfLength , HalfOfLength, 1.0f), Vector(-1.0f,  1.0f, 1.0f, 0.0f), 1.0f, 0.0f);
	vertex[2].SetAll(Vertex( HalfOfLength, -HalfOfLength , HalfOfLength, 1.0f), Vector( 1.0f, -1.0f, 1.0f, 0.0f), 0.0f, 1.0f);
	vertex[3].SetAll(Vertex(-HalfOfLength, -HalfOfLength , HalfOfLength, 1.0f), Vector(-1.0f, -1.0f, 1.0f, 0.0f), 1.0f, 1.0f);

	vertex[4].SetAll(Vertex( HalfOfLength,  HalfOfLength , -HalfOfLength, 1.0f), Vector( 1.0f,  1.0f, -1.0f, 0.0f), 1.0f, 1.0f);
	vertex[5].SetAll(Vertex(-HalfOfLength,  HalfOfLength , -HalfOfLength, 1.0f), Vector(-1.0f,  1.0f, -1.0f, 0.0f), 0.0f, 1.0f);
	vertex[6].SetAll(Vertex( HalfOfLength, -HalfOfLength , -HalfOfLength, 1.0f), Vector( 1.0f, -1.0f, -1.0f, 0.0f), 1.0f, 0.0f);
	vertex[7].SetAll(Vertex(-HalfOfLength, -HalfOfLength , -HalfOfLength, 1.0f), Vector(-1.0f, -1.0f, -1.0f, 0.0f), 0.0f, 0.0f);

	m_vecTriangle[0] = VecTriangle(vertex[0], vertex[1], vertex[2]);
	m_vecTriangle[1] = VecTriangle(vertex[3], vertex[2], vertex[1]);

	m_vecTriangle[2] = VecTriangle(vertex[4], vertex[6], vertex[5]);
	m_vecTriangle[3] = VecTriangle(vertex[5], vertex[6], vertex[7]);
	
	m_vecTriangle[4] = VecTriangle(vertex[0], vertex[2], vertex[4]);
	m_vecTriangle[5] = VecTriangle(vertex[6], vertex[4], vertex[2]);

	m_vecTriangle[6] = VecTriangle(vertex[1], vertex[5], vertex[3]);
	m_vecTriangle[7] = VecTriangle(vertex[7], vertex[3], vertex[5]);
 
	m_vecTriangle[8] = VecTriangle(vertex[0], vertex[4], vertex[1]);
	m_vecTriangle[9] = VecTriangle(vertex[5], vertex[1], vertex[4]);

	m_vecTriangle[10] = VecTriangle(vertex[2], vertex[3], vertex[6]);
	m_vecTriangle[11] = VecTriangle(vertex[7], vertex[6], vertex[3]);

	for(int i = 0; i < 12; i+=2)
	{
		m_vecTriangle[i].GetVecVertex(0).SetTxy(0.0f, 0.0f);
		m_vecTriangle[i].GetVecVertex(1).SetTxy(1.0f, 0.0f);
		m_vecTriangle[i].GetVecVertex(2).SetTxy(0.0f, 1.0f);

		m_vecTriangle[i+1].GetVecVertex(0).SetTxy(1.0f, 1.0f);
		m_vecTriangle[i+1].GetVecVertex(1).SetTxy(0.0f, 1.0f);
		m_vecTriangle[i+1].GetVecVertex(2).SetTxy(1.0f, 0.0f);
	}
}

void Cube::VectorUniform()
{
	for(int i = 0; i < 12; i++)
	{
		m_vecTriangle[i] = m_vecTriangle[i].VectorUniform();
	}
}

void Cube::Render()
{ 
	for(int i = 0; i < 12; i++)
	{
		m_vecTriangle[i].Render();
	}
}




Sphere::Sphere(){}

Sphere::Sphere(double radius, int vertexNum)
{
	int i = 0;
	int j = 0;
	m_radius = radius;
	m_vertexNum = (vertexNum / 2) * 2;
	m_pVecTriangle = new VecTriangle[m_vertexNum * m_vertexNum];

	Matrix rx;
	Matrix ry;

	double angle = 2.0f * PI / m_vertexNum;

	rx.RotationX(angle);
	ry.RotationY(angle);
	
	double dtx = 1.0f / (m_vertexNum);
	double dty = 1.0f / (m_vertexNum / 2);
	VecVertex v0 = VecVertex(Vertex(0.0f, m_radius, 0.0f, 1.0f), Vector(0.0f, m_radius, 0.0f, 0.0f), 1.0f, 0.0f);
	VecVertex v1;
	VecVertex v2;
	VecVertex v3;
	for(i = 0; i < m_vertexNum / 2; i++)
	{
		v1 = rx * v0;
		v2 = ry * v0;
		v3 = ry * v1;
		
		v1.SetTx(v0.GetTx());
		v1.SetTy(0.5 + (v1.GetPosition().GetY() > 0 ? -0.5f : 0.5f) * sqrt(1 - pow(v1.GetVector().Identity().Dot(Vector(0.0f, 0.0f, 1.0f, 0.0f)), 2)));
		v2.SetTx(v0.GetTx() - dtx);
		v2.SetTy(v0.GetTy());
		v3.SetTx(v2.GetTx());
		v3.SetTy(v1.GetTy());

		VecVertex temp0 = v0;
		VecVertex temp1 = v1;
		VecVertex temp2 = v2;
		VecVertex temp3 = v3;

		for(j = 0; j < m_vertexNum; j++)
		{
			m_pVecTriangle[(2 * i) * m_vertexNum + (2 * j)]	   = VecTriangle(temp0, temp1, temp2);
			m_pVecTriangle[(2 * i) * m_vertexNum + (2 * j) +1] = VecTriangle(temp3, temp2, temp1);
	
			temp0 = ry * temp0;
			temp1 = ry * temp1;
			temp2 = ry * temp2;
			temp3 = ry * temp3;
			
			temp0.SetTx(temp0.GetTx() - dtx);
			temp1.SetTx(temp1.GetTx() - dtx);
			temp2.SetTx(temp2.GetTx() - dtx);
			temp3.SetTx(temp3.GetTx() - dtx);
		}
		v0 = rx * v0;
		v0.SetTy(0.5 + (v0.GetPosition().GetY() > 0 ? -0.5f : 0.5f) * sqrt(1 - pow(v0.GetVector().Identity().Dot(Vector(0.0f, 0.0f, 1.0f, 0.0f)), 2)));
		v0.SetTx(1.0f);
	}
}

void Sphere::VectorUniform()
{
	for(int i = 0; i < m_vertexNum * m_vertexNum; i++)
	{
		m_pVecTriangle[i] = m_pVecTriangle[i].VectorUniform();
	}
}

void Sphere::Render()
{
	for(int i = 0; i < m_vertexNum * m_vertexNum; i++)
	{
		m_pVecTriangle[i].Render();
	}
}

Sphere::~Sphere()
{
	delete [] m_pVecTriangle;
}



Column::Column(){}
Column::Column(double radius, double high, int vertexNum)
{
	m_radius = radius;
	m_high = high;
	m_vertexNum = vertexNum;
	double halfOfHigh = high / 2;

	Matrix r;
	r.RotationY((2 * PI) / m_vertexNum);
	
	VecVertex v[10];

	v[0] = VecVertex(Vertex(0.0f,  halfOfHigh, m_radius, 1.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
	v[1] = VecVertex(Vertex(0.0f, -halfOfHigh, m_radius, 1.0f), Vector(0.0f, -1.0f, 0.0f, 0.0f), 0.0f, 0.0f);;
	v[2] = r * v[0];
	v[3] = r * v[1];
	v[4] = VecVertex(Vertex(0.0f,  halfOfHigh, m_radius, 1.0f), Vector(0.0f, 0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
	v[5] = VecVertex(Vertex(0.0f, -halfOfHigh, m_radius, 1.0f), Vector(0.0f, 0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
	v[6] = r * v[4];
	v[7] = r * v[5];
	v[8] = VecVertex(Vertex(0.0f,  halfOfHigh, 0.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
	v[9] = VecVertex(Vertex(0.0f, -halfOfHigh, 0.0f, 1.0f), Vector(0.0f, -1.0f, 0.0f, 0.0f), 0.0f, 0.0f);

	m_pVecTriangle = new VecTriangle[4 * m_vertexNum];

	for(int i = 0; i < m_vertexNum; i++)
	{
		m_pVecTriangle[4 * i]		= VecTriangle(v[8], v[0], v[2]);
		m_pVecTriangle[4 * i + 1]	= VecTriangle(v[9], v[3], v[1]);
		m_pVecTriangle[4 * i + 2]   = VecTriangle(v[4], v[5], v[6]);
		m_pVecTriangle[4 * i + 3]   = VecTriangle(v[7], v[6], v[5]);

		for(int j = 0; j < 8; j++)
		{
			v[j] = r * v[j];
		}
	}
}   

void Column::VectorUniform()
{
	for(int i = 0; i < 4 * m_vertexNum; i++)
	{
		m_pVecTriangle[i] = m_pVecTriangle[i].VectorUniform();
	}
}

void Column::Render()
{
	for(int i = 0; i < 4 * m_vertexNum; i++)
	{
		m_pVecTriangle[i].Render();
	}
}

Column::~Column()
{
	delete [] m_pVecTriangle;
}


Cirque::Cirque()
{}

Cirque::Cirque(double insideRadius, double outsideRadius, int circleNum, int vertexNum)
{
	int i = 0;
	
	m_insideRadius = insideRadius;
	m_outsideRadius = outsideRadius;
	m_circleNum = circleNum;
	m_vertexNum = (vertexNum / 2) * 2;
	m_pVecTriangle = new VecTriangle[2 * m_vertexNum * m_circleNum];

	VecVertex * pCircleVertex = new VecVertex[2 * m_vertexNum];

	pCircleVertex[0] = VecVertex(Vertex(0.0f, 0.0f, (m_outsideRadius - m_insideRadius) / 2, 1.0f), Vector(0.0f, 0.0f, 1.0f, 0.0f), 0.0f, 0.0f);

	Matrix r;
	r.RotationX((2 * PI) / m_vertexNum);

	for(i = 1; i < m_vertexNum; i++)
	{
		pCircleVertex[i] = r * pCircleVertex[i - 1];
	}
	
	r.Translation(0.0f, 0.0f, (m_outsideRadius + m_insideRadius) / 2);

	for(i = 0; i < m_vertexNum; i++)
	{
		pCircleVertex[i] = r * pCircleVertex[i];
	}

	r.RotationY((2 * PI) / m_circleNum);
	for(i = 0; i < m_vertexNum; i++)
	{
		pCircleVertex[i + m_vertexNum] = r * pCircleVertex[i];
	}

	for(i = 0; i < m_circleNum; i++)
	{
		for(int j = 0; j < m_vertexNum; j++)
		{
			m_pVecTriangle[i * 2 * m_vertexNum + 2 * j]		= VecTriangle(pCircleVertex[j], pCircleVertex[(j + 1) % m_vertexNum], pCircleVertex[j + m_vertexNum]);
			m_pVecTriangle[i * 2 * m_vertexNum + 2 * j + 1] = VecTriangle(pCircleVertex[(j + 1) % m_vertexNum + m_vertexNum ], pCircleVertex[j + m_vertexNum], pCircleVertex[(j + 1) % m_vertexNum]);
		}

		for(int k = 0; k < 2 * m_vertexNum; k++)
		{
			pCircleVertex[k] = r * pCircleVertex[k];
		}
	}

	delete [] pCircleVertex;
}

void Cirque::VectorUniform()
{
	for(int i = 0; i < 2 * m_vertexNum * m_circleNum; i++)
	{
		m_pVecTriangle[i] = m_pVecTriangle[i].VectorUniform();
	}
}

void Cirque::Render()
{
	for(int i = 0; i < 2 * m_vertexNum * m_circleNum; i++)
	{
		m_pVecTriangle[i].Render();
	}
}

Cirque::~Cirque()
{
	delete [] m_pVecTriangle;
}






Space::Space()
{

}

Space::Space(const Vector & vector, const Vertex & vertex)
{
	m_vector = vector;
	m_vertex = vertex;
}

Matrix Space::Shadow(const Vector & vector) const
{
	Vector tv0 = vector.Identity();
	Vector tv1 = m_vector.Identity();

	double x[3];
	double y[3];
	double z[3];

	x[0] = tv0.GetX();
	y[0] = tv0.GetY();
	z[0] = tv0.GetZ();

	x[1] = tv1.GetX();
	y[1] = tv1.GetY();
	z[1] = tv1.GetZ();

	x[2] = m_vertex.GetX();
	y[2] = m_vertex.GetY();
	z[2] = m_vertex.GetZ();

	double c1 = x[0] * x[1] + y[0] * y[1] + z[0] * z[1];
	double c2 = x[1] * x[2] + y[1] * y[2] + z[1] * z[2];
	double c = c2 / c1;
	Matrix temp(1 - ((x[0] * x[1]) / c1),	  -(x[0] * y[1]) / c1 ,		-(x[0] * z[1]) / c1 , x[0] * c,
					-(y[0] * x[1]) / c1 , 1 - ((y[0] * y[1]) / c1),		-(y[0] * z[1]) / c1 , y[0] * c,
					-(z[0] * x[1]) / c1 ,	  -(z[0] * y[1]) / c1 ,	1 - ((z[0] * z[1]) / c1), z[0] * c,
					0.0f, 0.0f, 0.0f, 1.0f);

	return temp;
}

Matrix Space::Reflect() const
{
	double x[2];
	double y[2];
	double z[2];

	Vector tv0 = m_vector.Identity();

	x[0] = tv0.GetX();
	y[0] = tv0.GetY();
	z[0] = tv0.GetZ();
	
	x[1] = m_vertex.GetX();
	y[1] = m_vertex.GetY();
	z[1] = m_vertex.GetZ();

	double c1 = (x[0] * x[0] + y[0] * y[0] + z[0] * z[0]) / 2;
	double c2 = x[0] * x[1] + y[0] * y[1] + z[0] * z[1];
	Matrix temp( (1 - x[0] * x[0] / c1),		 -(x[0] * y[0] / c1),		  -(x[0] * z[0] / c1), x[0] * c2 / c1,
					-(y[0] * x[0] / c1),	  (1 - y[0] * y[0] / c1),		  -(y[0] * z[0] / c1), y[0] * c2 / c1,
					-(z[0] * x[0] / c1),		 -(z[0] * y[0] / c1),	   (1 - z[0] * z[0] / c1), z[0] * c2 / c1,
				0.0f, 0.0f, 0.0f, 1.0f);
	return temp;
}







Surface::Surface()
{}

Surface::Surface(const Vertex & vertex0, const Vertex & vertex1, const Vertex & vertex2)
{
	Vector middleVertex = (vertex1 + vertex2) / 2;
	Vertex vertex3 = (middleVertex * 2) - vertex0;

	Vector t;
	VecVertex vecVertex0(vertex0, t, 0.0f, 0.0f);
	VecVertex vecVertex1(vertex1, t, 1.0f, 0.0f);
	VecVertex vecVertex2(vertex2, t, 0.0f, 1.0f);
	VecVertex vecVertex3(vertex3, t, 1.0f, 1.0f);

	m_vecTriangle0 = VecTriangle(vecVertex0, vecVertex1, vecVertex2);
	m_vecTriangle1 = VecTriangle(vecVertex3, vecVertex2, vecVertex1);

	m_vecTriangle0 = m_vecTriangle0.VectorUniform();
	m_vecTriangle1 = m_vecTriangle1.VectorUniform();
}

void Surface::Render()
{
	m_vecTriangle0.Render();
	m_vecTriangle1.Render();
}
