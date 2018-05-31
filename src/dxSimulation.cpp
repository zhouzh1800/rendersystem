// dxSimulation.cpp : Defines the entry point for the application.
//
//	program:	directX simulation
//	author:		zhou
//	date:		2007
//
 
#include <malloc.h> 
  
#include "stdafx.h"
#include "myDx.h"
  
Sphere	sphere(70.0f, 30);
Cube	cube(100.0f);
Column	column(80.0f, 100.0f, 20);
Cirque	cirque(50.0f, 80.0f, 20, 20);
  
#define HIGH	-70.0f
#define WIDTH	300.0f
  
Surface terra(Vertex(-WIDTH, HIGH, WIDTH, 1.0f), Vertex(WIDTH, HIGH, WIDTH, 1.0f), Vertex(-WIDTH, HIGH, -WIDTH, 1.0f));

Space space(Vector(0.0, 1.0f, 0.0f, 0.0f), Vertex(0.0f, HIGH, 0.0f, 1.0f));
 
Matrix shadow;
Matrix reflection;

#define dis	120.0f
Matrix sphereMatrix(1.0f, 0.0f, 0.0f, dis,
					0.0f, 1.0f, 0.0f, 0,
					0.0f, 0.0f, 1.0f, dis,
					0.0f, 0.0f, 0.0f, 1.0f);

Matrix cubeMatrix(1.0f, 0.0f, 0.0f, -dis,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, -dis,
					0.0f, 0.0f, 0.0f, 1.0f);

Matrix columnMatrix(1.0f, 0.0f, 0.0f, dis,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, -dis,
					0.0f, 0.0f, 0.0f, 1.0f);

Matrix cirqueMatrix(1.0f, 0.0f, 0.0f, -dis,
					0.0f, 1.0f, 0.0f, 0,
					0.0f, 0.0f, 1.0f, dis,
					0.0f, 0.0f, 0.0f, 1.0f);

void Display()
{

	// draw objects
	GetBackSystem().SetTextureEnable(true);
	GetBackSystem().SetTBufferEnable(true);

	GetBackSystem().GetTBuffer().SetWay(TEMPLATE_WRITE);
	GetBackSystem().GetTBuffer().SetValue(1);
	
	GetBackSystem().SetTexture("mirror.bmp");
	GetBackSystem().SetWorldMatrix(Matrix(1.0f, 0.0f, 0.0f, 0.0f,
										  0.0f, 1.0f, 0.0f, 0.0f,
	   									  0.0f, 0.0f, 1.0f, 0.0f,
	 									  0.0f, 0.0f, 0.0f, 1.0f));
	terra.Render();

	GetBackSystem().GetTBuffer().SetValue(0);
	GetBackSystem().SetTexture("earth.bmp");
	GetBackSystem().SetWorldMatrix(sphereMatrix);
	sphere.Render();
	GetBackSystem().SetTexture("cat.bmp");
	GetBackSystem().SetWorldMatrix(cubeMatrix);
	cube.Render();

	GetBackSystem().SetTextureEnable(false);

	GetBackSystem().GetMaterial().GetLightColor().SetAll(0xff, 0x0, 0xff);
	GetBackSystem().SetWorldMatrix(columnMatrix);
	column.Render();
	
	GetBackSystem().GetMaterial().GetLightColor().SetAll(0xff, 0xff, 0x0);
	GetBackSystem().SetWorldMatrix(cirqueMatrix);
	cirque.Render();

	GetBackSystem().SetTBufferEnable(false);
	

	


 

	// draw reflection for objects
	GetBackSystem().SetZBufferEnable(false);
	GetBackSystem().SetDeasil(false);
	GetBackSystem().SetTextureEnable(true);
	GetBackSystem().GetColorBuffer().SetBlendEnable(true);
	GetBackSystem().SetTBufferEnable(true);

	GetBackSystem().GetTBuffer().SetValue(1);
	GetBackSystem().GetTBuffer().SetWay(TEMPLATE_EQUA);
	
	GetBackSystem().GetColorBuffer().SetBlendSrc(LightColor(0x7f, 0x7f, 0x7f));
	GetBackSystem().GetColorBuffer().SetBlendDest(LightColor(0x7f, 0x7f, 0x7f));

	GetBackSystem().SetTexture("earth.bmp");
	GetBackSystem().SetWorldMatrix(reflection * sphereMatrix);
	sphere.Render();

	GetBackSystem().SetTexture("cat.bmp");
	GetBackSystem().SetWorldMatrix(reflection * cubeMatrix);
	cube.Render();
	 
	GetBackSystem().SetTextureEnable(false);


	GetBackSystem().GetMaterial().GetLightColor().SetAll(0xff, 0x0, 0xff);
	GetBackSystem().SetWorldMatrix(reflection * columnMatrix);
	column.Render();

	GetBackSystem().GetMaterial().GetLightColor().SetAll(0xff, 0xff, 0x0);
	GetBackSystem().SetWorldMatrix(reflection * cirqueMatrix);
	cirque.Render();

	GetBackSystem().SetTBufferEnable(false);
	GetBackSystem().GetColorBuffer().SetBlendEnable(false);
	GetBackSystem().SetDeasil(true);
	GetBackSystem().SetZBufferEnable(true);
	

	 
	

	

	
	// draw shadow for objects
	GetBackSystem().SetZBufferEnable(false);
	GetBackSystem().GetColorBuffer().SetBlendEnable(true);
	GetBackSystem().SetTBufferEnable(true);

	GetBackSystem().GetTBuffer().SetValue(1);
	GetBackSystem().GetTBuffer().SetWay(TEMPLATE_EQUA);
	GetBackSystem().GetTBuffer().SetOperatroValue(TBuffer_INC);

	GetBackSystem().GetColorBuffer().SetBlendSrc(LightColor(0x7f, 0x7f, 0x7f));
	GetBackSystem().GetColorBuffer().SetBlendDest(LightColor(0x7f, 0x7f, 0x7f));

	GetBackSystem().GetMaterial().GetLightColor().SetAll(0, 0, 0);
	
	GetBackSystem().SetWorldMatrix(shadow * sphereMatrix);
	sphere.Render();
	
	GetBackSystem().SetWorldMatrix(shadow * cubeMatrix);
	cube.Render();
 
	GetBackSystem().SetWorldMatrix(shadow * columnMatrix);
	column.Render();
	
	GetBackSystem().SetWorldMatrix(shadow * cirqueMatrix);
	cirque.Render();
	
	GetBackSystem().GetTBuffer().SetOperatroValue(0);
	GetBackSystem().SetZBufferEnable(true);
	GetBackSystem().GetColorBuffer().SetBlendEnable(false);
	GetBackSystem().SetTBufferEnable(false);

	
return;
} 

