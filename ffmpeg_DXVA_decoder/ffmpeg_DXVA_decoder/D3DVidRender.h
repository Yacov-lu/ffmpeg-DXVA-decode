#pragma once
#include "stdafx.h"
#include "d3d9.h"
#include "d3dx9.h"

class CD3DVidRender
{
public:
	CD3DVidRender(void);
	~CD3DVidRender(void);
	
	void Cleanup();

	BOOL InitD3D_RGB32(HWND hwnd, int img_width, int img_height);

	BOOL InitD3D_YUV(HWND hwnd, int img_width, int img_height);

	BOOL Render_RGB32(unsigned char* pdata, int width, int height);

	BOOL Render_YUV(unsigned char * pdata, int img_width, int img_height);

	void calculate_display_rect(RECT *rect,int img_width, int img_height, int scr_width, int scr_height) ;

public:
	RECT m_rtViewport;
	D3DPRESENT_PARAMETERS d3dpp; 
	IDirect3D9 * m_pDirect3D9;  
	IDirect3DDevice9 * m_pDirect3DDevice;  
	IDirect3DSurface9 * m_pDirect3DSurfaceRender; 
	IDirect3DSurface9 * m_pBackBuffer;

	RECT m_rtFont;
	ID3DXFont* m_pD3DXFont;
	D3DXFONT_DESC m_font_desc;
};
