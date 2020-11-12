// FingerWnd.cpp : implementation file
//

#include "stdafx.h"
#include "FingerWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  PT_SIZE	6

/////////////////////////////////////////////////////////////////////////////
// CFingerWnd

CFingerWnd::CFingerWnd()
{
	m_pImg = NULL;
	m_nImageWidth = 0;
	m_nImageHeight = 0;

	m_bDrawImageArea = FALSE;
}

CFingerWnd::~CFingerWnd()
{
	if (m_pImg)
	{
		delete[] m_pImg;
	}
}


BEGIN_MESSAGE_MAP(CFingerWnd, CStatic)
	//{{AFX_MSG_MAP(CFingerWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFingerWnd message handlers

void CFingerWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CStatic::OnLButtonDown(nFlags, point);
}

void CFingerWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{		
	CStatic::OnRButtonDown(nFlags, point);
}

void CFingerWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
		
	DrawImage();
}

BOOL CFingerWnd::IsEmpty()
{
	if (!m_pImg)
		return TRUE;

	return FALSE;
}

BOOL CFingerWnd::SetImage( PBYTE p_pImg, int p_nWidth, int p_nHeight, BOOL p_bStretch/* = TRUE*/ )
{
	if (p_pImg == NULL) 
	{
		ClearFpImage();
	}
	else
	{
		if (m_pImg)
		{
			delete[] m_pImg;
		}
		m_pImg = new BYTE[p_nWidth * p_nHeight];
		memcpy(m_pImg, p_pImg, p_nWidth*p_nHeight);
		m_nImageWidth = p_nWidth;
		m_nImageHeight = p_nHeight;
	}	

	DrawImage(p_bStretch);

	return TRUE;
}

#define		LM_SIZEOF_4(pp_value) (((pp_value / 4) + ((pp_value % 4)? 1: 0)) * 4)
#define		LM_MOD_OF_4(pp_value) ((pp_value % 4)? (4 - (pp_value % 4)): 0)

int	ImgToBmp(							//:CAL:
			 int					p_X				//:IN :
			 ,	int					p_Y				//:IN :
			 ,	char*				p_bmp			//:OUT:
			 ,	char*				p_img			//:IN :
			 )
{
	int					y					;
	char				*p_mono				;
	int					i,idx				;
	int					w_buff_point		;
	{ //.if ( p_inf == 8 ) {
		for( idx = 0,y = p_Y ; y > 0 ; y -= 1 ) {
			w_buff_point = p_X * (y -1) ;
			p_mono = &p_img[w_buff_point] ;
			for( i = 0 ; i < p_X ; idx++, i++){
				p_bmp[idx] = p_mono[i];
			}
			for( i = 0 ; i < LM_MOD_OF_4(p_X) ; idx++, i++){
				p_bmp[idx] = (UCHAR)255;
			}
		}
	}
	return 0 ;
}

HBITMAP GetHBitmap( HDC p_hdc, PBYTE p_pImage, long p_nWidth, long p_nHeight )
{
	unsigned char head[1064]={
		/***************************/
		//infoheader
		0x28,0x00,0x00,0x00,//struct size
			0x00,0x00,0x00,0x00,//map width*** 
			0x00,0x00,0x00,0x00,//map height***
			
			0x01,0x00,//must be 1 planes
			0x08,0x00,//color count***
			0x00,0x00,0x00,0x00, //compression
			//0x00,0x68,0x01,0x00,//data size***
			0x00,0x00,0x00,0x00,//data size***
			0x00,0x00,0x00,0x00, //dpix
			0x00,0x00,0x00,0x00, //dpiy
			0x00,0x00,0x00,0x00,//color used
			0x00,0x00,0x00,0x00,//color important
	};
	
	long num;
	num=p_nWidth; head[4]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[5]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[6]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[7]= (unsigned char)(num & 0xFF);
	
	
	num=p_nHeight; head[8]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[9]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[10]= (unsigned char)(num & 0xFF);
	num=num>>8;  head[11]= (unsigned char)(num & 0xFF);
	
	unsigned int i,j;
	
	j=0;
	for (i=40;i<1064;i=i+4)
	{
		head[i]=head[i+1]=head[i+2]=j; 
		head[i+3]=0;
		j++;
	}	
	
	HBITMAP hBitmap = NULL;
	hBitmap = ::CreateCompatibleBitmap( p_hdc, p_nWidth, p_nHeight );
	::SetDIBits( p_hdc, hBitmap, 0, p_nHeight, p_pImage, (BITMAPINFO*)head, DIB_RGB_COLORS );
	
	return hBitmap;
}