void Present()
{
	GetBackSystem().GetColorBuffer().Clear(0xff0000);
	GetBackSystem().GetZBuffer().Clear(1.0f);
	GetBackSystem().GetTBuffer().Clear(0);
 
	Display();

	GetBackSystem().GetColorBuffer().Present();
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Matrix	rotation;
	Matrix	coordinate;
	Matrix	translate1;
	Matrix	translate0;
	Matrix	translate2;

	Camera & camera = GetBackSystem().GetCamera();
	
	Vector temp0 = camera.GetRightVector().Identity() * 10;
	Vector temp1 = camera.GetDirectionVector().Identity() * 10;
	Vector temp2 = camera.GetUpVector().Identity() * 10;

	switch (message)
	{ 
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					{
						DestroyWindow(hWnd);
					}
					break;
				case 'A':	
					// left translation
					translate0.Translation(-temp0.GetX(), -temp0.GetY(), -temp0.GetZ());
					GetBackSystem().SetCamera(translate0 * camera);
					break;
 
				case 'D':
					// right translation
					translate0.Translation(temp0.GetX(), temp0.GetY(), temp0.GetZ());
					GetBackSystem().SetCamera(translate0 * camera);
					break;

				case 'W':
					// forward translation
					translate1.Translation(temp1.GetX(), temp1.GetY(), temp1.GetZ());
					GetBackSystem().SetCamera(translate1 * camera);
					break;

				case 'S':
					//backward translation
					translate1.Translation(-temp1.GetX(), -temp1.GetY(), -temp1.GetZ());
					GetBackSystem().SetCamera(translate1 * camera);
					break;

				case 'R':
					// up translation
					translate2.Translation(temp2.GetX(), temp2.GetY(), temp2.GetZ());
					GetBackSystem().SetCamera(translate2 * camera);
					break;
 
				case 'F':
					// down translation
					translate2.Translation(-temp2.GetX(), -temp2.GetY(), -temp2.GetZ());
					GetBackSystem().SetCamera(translate2 * camera);
					break;
 
				case 'J':
					// left rotation
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationY(PI / 20);
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;
  
				case 'L':
					// right rotation
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationY(-PI / 20);
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;
     
				case 'I':
					// look down
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationX(0.07f);
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;

  				case 'K':
					// look up
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationX(-0.07f);	
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;

				case 'U':
					// left roll
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationZ(0.07f);		
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;
       
				case 'O':
					// right roll
					coordinate = camera.GetVectorCoordMatrix();
					rotation.RotationZ(-0.07f);
					GetBackSystem().SetCamera((coordinate.Transpose() * rotation * coordinate) * camera);
					break;
 
				case VK_RIGHT:
					// object rotation
					rotation.RotationY(PI / 20);
					sphereMatrix = sphereMatrix * rotation;
					cubeMatrix = cubeMatrix * rotation;
					columnMatrix = columnMatrix * rotation;
					cirqueMatrix = cirqueMatrix * rotation;
					break;

				case VK_LEFT:
					//object rotation
					rotation.RotationY(-PI / 20);
					sphereMatrix = sphereMatrix * rotation;
					cubeMatrix = cubeMatrix * rotation;
					columnMatrix = columnMatrix * rotation;
					cirqueMatrix = cirqueMatrix * rotation;
					break;

				case VK_UP:
					break;
				default:
					break;
			}
			Present();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}   
   return 0;
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	
	MSG			msg;
	WNDCLASS	wcex;
	HWND		hWnd;
  
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "hello";

	RegisterClass(&wcex);
  
	hWnd = CreateWindow("hello","hello world", WS_OVERLAPPEDWINDOW,
		10, 10, 800, 600, NULL, NULL, hInstance, NULL);
 
	if (!hWnd)
	{
		return FALSE;
	}
  
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
    











	// set wnd that backSystem operate for
	GetBackSystem().SetHWnd(hWnd);
	
	//Enable Z Buffer
	GetBackSystem().SetZBufferEnable(true);
	
	//disable T Buffer
	GetBackSystem().SetTBufferEnable(false);
 
	// set Draw Style so use phong render
	GetBackSystem().GetColorBuffer().SetDrawStyle(false);

	// set Light
	GetBackSystem().SetLight(Light(EnvironmentLight(LightColor(50, 50, 50)),
								   DirectionLight(LightColor(30, 20, 100), Vector(-1.0f, 0.0f, 0.5f, 0.0f)), 
	 		  					   DotLight(LightColor(150, 150, 150), Vertex(0.0f, 150.0f, 0.0f, 1.0f), 0.5f, 0.002f, 0.0f)));
	
	// set camera
	GetBackSystem().SetCamera(Camera(Vertex(-220.0f, 200.0f, 300.0f, 1.0f),
			    					 Vector(0.0f, -200.0f, 0.0f, 1.0f),
 	  	   		  					 Vector(0.0f, 1.0f, 0.0f, 1.0f)));

	// set projection matrix
	GetBackSystem().SetProjection(3.141526f * 0.5f, 400 / 300.0f, 1.0f, 1000.0f);
 
	// set WorldMatrix
	GetBackSystem().SetWorldMatrix(Matrix(1.0f, 0.0f, 0.0f, 0.0f,
										  0.0f, 1.0f, 0.0f, 0.0f,
	   									  0.0f, 0.0f, 1.0f, 0.0f,
										  0.0f, 0.0f, 0.0f, 1.0f));
	
	// set viewport
	long winWidth = GetBackSystem().GetZBuffer().GetWidth();
	long winHeight = GetBackSystem().GetZBuffer().GetHeight();
	double rate = 0.05f;
	double oneSub2xRate = (1 - 2 * rate);
	GetBackSystem().SetViewport((long)(winWidth * rate), (long)(winHeight * rate), (long)(winWidth * oneSub2xRate), (long)(winHeight * oneSub2xRate), 0, 1);

	//enable these for another view
/*			
	cube.VectorUniform();
	sphere.VectorUniform();
	cirque.VectorUniform();
	column.VectorUniform();
*/	

	shadow = space.Shadow(Vector(1.0f, 1.0f, 1.0f, 0.0f));	// shadow matrix
	reflection = space.Reflect();								// reflection matrix
	
	Present();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	     
	return 0;
} 
 


