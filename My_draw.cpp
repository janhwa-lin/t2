// Wireless_RxView.cpp : implementation of the CWireless_RxView class
//

#include "stdafx.h"
#include "Wireless_Rx.h"

#include "Wireless_RxDoc.h"
#include "Wireless_RxView.h"

#include "inc/RTBridgeBoardCore.h"
#include "inc/RTI2CModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	typedef struct _DATA_LOG_DEF_ {
//		CString sName;
//		CString sLastValue;
//		float*  fData;
//		float*	fToggleLog;
//		float	fMax;
//		float	fMin;
//		CPoint* ptCurve;
//		CPoint	ptBase;
//		COLORREF	clr;
//		CPen	pen;
//		BOOL	bShow;
//		int		nDrawType;
//	};


// CWireless_RxView

IMPLEMENT_DYNCREATE(CWireless_RxView, CView)

BEGIN_MESSAGE_MAP(CWireless_RxView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_COMMAND_I2CTEST, &CWireless_RxView::OnCommandI2ctest)
	//ON_BN_CLICKED(IDC_BUTTON_TEST2, &CWireless_RxView::OnBnClickedButtonTest2)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CWireless_RxView construction/destruction

CWireless_RxView::CWireless_RxView()
{

}

CWireless_RxView::~CWireless_RxView()
{
	if (m_pDCMem != NULL)
	{
		ENSURE(m_pbitmapOld != NULL);
		m_pDCMem->SelectObject(m_pbitmapOld);
		delete m_pDCMem;
		delete m_pBitmap;
	}
}

BOOL CWireless_RxView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bPreCreated = TRUE;
	bPreCreated = CView::PreCreateWindow(cs);
	if( !bPreCreated )
		return FALSE;

	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//AfxMessageBox(_T("PreCreateWindow"));

	return CView::PreCreateWindow(cs);
}

// CWireless_RxView drawing

