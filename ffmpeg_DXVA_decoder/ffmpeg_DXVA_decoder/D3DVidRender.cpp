#include "stdafx.h"
#include "D3DVidRender.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// 使用stdio.h和strsafe.h，可实现类似TRACE的功能
/*void LogTrace(LPCTSTR pszFormat, ...)  
{  
	va_list pArgs;  

	char szMessageBuffer[16380]={0};  
	va_start( pArgs, pszFormat );  
	_vsntprintf( szMessageBuffer, 16380, pszFormat, pArgs );  
	va_end( pArgs );  

	OutputDebugString(szMessageBuffer);  
}*/  

CD3DVidRender::CD3DVidRender(void)
{
	m_pDirect3D9 = NULL;  
	m_pDirect3DDevice = NULL;  
	m_pDirect3DSurfaceRender = NULL; 
	m_pBackBuffer = NULL;
}

CD3DVidRender::~CD3DVidRender(void)
{
}

void CD3DVidRender::Cleanup()  
{  
	if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	if(m_pDirect3DSurfaceRender) 
	{
		m_pDirect3DSurfaceRender->Release(); 
		m_pDirect3DSurfaceRender = NULL;
	}
	if(m_pDirect3DDevice)  
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = NULL;
	}
	if(m_pDirect3D9)  
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
	if (m_pD3DXFont)
	{
		m_pD3DXFont->Release() ;
		m_pD3DXFont = NULL ;
	}
} 

BOOL CD3DVidRender::InitD3D_RGB32(HWND hwnd, int img_width, int img_height)
{
	HRESULT lRet;

	if (m_pDirect3D9)
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);  
	if(m_pDirect3D9 == NULL) 
	{
		return FALSE;  
	}

	ZeroMemory(&d3dpp, sizeof(d3dpp)); 
	d3dpp.Windowed = TRUE;  
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;  
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; 

	D3DCAPS9 caps;
	DWORD BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	HRESULT hr = m_pDirect3D9->GetDeviceCaps ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ;
	if ( SUCCEEDED(hr) )
	{
		if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		{
			BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
		}
		else
		{
			BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
		}
	}

	if (m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = NULL;
	}
	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, BehaviorFlags, &d3dpp, &m_pDirect3DDevice);  
	if(FAILED(lRet))  
	{
		return FALSE;  
	} 

	if (m_pDirect3DSurfaceRender)
	{
		m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;
	}
	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(img_width, img_height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, 
&m_pDirect3DSurfaceRender, NULL);
	if(FAILED(lRet))  
	{
		return FALSE;  
	}

	//// create the font
	//ZeroMemory(&m_font_desc, sizeof(m_font_desc));

	//// set font descripter
	//strcpy(m_font_desc.FaceName, "楷体");
	//m_font_desc.Height = 32;

	//// Creates a font object indirectly for both a device and a font
	//D3DXCreateFontIndirect(m_pDirect3DDevice, &m_font_desc, &m_pD3DXFont);

	return TRUE;  
}

BOOL CD3DVidRender::InitD3D_YUV(HWND hwnd,int img_width, int img_height)
{
	HRESULT lRet;

	if (m_pDirect3D9)
	{
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
	m_pDirect3D9 = Direct3DCreate9( D3D_SDK_VERSION );  
	if( m_pDirect3D9 == NULL ) 
	{
		return FALSE; 
	}

	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 
	d3dpp.Windowed = TRUE;  
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; 
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; 

	D3DCAPS9 caps;
	DWORD BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	HRESULT hr = m_pDirect3D9->GetDeviceCaps ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ;
	if ( SUCCEEDED(hr) )
	{
		if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		{
			BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE ;
		}
		else
		{
			BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
		}
	}

	if (m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = NULL;
	}
	lRet = m_pDirect3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, BehaviorFlags, &d3dpp, &m_pDirect3DDevice );
	if(FAILED(lRet)) 
	{
		return FALSE; 
	}

	if (m_pDirect3DSurfaceRender)
	{
		m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;
	}
	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface( img_width, img_height, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, NULL); 
	if(FAILED(lRet))  
	{
		return FALSE;
	}

	return TRUE;  
}

