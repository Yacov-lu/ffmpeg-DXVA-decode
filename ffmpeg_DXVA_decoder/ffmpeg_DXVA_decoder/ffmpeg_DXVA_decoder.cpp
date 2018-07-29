// ffmpeg_DXVA_decoder.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ffmpeg_DXVA_decoder.h"
#include "ffmpeg_dxva2.h"
#include "D3DVidRender.h"

#define MAX_LOADSTRING 100

#pragma warning(disable : 4996)

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

HANDLE g_hdecodeThread = NULL;
HWND   g_hwWnd = NULL;
BOOL   g_bDecodeThreadCanRun = FALSE;

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FFMPEG_DXVA_DECODER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FFMPEG_DXVA_DECODER));

    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FFMPEG_DXVA_DECODER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FFMPEG_DXVA_DECODER);
	//wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		g_bDecodeThreadCanRun = TRUE;
		g_hwWnd = hWnd;
		g_hdecodeThread = CreateThread(NULL, 0, ThreadProc, 0, 0,NULL);
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		g_bDecodeThreadCanRun = FALSE;
		WaitForSingleObject(g_hdecodeThread, INFINITE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


CD3DVidRender m_D3DVidRender;
AVPixelFormat GetHwFormat(AVCodecContext *s, const AVPixelFormat *pix_fmts)
{
	InputStream* ist = (InputStream*)s->opaque;
	ist->active_hwaccel_id = HWACCEL_DXVA2;
	ist->hwaccel_pix_fmt = AV_PIX_FMT_DXVA2_VLD;
	return ist->hwaccel_pix_fmt;
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{

	const char *filename = "H:\\Ѹ������\\Ц������.Funny.Soccer.2016.HD720P.X264.AAC.Mandarin.CHS-ENG.Adans.mp4";

	int setup_hwdecode = 0;

	av_register_all();//ע�������

	AVFormatContext *fc = NULL;
	int res = avformat_open_input(&fc, filename, NULL, NULL);//���ļ�
	if (res < 0) {
		printf("error %x in avformat_open_input\n", res);
		return 1;
	}

	res = avformat_find_stream_info(fc, NULL);//ȡ������Ϣ
	if (res < 0)
	{
		printf("error %x in avformat_find_stream_info\n", res);
		return 1;
	}
	//������Ƶ������Ƶ��
	av_dump_format(fc, 0, filename, 0);//�г������ļ����������Ϣ
	int i;
	int videoindex = -1;

	for (i = 0; i < fc->nb_streams; i++)
	{
		if (fc->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
		}
	}
	if (videoindex == -1)
	{
		av_log(NULL, AV_LOG_DEBUG, "can't find video stream\n");
		return 0;

	}//û���ҵ���Ƶ�� 


	AVCodec *codec = avcodec_find_decoder(fc->streams[videoindex]->codec->codec_id);	//��������Ϣ�ҵ�������
	if (!codec)
	{
		printf("decoder not found\n");
		return 1;
	}

	AVCodecContext *codecctx = fc->streams[videoindex]->codec;

	bool bAccel = true;
	AVCodecContext *temp_codecctx = codecctx;
	memcpy(temp_codecctx, codecctx, sizeof(codecctx));

	if (bAccel && codecctx->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		switch (codec->id)
		{
		case AV_CODEC_ID_MPEG2VIDEO:
		case AV_CODEC_ID_H264:
		case AV_CODEC_ID_VC1:
		case AV_CODEC_ID_WMV3:
		case AV_CODEC_ID_HEVC:
		case AV_CODEC_ID_VP9:
		{
			codecctx->thread_count = 1;  // Multithreading is apparently not compatible with hardware decoding
			InputStream *ist = new InputStream();
			ist->hwaccel_id = HWACCEL_AUTO;
			ist->active_hwaccel_id = HWACCEL_AUTO;
			ist->hwaccel_device = "dxva2";
			ist->dec = codec;
			ist->dec_ctx = codecctx;

			codecctx->opaque = ist;
			if (dxva2_init(codecctx, g_hwWnd) == 0)
			{
				codecctx->get_buffer2 = ist->hwaccel_get_buffer;
				codecctx->get_format = GetHwFormat;
				codecctx->thread_safe_callbacks = 1;

				break;
			}

			bAccel = false;
			break;
		}
		default:
			bAccel = false;
			break;
		}
	}

	AVFrame	*pFrameBGR = NULL;
	uint8_t	*out_buffer = NULL;
	struct SwsContext *img_convert_ctx = NULL;
	if (!bAccel)
	{
		avcodec_close(codecctx);
		codecctx = temp_codecctx;

		m_D3DVidRender.InitD3D_YUV(g_hwWnd, codecctx->width, codecctx->height);

		pFrameBGR = av_frame_alloc();
		out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codecctx->width, codecctx->height, 1));
		av_image_fill_arrays(pFrameBGR->data, ((AVPicture *)pFrameBGR)->linesize, out_buffer, AV_PIX_FMT_YUV420P, codecctx->width, codecctx->height, 1);
		img_convert_ctx = sws_getContext(codecctx->width, codecctx->height, codecctx->pix_fmt, codecctx->width, codecctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	}

	res = avcodec_open2(codecctx, codec, NULL);
	if (res < 0) {
		printf("error %x in avcodec_open2\n", res);
		return 1;
	}

	AVPacket pkt = { 0 };
	AVFrame *picture = av_frame_alloc();
	DWORD wait_for_keyframe = 60;

	int count = 0;

	while (g_bDecodeThreadCanRun && av_read_frame(fc, &pkt) == 0)
	{
		if (pkt.stream_index == videoindex)
		{
			int got_picture = 0;

			DWORD t_start = GetTickCount();
			int bytes_used = avcodec_decode_video2(codecctx, picture, &got_picture, &pkt);
			if (got_picture)
			{
				if (bAccel)
				{
					//��ȡ����ͬʱ��Ⱦ
					dxva2_retrieve_data_call(codecctx, picture);

					DWORD t_end = GetTickCount();
					printf("dxva2 time using: %lu\n", t_end - t_start);
				}
				else
				{
					if (img_convert_ctx &&pFrameBGR && out_buffer)
					{
						//ת�����ݲ���Ⱦ
						sws_scale(img_convert_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, codecctx->height, pFrameBGR->data, pFrameBGR->linesize);
						m_D3DVidRender.Render_YUV(out_buffer, picture->width, picture->height);

						DWORD t_end = GetTickCount();
						printf("normal time using: %lu\n", t_end - t_start);
					}
				}

				count++;
			}
			Sleep(30u);
			av_packet_unref(&pkt);
		}
		else
		{
			av_packet_unref(&pkt);
		}
	}

	av_free(picture);

	avcodec_close(codecctx);
	avformat_close_input(&fc);

	return 0;
}