//void CWireless_RxView::OnDraw(CDC* /*pDC*/)
void CWireless_RxView::OnDraw(CDC* pDC)
{

	//AfxMessageBox(_T("On Draw"));

	CWireless_RxDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//AfxMessageBox(_T("pDoc is OK"));

	const int BIT_LDO_EN = 0x01;
	const int BIT_ILIM_FLAG = 0x02;
	const int BIT_CHG_DONE = 0x04;
	const int BIT_BAT_PRECHARGE = 0x08;
	const int BIT_BAT_SHORT = 0x10;
	const int BIT_THERMAL_FLAG = 0x20;
	const int BIT_HOT_FLAG = 0x40;
	const int BIT_BAT_FULL= 0x80;

	CPaintDC*	pDCPaint;
	CRect		rectClip;
	CRect	rectLogClip;
	pDCPaint = (CPaintDC*)pDC;
	rectClip = pDCPaint->m_ps.rcPaint;
	rectLogClip = rectClip;

	if (m_pDCMem == NULL)
	{
		m_pDCMem = new CDC();
		ASSERT(m_pDCMem != NULL);
		VERIFY(m_pDCMem->CreateCompatibleDC(pDC));
		//m_pDCMem->SetMapMode(MM_LOENGLISH);
	}

	if (m_pBitmap == NULL)
		m_pBitmap = new CBitmap();

	CString s;

	CRect client_rect( 100, 100, 200, 200);
	//CRect client_rect;
	GetClientRect(&client_rect);

	int cxClip = client_rect.Width();
	int cyClip = client_rect.Height();
	HBITMAP hBitMap;
	if (cxClip > m_sizeBitmap.cx || cyClip > m_sizeBitmap.cy)
	{
		if (m_pbitmapOld != NULL)
		{
			m_pDCMem->SelectObject(m_pbitmapOld);
			m_pBitmap->DeleteObject();
		}

		m_pBitmap->CreateCompatibleBitmap(pDC, cxClip, cyClip);
		hBitMap = (HBITMAP)(*m_pBitmap);


		//hBitMap =  (HBITMAP) m_pBitmap->GetSafeHandle();
		m_pbitmapOld = m_pDCMem->SelectObject(m_pBitmap);
		m_sizeBitmap.cx = cxClip;
		m_sizeBitmap.cy = cyClip;
	}

	GetClientRect(&client_rect);
	m_pDCMem->FillSolidRect(&client_rect, RGB(255, 255, 255));

	CPoint pt_base( 50, 450);

	int max_curve_high = 400;
	int cur_x;
	int cur_y;
	double last_diff = 0;
	double curr_diff = 0;
	double show_diff = 6;

	TRIVERTEX vert[2];
    GRADIENT_RECT gRect;
	CRect rect1;
	int no_log_cnt = 0;


	CPen Pen_gray_dot(PS_SOLID, 1, RGB(128, 128, 128));
	m_pDCMem->SelectObject(Pen_gray_dot);
	m_pDCMem->MoveTo(pt_base);
	m_pDCMem->LineTo( pt_base.x + 1000, pt_base.y);
	//m_pDCMem->LineTo( pt_base.x + 1000, pt_base.y - max_curve_high - 10);
	//m_pDCMem->LineTo( pt_base.x,  pt_base.y - max_curve_high - 10);
	//m_pDCMem->LineTo(pt_base);

	int ignore_cnt;

	//--------------------------------------------------------------------------------
	// draw Iout
	//--------------------------------------------------------------------------------
	COLORREF clr_iout;
	clr_iout = RGB(220, 20, 20);
	m_pDCMem->SetTextColor( clr_iout );
	//s.Format( _T("IOUT"));
	//m_pDCMem->TextOutW( pt_base.x - 10, pt_base.y - max_curve_high , s );

	CPen Pen_2(PS_SOLID, 1, clr_iout );
	m_pDCMem->SelectObject(Pen_2);
	m_pDCMem->MoveTo(pt_base);
	double fIoutZoom = max_curve_high / pDoc->m_fIout_max * 0.85;
	cur_x = pt_base.x;
	show_diff = 5;
	ignore_cnt = 0;

    vert[1].x = pt_base.x;
    vert[1].y = pt_base.y;
    vert[0].Red = 0xff00;
    vert[0].Green = 0xff00;
    vert[0].Blue = 0xf000;
    vert[0].Alpha = 0xF000;
    vert[0].x = pt_base.x + 200;
    vert[0].y = pt_base.y + 18*5;
    vert[1].Red = 0xff00;
    vert[1].Green = 0xCf00;
    vert[1].Blue = 0xff00;
    vert[1].Alpha = 0xFf00;
    gRect.UpperLeft = 1;
    gRect.LowerRight = 0;

	//m_pDCMem->MoveTo(cur_x, pt_base.y - pDoc->m_fIout[pDoc->m_nLastIdx+1] * fIoutZoom);
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	//{
	//	cur_y = pt_base.y - pDoc->m_fIout[i] * fIoutZoom;

	//	no_log_cnt ++;
	//	vert[1].x = cur_x;
	//	vert[1].y = cur_y;
	//	vert[0].x = cur_x + 1;
	//	vert[0].y = pt_base.y;
	//	GradientFill(m_pDCMem->GetSafeHdc(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	//	m_pDCMem->LineTo(cur_x, cur_y);
	//	cur_x ++;
	//	if (i==pDoc->m_nLastIdx+1)
	//		curr_diff = 10;
	//	else 
	//		curr_diff = pDoc->m_fIout[i] - pDoc->m_fIout[i-1];
	//	if (//(no_log_cnt > 100) 
	//		 (pDoc->m_nLogPt[i] > 0) 
	//		//((ignore_cnt == 0) && abs(curr_diff - last_diff) > abs(last_diff * 1.8)) )
	//		)
	//	{
	//		no_log_cnt =0;
	//		ignore_cnt = 50;
	//		s.Format( _T("%.0f mA"), pDoc->m_fIout[i]);
	//		if (pDoc->m_fIout[i] < pDoc->m_fIout[i-1])
	//			m_pDCMem->TextOutW( cur_x + 5,  cur_y - 20, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 25,  cur_y - 20, s );
	//		
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_iout );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}
	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	cur_y = pt_base.y - pDoc->m_fIout[i] * fIoutZoom;
	//	no_log_cnt ++;
	//	vert[1].x = cur_x;
	//	vert[1].y = cur_y;
	//	vert[0].x = cur_x + 1;
	//	vert[0].y = pt_base.y;
	//	GradientFill(m_pDCMem->GetSafeHdc(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fIout[i] * fIoutZoom);
	//	cur_x ++;

	//	curr_diff = pDoc->m_fIout[i] - pDoc->m_fIout[i-1];

	//	if ( i == pDoc->m_nLastIdx-1 ||
	//		//((ignore_cnt == 0) && abs(curr_diff - last_diff) > abs(last_diff * 1.8)) ||
	//		(pDoc->m_nLogPt[i] > 0) //|| 
	//		//(no_log_cnt > 100)
	//		)
	//	{
	//		ignore_cnt = 50;

	//		no_log_cnt =0;
	//		s.Format( _T("%.0f mA"), pDoc->m_fIout[i]);
	//		if (i == pDoc->m_nLastIdx-1)
	//			s.AppendFormat( _T(" (IOUT) "));
	//		if (pDoc->m_fIout[i] < pDoc->m_fIout[i-1])
	//			m_pDCMem->TextOutW( cur_x + 5,  cur_y - 20, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 25,  cur_y - 20, s );
	//		
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_iout );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}




    //vert[1].x = posCF.x;
    //vert[1].y = posCF.y;
    //vert[0].Red = 0xdf00;
    //vert[0].Green = 0xdf00;
    //vert[0].Blue = 0xdf00;
    //vert[0].Alpha = 0xff00;
    //vert[0].x = posCF.x + 200;
    //vert[0].y = posCF.y + 18*10;
    //vert[1].Red = 0xff00;
    //vert[1].Green = 0xff00;
    //vert[1].Blue = 0xff00;
    //vert[1].Alpha = 0xFff0;
    //gRect.UpperLeft = 1;
    //gRect.LowerRight = 0;
	//GradientFill(pDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);



	CFont font;
	CFont font_bold;
	CSize size = m_pDCMem->GetTextExtent(_T("Hello World!")); // measure the Hello World! string
	font.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
	font_bold.CreateFont(14, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
	//font.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arrial"));
	CGdiObject *pOldFont = m_pDCMem->SelectObject(&font); // save the current font






	CRect rect3;



	////--------------------------------------------------------------------------------
	//// draw Temperature
	////--------------------------------------------------------------------------------
	//COLORREF clr_ts;
	////clr_ts = RGB(140, 55, 209);
	//clr_ts = RGB(80, 185, 199);

	//m_pDCMem->SetTextColor( clr_ts );

	//double fTempZoom = 3.5;
	//CPen Pen_5(PS_DOT, 1, clr_ts);
	//m_pDCMem->SelectObject(Pen_5);
	//cur_x = pt_base.x;
	//show_diff = 0.5;
	//no_log_cnt = 0;
	//double last2 = 0;
	//last2 = 0;
	//cur_y = pt_base.y - pDoc->m_fbuf[2][pDoc->m_nLastIdx+1] * fTempZoom;
	//m_pDCMem->MoveTo(cur_x, cur_y);
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fbuf[2][i] * fTempZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fbuf[2][i] * fTempZoom);

	//	cur_x ++;
	//	if (i==pDoc->m_nLastIdx+1)
	//		curr_diff = 10;
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fbuf[2][i+1] - pDoc->m_fbuf[2][i];

	//	bool bTS_REGULATE = (pDoc->m_status[i] & BIT_THERMAL_FLAG) ^ (pDoc->m_status[i-1] & BIT_THERMAL_FLAG);

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fbuf[2][i] - pDoc->m_fbuf[2][i-2]);
	//	if ( //((ignore_cnt == 0) && pDoc->check_boundary(2, i, 0.3)) || 
	//		  no_log_cnt > 150 || 
	//		  bTS_REGULATE ||
	//		  abs(pDoc->m_fbuf[2][i]-pDoc->m_fbuf[2][i-1]) > 50
	//		  )
	//	{

	//		ignore_cnt = 32;
	//		s.Format( _T("%.0f oC"), pDoc->m_fbuf[2][i]);
	//		if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		no_log_cnt =  0;
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_ts );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}

	//cur_y = pt_base.y - pDoc->m_fbuf[2][0] * fTempZoom;
	//m_pDCMem->MoveTo(cur_x, cur_y);
	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fbuf[2][i] * fTempZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fbuf[2][i] * fTempZoom);
	//	cur_x ++;
	//	curr_diff = pDoc->m_fbuf[2][i] - pDoc->m_fbuf[2][i-1];

	//	bool bTS_REGULATE = (pDoc->m_status[i] & BIT_THERMAL_FLAG) ^ (pDoc->m_status[i-1] & BIT_THERMAL_FLAG);

	//	if (i > 2)
	//		last_diff = (pDoc->m_fbuf[2][i] - pDoc->m_fbuf[2][i-2]);
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fbuf[2][i+1] - pDoc->m_fbuf[2][i];

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fbuf[2][i] - pDoc->m_fbuf[2][i-2]);
	//	if ( (ignore_cnt == 0 && i < pDoc->m_nLastIdx-20) && (
	//		  //pDoc->check_boundary(2, i, 0.3) || 
	//		  abs(pDoc->m_fbuf[2][i]-pDoc->m_fbuf[2][i-1]) > 50)
	//		  || bTS_REGULATE
	//		  || (i == pDoc->m_nLastIdx-1)
	//		  || (no_log_cnt > 250)
	//		  )
	//	{
	//		ignore_cnt = 32;
	//		no_log_cnt = 0;
	//		s.Format( _T("%.0f oC"), pDoc->m_fbuf[2][i]);
	//		if (i == pDoc->m_nLastIdx-1)
	//		{
	//			s.AppendFormat( _T(" (Temperature) "));
	//			m_pDCMem->TextOutW( cur_x + 20,  cur_y - 7, s );
	//		}
	//		else if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_ts );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}













	////--------------------------------------------------------------------------------
	//// draw Vrect
	////--------------------------------------------------------------------------------
	//COLORREF clr_ce;
	//clr_ce = RGB(20, 255, 149);
	//COLORREF clr_vrect;
	//clr_vrect = RGB(2, 155, 49);
	//m_pDCMem->SetTextColor( clr_vrect );
	////s.Format( _T("VRECT"));
	////m_pDCMem->TextOutW( pt_base.x - 10, pt_base.y - max_curve_high +20 , s );

	//CPen Pen_vrect(PS_SOLID, 1, clr_vrect);
	//CPen Pen_debug(PS_SOLID, 2, clr_vrect);
	//m_pDCMem->SelectObject(Pen_vrect);
	//m_pDCMem->MoveTo(pt_base);
	//double fVrectZoom = max_curve_high / pDoc->m_fVrect_max;
	//cur_x = pt_base.x;
	//show_diff = 0.5;
	//no_log_cnt = 0;
	//double diff_para = 0.02;
	//m_pDCMem->MoveTo(cur_x, pt_base.y - pDoc->m_fVrect[pDoc->m_nLastIdx+1] * fVrectZoom);
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fVrect[i] * fVrectZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fVrect[i] * fVrectZoom);
	//	cur_x ++;
	//	if (i==pDoc->m_nLastIdx+1)
	//		curr_diff = 10;
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fVrect[i+1] - pDoc->m_fVrect[i];



	//	int ce = (int) pDoc->m_fbuf[0][i];
	//	if (ce != 0)
	//	{
	//		if (ce > 0)
	//			rect3 = CRect( cur_x, cur_y - ce*10, cur_x+2, cur_y - 5);
	//		else
	//			rect3 = CRect( cur_x, cur_y - ce*10, cur_x+2, cur_y + 5);
	//		m_pDCMem->FillSolidRect(&rect3, clr_ce );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}



	//	pDoc->m_nLogPt[i] = 0;

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fVrect[i] - pDoc->m_fVrect[i-2]);
	//	if ( ((ignore_cnt == 0) && pDoc->check_boundary(0, i, 0.3)) || 
	//		  no_log_cnt > 100  
	//		 // (pDoc->m_fbuf[0][i-1] ==0 && pDoc->m_fbuf[0][i] != 0) ||
	//		  //abs(pDoc->m_fVrect[i]-pDoc->m_fVrect[i-1]) > 50
	//		  )
	//	{

	//		pDoc->m_nLogPt[i] = 1;
	//		ignore_cnt = 32;
	//		s.Format( _T("%.2fV"), pDoc->m_fVrect[i]);
	//		m_pDCMem->TextOutW( cur_x - 35, cur_y - 20, s );

	//		no_log_cnt =  0;
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_vrect );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	double last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}
	//m_pDCMem->MoveTo(cur_x, pt_base.y - pDoc->m_fVrect[0] * fVrectZoom);
	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fVrect[i] * fVrectZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fVrect[i] * fVrectZoom);
	//	cur_x ++;
	//	curr_diff = pDoc->m_fVrect[i] - pDoc->m_fVrect[i-1];


	//	int ce = (int) pDoc->m_fbuf[0][i];
	//	if (ce != 0)
	//	{
	//		if (ce > 0)
	//			rect3 = CRect( cur_x, cur_y - ce*10, cur_x+2, cur_y - 5);
	//		else
	//			rect3 = CRect( cur_x, cur_y - ce*10, cur_x+2, cur_y + 5);
	//		m_pDCMem->FillSolidRect(&rect3, clr_ce );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}


	//	pDoc->m_nLogPt[i] = 0;

	//	if (i > 2)
	//		last_diff = (pDoc->m_fVrect[i] - pDoc->m_fVrect[i-2]);
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fVrect[i+1] - pDoc->m_fVrect[i];

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fVrect[i] - pDoc->m_fVrect[i-2]);
	//	if ( (ignore_cnt == 0 && i < pDoc->m_nLastIdx-20) && (
	//		  pDoc->check_boundary(0, i, 0.3) || 
	//		 // abs(pDoc->m_fVrect[i]-pDoc->m_fVrect[i-1]) > 50) ||
	//		  //(pDoc->m_fbuf[0][i-1] ==0 && pDoc->m_fbuf[0][i] != 0) ||
	//		  //(i == pDoc->m_nLastIdx-1) ||
	//		  (no_log_cnt > 100))
	//		  )
	//	{
	//		pDoc->m_nLogPt[i] = 1;
	//		ignore_cnt = 32;
	//		no_log_cnt = 0;
	//		s.Format( _T("%.2fV"), pDoc->m_fVrect[i]);
	//		if (i == pDoc->m_nLastIdx-1)
	//		{
	//			s.AppendFormat( _T(" (VRECT) "));
	//			m_pDCMem->TextOutW( cur_x + 20,  cur_y - 7, s );
	//		}
	//		else
	//			m_pDCMem->TextOutW( cur_x - 35, cur_y - 20, s );

	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr_vrect );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	double last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}

	//--------------------------------------------------------------------------------
	// draw new Vout
	//--------------------------------------------------------------------------------
	_DATA_LOG_DEF_* pD;

	int previous_log_x = 0;
	for(int draw_sel=0; draw_sel<12; draw_sel++)
	//for(int draw_sel=0; draw_sel<2; draw_sel++)
	{
		switch (draw_sel)
		{
		case 0:	pD = &pDoc->ibus; break;
		case 1:	pD = &pDoc->ibat; break;
		case 2:	pD = &pDoc->ibat_set; break;
		case 3:	pD = &pDoc->isys; break;
		case 4:	pD = &pDoc->iin_ta; break;
		case 5:	pD = &pDoc->imid; break;

		case 6:	 pD = &pDoc->vbus; break;
		case 7:	 pD = &pDoc->vbat; break;
		case 9:	 pD = &pDoc->vsys; break;
		case 10: pD = &pDoc->vin_ta; break;
		case 11: pD = &pDoc->vmid; break;
		default:
			break;
		}
		//if (draw_sel == 0)
		//	pD = &pDoc->new_iout;
		//else if (draw_sel == 1)
		//	pD = &pDoc->new_vrect;
		//else if (draw_sel == 2)
		//	pD = &pDoc->new_vout;
		//else if (draw_sel == 3)
		//	pD = &pDoc->new_rp;
		//else if (draw_sel == 4)
		//	pD = &pDoc->new_ts;
		//else
		//	break;

		m_pDCMem->SetTextColor( pD->clr);
		m_pDCMem->SelectObject( pD->pen);
		int idx = pDoc->m_nLastIdx;
		double ZoomScale = max_curve_high / (pD->fMax - pD->fMin/1.2) * pD->dShowScale;
		bool bFirstMinLog = true;
		bool bFirstMaxLog = true;
		for(int i=0; i<1000; i++)
		{
			if (idx == DATA_LENGTH)
				idx = 0;
			cur_x = pt_base.x + i;
			cur_y = pt_base.y - (pD->fData[idx] - pD->fMin/1.2)*ZoomScale - pD->ptBase.y;
			if (i == 0)
				m_pDCMem->MoveTo(cur_x, cur_y);
			else 
			{
				m_pDCMem->LineTo(cur_x, cur_y);

				if (
					(pD->fToggleLog[idx] != pD->fToggleLog[idx-1]) ||
					(((pD->fData[idx] == pD->fMin)) && bFirstMinLog) ||
					(((pD->fData[idx] == pD->fMax)) && bFirstMaxLog) ||
					(((pD->fData[idx] == pD->fMax) || (pD->fData[idx] == pD->fMin)) && (cur_x > previous_log_x + 180))
					)
				{
					rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
					m_pDCMem->FillSolidRect(&rect1, pD->clr );
					m_pDCMem->SetBkColor(RGB(255, 255, 255));

					if (pD->sUnit == _T("V"))
						s.Format( _T("%.2f"), pD->fData[idx]);
					else
						s.Format( _T("%.0f"), pD->fData[idx]);
					s += pD->sUnit;
					m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
					previous_log_x = cur_x;
					if (pD->fData[idx] == pD->fMin)	
						bFirstMinLog = false;
					if (pD->fData[idx] == pD->fMax)	
						bFirstMaxLog = false;
				}

				if (draw_sel == 0)
				{
				vert[1].x = cur_x;
				vert[1].y = cur_y;
				vert[0].x = cur_x + 1;
				vert[0].y = pt_base.y;
				GradientFill(m_pDCMem->GetSafeHdc(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
				}

			}

			if (i == 999)
			{

				int pre_idx = (pDoc->m_nLastIdx > 0) ? pDoc->m_nLastIdx-1 : 0;
				if (pD->sUnit == _T("V"))
					pD->sLastValue.Format( _T("%.2f"), pD->fData[pre_idx]);
				else
					pD->sLastValue.Format( _T("%.0f"), pD->fData[pre_idx]);
				pD->sLastValue += pD->sUnit + _T(" (") + pD->sName + _T(")  ");

				m_pDCMem->TextOutW( cur_x+ 20,  cur_y - 7, pD->sLastValue );
				rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
				m_pDCMem->FillSolidRect(&rect1, pD->clr );
				m_pDCMem->SetBkColor(RGB(255, 255, 255));
			}

			idx ++;
		}
	}
	//--------------------------------------------------------------------------------



	////--------------------------------------------------------------------------------
	//// draw Vout
	////--------------------------------------------------------------------------------
	//COLORREF clr;
	//clr = RGB(92, 55, 149);
	//m_pDCMem->SetTextColor( clr );
	////s.Format( _T("VOUT"));
	////m_pDCMem->TextOutW( pt_base.x - 10, pt_base.y - max_curve_high -20 , s );

	//CPen Pen_3(PS_SOLID, 1, clr);
	//m_pDCMem->SelectObject(Pen_3);
	//double fVoutZoom = max_curve_high / pDoc->m_fVout_max;
	//cur_x = pt_base.x;
	//show_diff = 0.5;
	//no_log_cnt = 0;
	//last2 = 0;
	//diff_para = 0.02;
	//m_pDCMem->MoveTo(cur_x, pt_base.y - pDoc->m_fVout[pDoc->m_nLastIdx+1] * fVrectZoom);
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fVout[i] * fVoutZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fVout[i] * fVoutZoom);
	//	cur_x ++;
	//	if (i==pDoc->m_nLastIdx+1)
	//		curr_diff = 10;
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fVout[i+1] - pDoc->m_fVout[i];

	//	//bool bBAT_SHORT_EDGE = (pDoc->m_status[i] & BIT_BAT_SHORT) ^ (pDoc->m_status[i-1] & BIT_BAT_SHORT);
	//	bool bBAT_SHORT_EDGE = (pDoc->m_status[i]) ^ (pDoc->m_status[i-1]);


	//	if (i > 2 )
	//		last_diff = (pDoc->m_fVout[i] - pDoc->m_fVout[i-2]);
	//	if ( ((ignore_cnt == 0) && pDoc->check_boundary(2, i, 0.3)) || 
	//		  no_log_cnt > 200 || 
	//		  (abs(pDoc->m_fIout[i]-pDoc->m_fIout[i-1]) > 100) ||
	//		  abs(pDoc->m_fVout[i]-pDoc->m_fVout[i-1]) > 50 ||
	//		  bBAT_SHORT_EDGE
	//		  )
	//	{

	//		ignore_cnt = 32;
	//		s.Format( _T("%.2fV"), pDoc->m_fVout[i]);
	//		if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		no_log_cnt =  0;
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}
	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fVout[i] * fVoutZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fVout[i] * fVoutZoom);
	//	cur_x ++;
	//	curr_diff = pDoc->m_fVout[i] - pDoc->m_fVout[i-1];

	//	if (i > 2)
	//		last_diff = (pDoc->m_fVout[i] - pDoc->m_fVout[i-2]);
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fVout[i+1] - pDoc->m_fVout[i];

	//	//bool bBAT_SHORT_EDGE = (pDoc->m_status[i] & BIT_BAT_SHORT) ^ (pDoc->m_status[i-1] & BIT_BAT_SHORT);
	//	bool bBAT_SHORT_EDGE = (pDoc->m_status[i]) ^ (pDoc->m_status[i-1]);

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fVout[i] - pDoc->m_fVout[i-2]);
	//	if ( (ignore_cnt == 0 && i < pDoc->m_nLastIdx-20) && (
	//		  pDoc->check_boundary(2, i, 0.3) || 
	//		  abs(pDoc->m_fVout[i]-pDoc->m_fVout[i-1]) > 50) ||
	//		  (abs(pDoc->m_fIout[i]-pDoc->m_fIout[i-1]) > 100) ||
	//		  (i == pDoc->m_nLastIdx-1) ||
	//		  bBAT_SHORT_EDGE ||
	//		  (no_log_cnt > 200)
	//		  )
	//	{
	//		ignore_cnt = 32;
	//		no_log_cnt = 0;
	//		s.Format( _T("%.2fV"), pDoc->m_fVout[i]);
	//		if (i == pDoc->m_nLastIdx-1)
	//		{
	//			s.AppendFormat( _T(" (VOUT) "));
	//			m_pDCMem->TextOutW( cur_x + 20,  cur_y - 7, s );
	//		}
	//		else if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}


	////--------------------------------------------------------------------------------
	//// draw RP
	////--------------------------------------------------------------------------------
	//clr = RGB(92, 105, 49);
	//m_pDCMem->SetTextColor( clr );
	//COLORREF clr_rp_diff;
	//clr_rp_diff = RGB(140, 155, 109);

	////CPen Pen_4(PS_SOLID, 1, clr);
	//CPen Pen_4(PS_DOT, 1, clr);
	//m_pDCMem->SelectObject(Pen_4);
	//fVoutZoom = max_curve_high /128;
	//cur_x = pt_base.x;
	//show_diff = 0.5;
	//no_log_cnt = 0;
	//last2 = 0;
	//diff_para = 0.02;
	//cur_y = pt_base.y - pDoc->m_fbuf[1][pDoc->m_nLastIdx+1] * fVoutZoom;
	//m_pDCMem->MoveTo(cur_x, cur_y);
	//int show_power_diff_cnt = 0;
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fbuf[1][i] * fVoutZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fbuf[1][i] * fVoutZoom);

	//	////------------------------------------------------------------
	//	//show_power_diff_cnt ++;
	//	//double power_diff  = pDoc->m_fbuf[1][i] * 39.5 - (pDoc->m_fVrect[i]*pDoc->m_fIout[i]);
	//	//if (((power_diff > 40) || (power_diff < -40)) && show_power_diff_cnt > 50)
	//	//{
	//	//	show_power_diff_cnt = 0;
	//	//	if (power_diff > 0)
	//	//		rect3 = CRect( cur_x, cur_y - power_diff, cur_x+2, cur_y - 5);
	//	//	else
	//	//			rect3 = CRect( cur_x, cur_y - power_diff, cur_x+2, cur_y + 5);
	//	//	m_pDCMem->FillSolidRect(&rect3, clr_rp_diff );
	//	//	m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	//}
	//	////------------------------------------------------------------


	//	cur_x ++;
	//	if (i==pDoc->m_nLastIdx+1)
	//		curr_diff = 10;
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fbuf[1][i+1] - pDoc->m_fbuf[1][i];

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fbuf[1][i] - pDoc->m_fbuf[1][i-2]);
	//	if ( //((ignore_cnt == 0) && pDoc->check_boundary(2, i, 0.3)) || 
	//		  no_log_cnt > 250 || 
	//		  abs(pDoc->m_fbuf[1][i]-pDoc->m_fbuf[1][i-1]) > 50
	//		  )
	//	{

	//		ignore_cnt = 32;
	//		s.Format( _T("%.0fmW"), pDoc->m_fbuf[1][i] * 39.5);
	//		if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		no_log_cnt =  0;
	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}

	//cur_y = pt_base.y - pDoc->m_fbuf[1][0] * fVoutZoom;
	//m_pDCMem->MoveTo(cur_x, cur_y);
	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	no_log_cnt ++;
	//	cur_y = pt_base.y - pDoc->m_fbuf[1][i] * fVoutZoom;
	//	m_pDCMem->LineTo(cur_x, pt_base.y - pDoc->m_fbuf[1][i] * fVoutZoom);
	//	cur_x ++;
	//	curr_diff = pDoc->m_fbuf[1][i] - pDoc->m_fbuf[1][i-1];

	//	if (i > 2)
	//		last_diff = (pDoc->m_fbuf[1][i] - pDoc->m_fbuf[1][i-2]);
	//	else if (i < 999)
	//		curr_diff = pDoc->m_fbuf[1][i+1] - pDoc->m_fbuf[1][i];

	//	if (i > 2 )
	//		last_diff = (pDoc->m_fbuf[1][i] - pDoc->m_fbuf[1][i-2]);
	//	if ( (ignore_cnt == 0 && i < pDoc->m_nLastIdx-20) && (
	//		  //pDoc->check_boundary(2, i, 0.3) || 
	//		  abs(pDoc->m_fbuf[1][i]-pDoc->m_fbuf[1][i-1]) > 50)
	//		  || (i == pDoc->m_nLastIdx-1)
	//		  || (no_log_cnt > 250)
	//		  )
	//	{
	//		ignore_cnt = 32;
	//		no_log_cnt = 0;
	//		s.Format( _T("%.0fmW"), pDoc->m_fbuf[1][i] * 39.5);
	//		if (i == pDoc->m_nLastIdx-1)
	//		{
	//			s.AppendFormat( _T(" (RP Power) "));
	//			m_pDCMem->TextOutW( cur_x + 20,  cur_y - 7, s );
	//		}
	//		else if (curr_diff > 0)
	//			m_pDCMem->TextOutW( cur_x - 10,  cur_y - 25, s );
	//		else
	//			m_pDCMem->TextOutW( cur_x - 10, cur_y - 25, s );

	//		rect1.SetRect(cur_x-2, cur_y-2, cur_x+2, cur_y+2);
	//		m_pDCMem->FillSolidRect(&rect1, clr );
	//		m_pDCMem->SetBkColor(RGB(255, 255, 255));
	//	}
	//	last2 = last_diff;
	//	last_diff = curr_diff;
	//	if (ignore_cnt)
	//		ignore_cnt --;
	//}




	//--------------------------------------------------------------------------------
	// draw Flags
	//--------------------------------------------------------------------------------
	COLORREF clr_flag[8];
	CPen Pen_flag[8];

	CString sStatus[8];
	sStatus[0].Format( _T("LDO_EN"));
	sStatus[1].Format( _T("I-LIM"));
	sStatus[2].Format( _T("CHG_DONE"));
	sStatus[3].Format( _T("BAT_PRE-CHG"));
	sStatus[4].Format( _T("BAT_SHORT"));
	sStatus[5].Format( _T("TS_REGULATE"));
	sStatus[6].Format( _T("TS_HOT"));
	sStatus[7].Format( _T("BAT_FULL"));

	clr_flag[0] = RGB(170, 175, 49);
	clr_flag[1] = RGB(30, 185, 199);
	clr_flag[2] = RGB(230, 155, 149);
	clr_flag[3] = RGB(230, 55, 229);
	clr_flag[4] = RGB(230, 155, 49);
	clr_flag[5] = RGB(0, 155, 49);
	clr_flag[6] = RGB(20, 85, 109);
	clr_flag[7] = RGB(20, 55, 249);
	CPoint pt_status_text( pt_base.x + 1010, pt_base.y + 20);

	//m_pDCMem->SelectObject(&font_bold); // save the current font
	for(int i=0; i<8; i++)
	{
		Pen_flag[i].CreatePen(PS_SOLID, 1, clr_flag[i]);
		m_pDCMem->SetTextColor( clr_flag[i] );
		m_pDCMem->TextOutW( pt_status_text.x,  pt_status_text.y + i*14, sStatus[i] );
	}
	//m_pDCMem->SelectObject(&font); // save the current font

	int FlagZoom = 5;
	cur_x = pt_base.x;
	show_diff = 0.5;
	no_log_cnt = 0;
	double last2 = 0;

	pt_base.y = pt_base.y + 60;

	int idx = pDoc->m_nLastIdx+1;
	//for(int i=pDoc->m_nLastIdx+1; i<1000; i++)
	for(int i=0; i<1000; i++)
	{
		if (idx >=1000)
			idx = 0;
		int flag = pDoc->m_status[idx];
		for(int bit=0; bit<8; bit++)
		{
			int bit_mask = 1 << bit;
			if (flag & bit_mask)
			{
				m_pDCMem->SelectObject(Pen_flag[bit]);
				m_pDCMem->MoveTo(cur_x, pt_status_text.y + bit*14 + 8);
				m_pDCMem->LineTo(cur_x, pt_status_text.y + bit*14 + 8 - FlagZoom);
			}
		}
		cur_x ++;
		idx++;
		if (idx == 1000)
			idx = 0;
	}

	//for(int i=0; i<pDoc->m_nLastIdx; i++)
	//{
	//	int flag = pDoc->m_status[i];
	//	for(int bit=0; bit<8; bit++)
	//	{
	//		int bit_mask = 1 << bit;
	//		if (flag & bit_mask)
	//		{
	//			m_pDCMem->SelectObject(Pen_flag[bit]);
	//			m_pDCMem->MoveTo(cur_x, pt_status_text.y + bit*14 + 8);
	//			m_pDCMem->LineTo(cur_x, pt_status_text.y + bit*14 + 8 -  FlagZoom);
	//		}
	//	}
	//	cur_x ++;
	//}


	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//s.Format( _T("%.1f s "), pDoc->m_fSec[pDoc->m_nLastIdx]);
	s.Format( _T("%.1f s "), pDoc->m_fSec[pDoc->m_nLastIdx-1]);
	m_pDCMem->SetTextColor( RGB(80, 80, 80) );
	m_pDCMem->TextOutW( pt_base.x + 1000,  pt_base.y - 54, s );
	int time_y = pt_base.y - 54;
	idx = pDoc->m_nLastIdx+1;
	cur_x = pt_base.x;
	//for(int i=0; i<1000; i++)
	for(int i=0; i<970; i++)
	{
		if (idx >= 1000-1)
			idx = 1;

		if (pDoc->m_status[idx] != pDoc->m_status[idx-1])
		{
			s.Format( _T(" %.0f s "), pDoc->m_fSec[idx]);
			m_pDCMem->TextOutW( cur_x - 20,  pt_base.y - 54, s );

			m_pDCMem->MoveTo(cur_x, pt_base.y-54 - 1);
			m_pDCMem->LineTo(cur_x, pt_base.y-54 - 8);
			idx += 40;
			cur_x+= 40;
			i+=40;
		}

		idx++;
		cur_x++;
	}



	GetClientRect(&rectLogClip);
	VERIFY(pDC->BitBlt(rectLogClip.left, rectLogClip.top, rectLogClip.Width(), rectLogClip.Height(),
		m_pDCMem, rectLogClip.left, rectLogClip.top, SRCCOPY));

}
// CWireless_RxView printing