BOOL CD3DVidRender::Render_RGB32(unsigned char * pdata, int img_width, int img_height)
{
	if (NULL == m_pDirect3DSurfaceRender)
	{
		return FALSE ;
	}

	HRESULT lRet;  

	D3DLOCKED_RECT d3d_rect;  
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);  
	if(FAILED(lRet))  
	{
		return FALSE;  
	} 
 
	byte * pSrc = pdata;  
	byte * pDest = (BYTE *)d3d_rect.pBits;  
	int lPitch = d3d_rect.Pitch;  
	unsigned long i = 0;  

	for (i = 0; i < img_height; i++)
	{
		memmove(pDest, pSrc, img_width * 4);
		pDest += lPitch;
		pSrc += img_width * 4;
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();  
	if(FAILED(lRet))  
	{
		return FALSE;  
	}  

	if (m_pDirect3DDevice == NULL)  
	{
		return FALSE;  
	} 

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);  
	m_pDirect3DDevice->BeginScene();  

	if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer); 
	GetClientRect(d3dpp.hDeviceWindow, &m_rtViewport);
	calculate_display_rect(&m_rtViewport,img_width,img_height, m_rtViewport.right, m_rtViewport.bottom) ;
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, m_pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);  
	//if (m_pDirect3DDevice != NULL)  
	//{
	//	m_rtFont = m_rtViewport ;
	//	m_rtFont.top = (m_rtViewport.bottom - m_rtViewport.top) * 3 / 4 ;
	//	m_pD3DXFont->DrawTextW(NULL, L"Programming is fun!哈哈", -1, &m_rtFont, DT_CENTER | DT_VCENTER, D3DCOLOR_XRGB(255,0,0));
	//}
	m_pDirect3DDevice->EndScene();  
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);

	RECT rcCurrentClient;
	GetClientRect(d3dpp.hDeviceWindow, &rcCurrentClient);
	if( ( UINT )rcCurrentClient.right != d3dpp.BackBufferWidth ||
		( UINT )rcCurrentClient.bottom != d3dpp.BackBufferHeight )
	{
		d3dpp.BackBufferWidth = rcCurrentClient.right;
		d3dpp.BackBufferHeight = rcCurrentClient.bottom;

		InitD3D_RGB32(d3dpp.hDeviceWindow, img_width, img_height);
	}

	if (m_pDirect3DDevice && m_pDirect3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		InitD3D_RGB32(d3dpp.hDeviceWindow, img_width, img_height);
	}

	return TRUE;  
}

void CD3DVidRender::calculate_display_rect(RECT *rect,int img_width, int img_height, int scr_width, int scr_height)
{
	double video_W	= img_width ;		// 视频宽度
	double video_H	= img_height ;		// 视频高度
	double show_H	= scr_height ;		//播放区域高度
	double show_W	= scr_width ;		//播放区域宽度

	if((video_W / video_H) <= show_W / show_H)
	{
		int adjust_W =(int)(show_W - (video_W / video_H ) * show_H) / 2 ;

		rect->left = adjust_W ;
		rect->top = 0 ;
		rect->right = scr_width - adjust_W ;
		rect->bottom = scr_height ;
	}
	else if((video_W / video_H) > show_W / show_H)
	{
		int adjust_H = (int)(show_H - (video_H / video_W) * show_W) / 2 ;

		rect->left	= 0 ;
		rect->top	= adjust_H ;
		rect->right = scr_width ;
		rect->bottom = scr_height - adjust_H ;
	}
}

BOOL CD3DVidRender::Render_YUV(unsigned char * pdata, int img_width, int img_height)
{
	HRESULT lRet = -1; 

	D3DLOCKED_RECT d3d_rect;  
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);  
	if(FAILED(lRet))  
	{
		return FALSE; 
	}

	byte *pSrc = pdata;  
	byte * pDest = (BYTE *)d3d_rect.pBits;  
	int stride = d3d_rect.Pitch;  
	unsigned long i = 0;  

	for(i = 0; i < img_height; i ++)
	{  
		memmove(pDest + i * stride,pSrc + i * img_width, img_width);  
	}  
	for(i = 0; i < img_height/2; i ++)
	{  
		memmove(pDest + stride * img_height + i * stride / 2,pSrc + img_width * img_height + img_width * img_height / 4 + i * img_width / 2, img_width / 2);  
	}   
	for(i = 0; i < img_height / 2; i ++)
	{  
		memmove(pDest + stride * img_height + stride * img_height / 4 + i * stride / 2,pSrc + img_width * img_height + i * img_width / 2, img_width / 2);  
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();   
	if(FAILED(lRet))  
	{
		return FALSE; 
	}

	m_pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 ); 
	m_pDirect3DDevice->BeginScene(); 
	if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer); 
	GetClientRect(d3dpp.hDeviceWindow, &m_rtViewport);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, m_pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene(); 
	m_pDirect3DDevice->Present( NULL, NULL, NULL, NULL ); 

	RECT rcCurrentClient;
	GetClientRect(d3dpp.hDeviceWindow, &rcCurrentClient);
	if( ( UINT )rcCurrentClient.right != d3dpp.BackBufferWidth ||
		( UINT )rcCurrentClient.bottom != d3dpp.BackBufferHeight )
	{
		d3dpp.BackBufferWidth = rcCurrentClient.right;
		d3dpp.BackBufferHeight = rcCurrentClient.bottom;

		InitD3D_YUV(d3dpp.hDeviceWindow, img_width, img_height);
	}

	if (m_pDirect3DDevice && m_pDirect3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		InitD3D_YUV(d3dpp.hDeviceWindow, img_width, img_height);
	}

	return TRUE; 
}