void CFingerWnd::DrawImageBuffer(HDC p_hdc, BYTE* p_pImageBuffer, int p_w, int p_h)
{
	HDC		w_hdc = p_hdc;	
	HBITMAP hBitmap = NULL;
	BYTE*	w_pTmpBuf;
	
	w_pTmpBuf = new BYTE[p_w * p_h * 4];

	if ( w_hdc != NULL && p_pImageBuffer != NULL )
	{
		ImgToBmp( p_w, p_h, (char*)w_pTmpBuf, (char*)p_pImageBuffer );
		hBitmap = GetHBitmap( w_hdc, w_pTmpBuf, p_w, p_h );
		HDC hdcMem = ::CreateCompatibleDC(w_hdc);
		HBITMAP hBmOld = (HBITMAP)::SelectObject( hdcMem, hBitmap );
		
		BitBlt(w_hdc, 0, 0, p_w, p_h, hdcMem, 0, 0, SRCCOPY);
		
		::SelectObject( hdcMem, hBmOld );
		::DeleteObject( hBitmap );
		::DeleteDC(hdcMem);
	}

	delete[] w_pTmpBuf;
}
#define IMAGE_AREA_LEFT		144
#define IMAGE_AREA_TOP		6
#define IMAGE_AREA_RIGHT	434
#define IMAGE_AREA_BOTTOM	478
void CFingerWnd::DrawImage(BOOL p_bStretch/* = TRUE*/)
{
	CClientDC	dc(this); // device context for painting
	CRect		rt;
	CDC			MemDC;	
	CBitmap		bmp, *pOldBmp;
	BYTE*		w_pTmpBuf;
	int			w_nDrawWidth;
	int			w_nDrawHeight;
	
	GetClientRect(&rt);

	if (m_pImg)
	{
		w_pTmpBuf = new BYTE[m_nImageWidth*m_nImageHeight];
		
		MemDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, m_nImageWidth, m_nImageHeight);
		pOldBmp = MemDC.SelectObject(&bmp);
		
		DrawImageBuffer( MemDC, m_pImg, m_nImageWidth, m_nImageHeight);
		
		if (p_bStretch)
		{
			dc.SetStretchBltMode(HALFTONE);
			dc.StretchBlt(	0, 0, rt.Width(), rt.Height(), 
				&MemDC, 
				0, 0, m_nImageWidth, m_nImageHeight,
				SRCCOPY);
		}
		else
		{
			CBrush		br, *pOldbr;

			br.CreateSolidBrush(RGB(255, 255, 255));

			pOldbr = dc.SelectObject(&br);

			dc.Rectangle(rt);
			w_nDrawWidth = m_nImageWidth;
			w_nDrawHeight = m_nImageHeight;
			if (w_nDrawWidth > rt.Width())
				w_nDrawWidth = rt.Width();
			if (w_nDrawHeight > rt.Height())
				w_nDrawHeight = rt.Height();
			dc.BitBlt(0, 0, w_nDrawWidth, w_nDrawHeight, &MemDC, 0, 0, SRCCOPY);

			br.DeleteObject();

			dc.SelectObject(pOldbr);
		}
		
		MemDC.SelectObject(pOldBmp);
		MemDC.DeleteDC();
		bmp.DeleteObject();
		
		delete[] w_pTmpBuf;
	}
	else
	{
		CBrush		br, *pOldbr;
		
		br.CreateSolidBrush(RGB(0, 0, 0));
		
		pOldbr = dc.SelectObject(&br);
		
		dc.Rectangle(rt);

		br.DeleteObject();
		
		dc.SelectObject(pOldbr);
	}
}

void CFingerWnd::DrawImageArea(BOOL p_bDrawImageArea)
{
	m_bDrawImageArea = p_bDrawImageArea;
}

void CFingerWnd::ClearFpImage()
{
	if (m_pImg)
	{
		delete[] m_pImg;
		m_pImg = NULL;
	}

	m_nImageWidth = 0;
	m_nImageHeight = 0;

	DrawImage();
}