BOOL CWireless_RxView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWireless_RxView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWireless_RxView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CWireless_RxView diagnostics

#ifdef _DEBUG
void CWireless_RxView::AssertValid() const
{
	CView::AssertValid();
}

void CWireless_RxView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWireless_RxDoc* CWireless_RxView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWireless_RxDoc)));
	return (CWireless_RxDoc*)m_pDocument;
}
#endif //_DEBUG


// CWireless_RxView message handlers

void CWireless_RxView::OnCommandI2ctest()
{
	//EnumerateBridgeBoard();
     HANDLE hEnum = EnumerateBridgeBoard();
     if (hEnum)

         {

                 INT_PTR nCount = GetBridgeBoardCount(hEnum);
                 //cout << nCount << " board(s) found!" << endl;
                 POSITION pos = GetFirstDeviceInfoPosition(hEnum);
                 while(pos)

                 {
                         PBRIDGEDEVICEINFO pInfo = GetNextDeviceInfo(hEnum,pos);
                         //cout << "nCapability = 0x" << hex << pInfo->nCapability << dec << endl; 
                         //cout << "nI2CCount = " << pInfo->nI2CCount << endl;
                         //cout << "VID = " << hex << pInfo->nVID << " PID = " << pInfo->nPID << dec << endl;
                         //cout << "Firmware Ver = " << pInfo->strFirmwareInfo << endl;
                         //cout << "Device Path = " << pInfo->strDevicePath << endl;
                         //cout << "Controller Name = " << pInfo->strControllerName << endl;
                         //cout << "Library Name = " << pInfo->strLibraryName << endl;
                         //cout << "Library Path = " << pInfo->strLibraryPath << endl;
                         //cout << "Vendor Name = " << pInfo->strVendorName << endl;
                         //cout << "*******************" << endl;
                         // no need to free PBRIDGEDEVICEINFO which get from GetNextDeviceInfo() 
                 }
                 FreeEnumerateBridgeBoard(hEnum); // free hEnum and related resource (include PBRIDGEDEVICEINFO)
         }
	INT_PTR nBus = 0;
	I2CSLAVEADDR slaveAddr;
	IRTI2CModule *iRTI2CModule;
	IRTBridgeBoard* iIRTBridgeBoard;
	iIRTBridgeBoard = ConnectToBridge( 0 );
	iRTI2CModule = iIRTBridgeBoard->GetI2CModule();
	iRTI2CModule->I2CScanSlaveDevice( nBus, &slaveAddr );

	I2CTRANSACTION tran;
	tran.nSlaveAddr = 0x22;
	tran.nCmd = 0x00;
	tran.nCmdSize = 0x03;
	tran.nBufferLength = 1;
	tran.pBuffer[0] = 0xff;
	int32_t nResult = iRTI2CModule->I2CHLWrite( nBus, &tran );
	CString s;
	s = Result2String( nResult );

}

void CWireless_RxView::OnBnClickedButtonTest2()
{
	OnCommandI2ctest();
}

BOOL CWireless_RxView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	return CView::OnEraseBkgnd(pDC);
}
