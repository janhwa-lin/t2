// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ATEView.cpp : implementation of the CATEView class
//


#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ATE.h"
#endif

#include "PropertiesDlg.h"
#include "ATEDoc.h"
#include "ATEView.h"

#include "MainFrm.h"
#include "ATEDoc.h"
#include "OutputWnd.h"

#include "visa.h"
#include <math.h>

#include "GlobalValue.h"
#include "GlobalFunction.h"

#include "inc/RTBridgeBoardCore.h"
#include "inc/RTI2CModule.h"

// copy files and folder
//------------------------------
#include "shlobj.h"
//#include "ShellOp.h"
#include "cshellfileop.h"
//------------------------------

//#define NO_DEVICE_SIM

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CATEView

IMPLEMENT_DYNCREATE(CATEView, CDHtmlViewSpec)

BEGIN_MESSAGE_MAP(CATEView, CDHtmlViewSpec)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CDHtmlViewSpec::OnFilePrint)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON_ADC_TEST1, &CATEView::OnButtonAdcTest1)
	ON_COMMAND(ID_BUTTON_TEST_3, &CATEView::OnButtonTest3)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CATEView)
	// by ID
	DHTML_EVENT_ONCHANGE(_T("sliderBar2"), On_i2c_write)

	DHTML_EVENT_ONCLICK(_T("btnEnd_id"), OnButtonEnd)
	DHTML_EVENT_ONCLICK(_T("browse"), OnButtonEnd)
	DHTML_EVENT_ONCLICK(_T("Test_Item_id"), OnClickFolder)
	DHTML_EVENT_ONCLICK(_T("btn_i2c_rd_reg"), On_i2c_read)

	DHTML_EVENT_ONCLICK(_T("btn_i2c_wr_reg"), On_i2c_write)

	DHTML_EVENT_ONCLICK(_T("btn_test_vbus_ovp"), On_test_vbus_ovp)
	DHTML_EVENT_ONCLICK(_T("btn_test_vbus_uvp"), On_test_vbus_uvp)
	DHTML_EVENT_ONCLICK(_T("btn_test_dac_cv"), On_test_dac_cv)
	DHTML_EVENT_ONCLICK(_T("btn_test_vx_ovp"), On_test_vx_ovp)
	DHTML_EVENT_ONCLICK(_T("btn_test_quick_cs_ocp"), On_test_quick_cs_ocp)
	DHTML_EVENT_ONCLICK(_T("btn_test_accurate_cs_ocp"), On_test_accurate_cs_ocp)
	DHTML_EVENT_ONCLICK(_T("btn_test_icsp_icsn"), On_test_icsp_icsn)

	DHTML_EVENT_ONCLICK(_T("btn_test_adc_cc1"), On_test_adc_cc1)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_cc2"), On_test_adc_cc2)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_cs"), On_test_adc_cs)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_vbus"), On_test_adc_vbus)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_vbrl"), On_test_adc_vbrl)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_vsys5"), On_test_adc_vsys5)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_ain1"), On_test_adc_ain1)
	//DHTML_EVENT_ONCLICK(_T("btn_test_adc_ain2"), On_test_adc_ain2_temp)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_ain2"), On_test_adc_ain2)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_vbp"), On_test_adc_vbp)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_vconn"), On_test_adc_vconn)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_ts"), On_test_adc_ts)
	DHTML_EVENT_ONCLICK(_T("btn_test_adc_csp"), On_test_adc_csp)

	DHTML_EVENT_ONCLICK(_T("id_gui_reg_search_btn"), On_gui_reg_search_btn)

	DHTML_EVENT_ONCLICK(_T("btn_start_dac_measure"), On_start_dac_test)
	DHTML_EVENT_ONCLICK(_T("btn_cancel_dac_measure"), On_cancel_dac_test)
  


	//  by class
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("btn"), On_html_btn)

	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("bit_value_sel"), OnBitValueChange)
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("bit_value_not_sel"), OnBitValueChange)

	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("btn btn-primary"), OnBitValueChange)
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("btn btn-default"), OnBitValueChange)

	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("btn_blue_sel"), OnBitModify)
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("btn_not_sel"), OnBitValueChange)
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("instrument_img"), OnInstrumentLink)

	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("postfoot"), OnButtonEnd)
	DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONMOUSEOVER, _T("postfoot2"), OnButtonEnd)
END_DHTML_EVENT_MAP()
// CATEView construction/destruction

CATEView::CATEView()
{
	// TODO: add construction code here
	m_bCopyFileDone = FALSE;

	OnInitTestItem();

	int last_addr = -1;
	int addr_idx = 0;
	for (int i = 0; i < 3000; i++)
	{
		if (g_reg[i].addr == 0)
		{
			g_reg_data[addr_idx++].addr = 0;
			break;
		}
		if (g_reg[i].addr != last_addr)
		{
			last_addr = g_reg[i].addr;
			g_reg_data[addr_idx].addr = g_reg[i].addr;
			//g_reg_data[addr_idx].value = 0x00000000;
			g_reg_data[addr_idx].value = 0x12345678;
			addr_idx++;
		}
	}

	//i2c_init();

}

CATEView::~CATEView()
{
}

BOOL CATEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDHtmlViewSpec::PreCreateWindow(cs);
}

void CATEView::OnInitialUpdate()
{

	SetRegisterAsDropTarget(FALSE);

	m_sTestDir.Format(_T("RT7800_test_report"));

	if (GetFileAttributes(m_sTestDir) == INVALID_FILE_ATTRIBUTES)
	{
		//CreateDirectory(m_sTestDir, NULL);
		copy_my_files();
	}

	//GotoUrl(_T("res/main/top_index.htm"));
	//GotoUrl(_T("res/main/test_report.html"));	// long time
	//GotoUrl(_T("res/main/main.htm"));
	//GotoUrl(_T("res/main/explore.htm"));
	//GotoUrl(_T("../ATE/res/main/explore.htm"));

#ifdef _DEBUG
	GotoUrl(_T("../ATE/RT7800_test_report/explore.htm"));
#else
	// release
	GotoUrl(_T("RT7800_test_report/explore.htm"));
#endif


	//update_all_gui_reg( _T(""));

	//Navigate2(_T("http://www.msdn.microsoft.com/visualc/"),NULL,NULL);
}


// CATEView printing


void CATEView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}


void CATEView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CATEView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

HRESULT CATEView::OnButtonEnd(IHTMLElement *pElement)
{
	UpdateData();
	CString str;
	str.Format(_T("value=%d\n"), m_iControl);
	AfxMessageBox(str);
	return S_OK;
}

// CATEView diagnostics

#ifdef _DEBUG
void CATEView::AssertValid() const
{
	CDHtmlViewSpec::AssertValid();
}

void CATEView::Dump(CDumpContext& dc) const
{
	CDHtmlViewSpec::Dump(dc);
}

CATEDoc* CATEView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CATEDoc)));
	return (CATEDoc*)m_pDocument;
}
#endif //_DEBUG


// CATEView message handlers


HRESULT CATEView::OnMouseOverElement(IHTMLElement *phtmlElement)
{
	IHTMLStyle *phtmlStyle;
	phtmlElement->get_style(&phtmlStyle);
	if (phtmlStyle)
	{
		VARIANT varColor;
		varColor.vt = VT_I4;
		varColor.lVal = 0xff0000;
		phtmlStyle->put_textDecorationUnderline(VARIANT_TRUE);
		phtmlStyle->put_color(varColor);
		phtmlStyle->put_cursor(L"hand");
		phtmlStyle->Release();

	}
	m_spCurrElement = phtmlElement;
	return S_OK;
}

HRESULT CATEView::OnMouseOutElement(IHTMLElement *phtmlElement)
{
	IHTMLStyle *phtmlStyle;
	phtmlElement->get_style(&phtmlStyle);
	if (phtmlStyle)
	{
		VARIANT varColor;
		varColor.vt = VT_I4;
		varColor.lVal = 0x000000;
		phtmlStyle->put_textDecorationUnderline(VARIANT_FALSE);
		phtmlStyle->put_color(varColor);
		phtmlStyle->Release();
	}
	m_spCurrElement = NULL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CATEView::ShowContextMenu(DWORD /*dwID*/, POINT *ppt, IUnknown* /*pcmdtReserved*/, IDispatch* /*pdispReserved*/)
{
	CMenu ctxMenu;

	m_strCtxFileName.Empty();
	if (m_spCurrElement != NULL)
	{

		CComBSTR bstrFileName;
		m_spCurrElement->get_innerText(&bstrFileName);
		if (bstrFileName)
			m_strCtxFileName = bstrFileName;
	}
	ctxMenu.LoadMenu(IDR_CONTEXT_MENU);

	ctxMenu.GetSubMenu(0)->EnableMenuItem(ID_CONTEXT_PROPERTIES, MF_BYCOMMAND |
		((m_spCurrElement == NULL) ? MF_GRAYED : MF_ENABLED));
	ctxMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, ppt->x, ppt->y, this, NULL);
	return S_OK;
}

void CATEView::OnContextBrowse()
{
	// TODO: Add your command handler code here
	OnBrowse(NULL);
	//OnContextProperties();
}

//void CATEView::OnContextExit() 
//{
//	// TODO: Add your command handler code here
//	EndDialog(IDOK);
//}

void CATEView::OnContextProperties()
{
	// TODO: Add your command handler code here
	ASSERT(!m_strCtxFileName.IsEmpty());
	CPropertiesDlg dlg;
	CString m_strCurrDir(_T(""));
	dlg.m_strFileName = m_strCurrDir + _T("\\") + m_strCtxFileName;
	dlg.DoModal();
}


HRESULT CATEView::OnBrowse(IHTMLElement* /*phtmlElement*/)
{
	BROWSEINFO bi;
	TCHAR szDisplayName[MAX_PATH];
	memset(&bi, 0x00, sizeof(bi));
	bi.hwndOwner = m_hWnd;
	bi.pszDisplayName = szDisplayName;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl)
	{
		SHGetPathFromIDList(pidl, szDisplayName);
		SetCurrentDirectory(szDisplayName);
		RefreshDir();

		// free the pidl
		CComPtr<IMalloc> spSHMalloc;
		if (FAILED(SHGetMalloc(&spSHMalloc)))
		{
			return S_FALSE;
		}
		if (spSHMalloc != NULL)
			spSHMalloc->Free(pidl);
	}

	return S_OK;
}

void CATEView::RefreshDir()
{
	CTempBuffer<TCHAR, MAX_PATH> strBuffer(MAX_PATH);
	strBuffer[0] = _T('\0');
	DWORD length = GetCurrentDirectory(MAX_PATH, strBuffer);


	if (length > MAX_PATH)
	{
		// We need to reallocate the string => MAX_PATH wasn't space enough.
		strBuffer.Reallocate(length);
		strBuffer[0] = _T('\0');
		GetCurrentDirectory(length, strBuffer);
	}

	m_strCurrDir = strBuffer;
	UpdateData(FALSE);

	CString strTable;
	strTable = _T("<TABLE border=1 cols=4 width=100%>\n")
		_T("<THEAD><TR><TD width=20>&nbsp;</TD><TD>File Name</TD><TD>Size</TD><TD>Modified</TD></TR></THEAD>");


	// recreate the filelist table
	CComPtr<IHTMLElement> spfileList;
	GetElementInterface(_T("fileList"), IID_IHTMLElement, (void **)&spfileList);

	if (spfileList == NULL)
		return;

	CString strFilter = m_strCurrDir + _T("\\*.*");
	WIN32_FIND_DATA finddata;
	HANDLE hFind = FindFirstFile(strFilter, &finddata);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	CWaitCursor cur;
	CString str, strDate;

	strTable.AppendFormat(_T("<style>\n")
		_T("tr {background-color:#ffffFF;}\n")
		_T("</style>\n")
		);

	strTable += _T("First name : <input type = \"text\" id=\"edit_id\" name = \"fname\" class=\"folder\" value=\"ab\"><br>");

	do
	{

		CTime t(finddata.ftLastWriteTime);
		strDate = t.Format(_T("%A, %B %d, %Y"));


		str.Format(_T("<TR><TD width=20><IMG src=%s.gif></TD><TD><SPAN class=%s>%s</SPAN></TD><TD>%lu</TD><TD>%s</TD></TR>"),
			((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _T("folder") : _T("file")),
			((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _T("folder") : _T("file")),
			finddata.cFileName, finddata.nFileSizeLow, (LPCTSTR)strDate);
		strTable += str;
	} while (FindNextFile(hFind, &finddata));
	strTable += _T("</TABLE>");
	FindClose(hFind);
	CComBSTR bstrTable = strTable;
	spfileList->put_innerHTML(bstrTable);
}

HRESULT CATEView::On_i2c_read(IHTMLElement *phtmlElement)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;
	CString sValue = GetItemValue(L"edit_i2c_rd_addr");
	sValue.Remove(_T('_'));
	DWORD32 addr = _tcstoul(sValue, 0, 16);

	int rdata = rt7800_rd(addr);

	s = bstr;
	s.Format(_T("read addr = %X, rdata=%02X"), addr, rdata);
	CComBSTR bstrTable = s;
	hResult = GetElement(L"i2c_rd_reg_result", &pElement2);
	pElement2->put_innerHTML(bstrTable);
	return S_OK;
}

HRESULT CATEView::On_i2c_write(IHTMLElement *phtmlElement)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;

		//CString itemtype = L"data_log";
		CString itemtype = L"Value";
		VARIANT value;
		BSTR bstrType = itemtype.AllocSysString();
		phtmlElement->getAttribute(bstrType, 0, &value);

	CString sValue = GetItemValue(L"edit_i2c_rd_addr");
	CString sWdata = GetItemValue(L"edit_i2c_wdata");
	sValue.Remove(_T('_'));
	DWORD32 addr = _tcstoul(sValue, 0, 16);
	int wdata = _tcstoul(sWdata, 0, 16);

	rt7800_wr(addr, wdata);

	s = bstr;
	s.Format(_T("I2C write addr = %X, wdata=0x%02X"), addr, wdata);
	CComBSTR bstrTable = s;
	hResult = GetElement(L"i2c_rd_reg_result", &pElement2);
	pElement2->put_innerHTML(bstrTable);
	return S_OK;
}

void CATEView::log_add(CString s)
{
	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	CATEDoc* pDoc = GetDocument();

	//pMainWnd->m_wndOutput.m_wndOutputBuild.ResetContent();
	pMainWnd->m_wndOutput.m_wndOutputBuild.AddString(s);

}


HRESULT CATEView::On_test_dac_cv(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GP");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("DAC_CV");

	m_nTestIdx = 7;

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;



	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];
	float vbus_i[6][512];
	float dac_cv_i[6][512];
	double last_tri_lev = 0;
	CString s(_T(""));


#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 4.0, 0.3);	// VCSP: 5V, 100mA
	//set_sm_off(viSM27);
	//set_sm_vi(viSM26, 6.0, dVbusIlim);	// VBUS: 6V, 200mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("GP"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 0.5, 1.0, _T("DAC_CV"));
	//------------------------------

	//set_scope_mode_normal();
	MyDelay(300);
#else

	float Ivbus_VBUS_4V[] = {
		0.010764, 0.010803, 0.010804, 0.010808, 0.010803, 0.010812, 0.010807, 0.010811, 0.010803, 0.010812, 0.010811, 0.010814, 0.010807, 0.010813, 0.010812, 0.010814, 0.010805, 0.010814, 0.010814, 0.010818, 0.010812, 0.010819, 0.010816, 0.010818, 0.010809, 0.010817, 0.010815, 0.010818, 0.010811,
		0.010818, 0.010812, 0.010813, 0.010803, 0.010814, 0.010813, 0.010818, 0.010813, 0.010821, 0.010818, 0.01082, 0.010812, 0.010821, 0.010819, 0.010824, 0.010816, 0.010823, 0.010819, 0.010819, 0.010809, 0.010819, 0.010818, 0.010822, 0.010817, 0.010824, 0.01082, 0.010822, 0.010813, 0.010821,
		0.010818, 0.010821, 0.010814, 0.010819, 0.010815, 0.010816, 0.010806, 0.010816, 0.010816, 0.010819, 0.010816, 0.010822, 0.010821, 0.010822, 0.010813, 0.010824, 0.010821, 0.010826, 0.010819, 0.010825, 0.010821, 0.010822, 0.010812, 0.010822, 0.010821, 0.010826, 0.010821, 0.010828, 0.010824,
		0.010826, 0.010818, 0.010825, 0.010824, 0.010826, 0.01082, 0.010825, 0.010821, 0.01082, 0.010811, 0.010821, 0.010821, 0.010826, 0.01082, 0.010827, 0.010824, 0.010826, 0.010819, 0.010827, 0.010825, 0.010828, 0.010821, 0.010828, 0.010824, 0.010825, 0.010815, 0.010825, 0.010823, 0.010827,
		0.010821, 0.010828, 0.010824, 0.010825, 0.010818, 0.010825, 0.010822, 0.010824, 0.010818, 0.010822, 0.010818, 0.010818, 0.010808, 0.010819, 0.010819, 0.010823, 0.010819, 0.010827, 0.010823, 0.010825, 0.010817, 0.010827, 0.010825, 0.010829, 0.010823, 0.01083, 0.010826, 0.010827, 0.010817,
		0.010826, 0.010826, 0.01083, 0.010825, 0.010832, 0.010828, 0.010831, 0.010822, 0.01083, 0.010828, 0.010831, 0.010824, 0.010829, 0.010826, 0.010827, 0.010816, 0.010826, 0.010826, 0.010832, 0.010826, 0.010833, 0.010831, 0.010833, 0.010825, 0.010833, 0.010831, 0.010834, 0.010828, 0.010834,
		0.010829, 0.010831, 0.010822, 0.010829, 0.010829, 0.010833, 0.010827, 0.010834, 0.010829, 0.010832, 0.010823, 0.010831, 0.010828, 0.010831, 0.010824, 0.010829, 0.010824, 0.010824, 0.010815, 0.010825, 0.010825, 0.01083, 0.010825, 0.010831, 0.010829, 0.010831, 0.010822, 0.010833, 0.010832,
		0.010835, 0.010828, 0.010834, 0.01083, 0.010831, 0.010822, 0.010831, 0.010831, 0.010834, 0.010828, 0.010835, 0.010832, 0.010833, 0.010825, 0.010833, 0.010831, 0.010834, 0.010826, 0.010832, 0.010828, 0.010829, 0.010818, 0.010829, 0.010828, 0.010833, 0.010826, 0.010834, 0.010831, 0.010833,
		0.010824, 0.010833, 0.010831, 0.010835, 0.010828, 0.010834, 0.010829, 0.01083, 0.010821, 0.01083, 0.010829, 0.010833, 0.010826, 0.010833, 0.010831, 0.010831, 0.010822, 0.010829, 0.010828, 0.01083, 0.010822, 0.010828, 0.010823, 0.010823, 0.010814, 0.010823, 0.010822, 0.010828, 0.010823,
		0.01083, 0.010828, 0.01083, 0.010822, 0.010831, 0.010829, 0.010834, 0.010827, 0.010834, 0.01083, 0.010831, 0.010822, 0.010831, 0.01083, 0.010834, 0.010829, 0.010836, 0.010832, 0.010834, 0.010826, 0.010832, 0.010832, 0.010835, 0.010828, 0.010833, 0.010829, 0.010829, 0.01082, 0.01083,
		0.01083, 0.010834, 0.010828, 0.010835, 0.010832, 0.010834, 0.010825, 0.010835, 0.010833, 0.010836, 0.01083, 0.010835, 0.010832, 0.010833, 0.010822, 0.010832, 0.010832, 0.010835, 0.010829, 0.010837, 0.010833, 0.010834, 0.010825, 0.010833, 0.01083, 0.010833, 0.010826, 0.010832, 0.010825,
		0.010826, 0.010816, 0.010827, 0.010825, 0.010831, 0.010826, 0.010834, 0.01083, 0.010832, 0.010824, 0.010833, 0.010832, 0.010836, 0.010829, 0.010835, 0.01083, 0.010832, 0.010823, 0.010832, 0.010832, 0.010835, 0.010829, 0.010836, 0.010833, 0.010834, 0.010826, 0.010834, 0.010831, 0.010834,
		0.010827, 0.010832, 0.010828, 0.010829, 0.010818, 0.010828, 0.010828, 0.010832, 0.010826, 0.010833, 0.010831, 0.010833, 0.010825, 0.010833, 0.010831, 0.010835, 0.010828, 0.010834, 0.01083, 0.010829, 0.01082, 0.010828, 0.010827, 0.010831, 0.010825, 0.010832, 0.010828, 0.010829, 0.01082,
		0.010828, 0.010824, 0.010828, 0.01082, 0.010826, 0.010819, 0.01082, 0.01081, 0.01082, 0.01082, 0.010825, 0.01082, 0.010828, 0.010824, 0.010827, 0.010819, 0.010828, 0.010825, 0.01083, 0.010823, 0.010829, 0.010826, 0.010826, 0.010817, 0.010827, 0.010827, 0.010831, 0.010825, 0.010831,
		0.010829, 0.01083, 0.010821, 0.01083, 0.010827, 0.010832, 0.010824, 0.010828, 0.010824, 0.010824, 0.010815, 0.010825, 0.010824, 0.01083, 0.010824, 0.010831, 0.010827, 0.010829, 0.010822, 0.01083, 0.010828, 0.010831, 0.010825, 0.01083, 0.010825, 0.010826, 0.010817, 0.010825, 0.010825,
		0.010829, 0.010823, 0.010829, 0.010825, 0.010827, 0.010819, 0.010826, 0.010823, 0.010827, 0.010818, 0.010825, 0.010819, 0.01082, 0.010809, 0.010819, 0.010819, 0.010824, 0.010819, 0.010825, 0.010823, 0.010825, 0.010818, 0.010826, 0.010824, 0.010828, 0.010822, 0.010828, 0.010823, 0.010825,
		0.010816, 0.010825, 0.010825, 0.010828, 0.010821, 0.010829, 0.010825, 0.010828, 0.010819, 0.010826, 0.010824, 0.010827, 0.010819, 0.010826, 0.01082, 0.010821, 0.010811, 0.010822, 0.01082, 0.010824, 0.010818, 0.010826, 0.010823, 0.010824, 0.010816, 0.010825, 0.010822, 0.010825, 0.010819,
		0.010825, 0.01082, 0.01082, 0.010809, 0.010819, 0.010818, 0.010822, 0.010815, 0.010822, 0.010818, 0.010819, 0.01081, 0.010819, 0.010816, 0.010817, 0.01081, 0.010815, 0.01081, 0.01081,
	};



	float Ivbus_VBUS_24V[] = {
		0.011529, 0.011571, 0.011571, 0.011577, 0.011572, 0.01158, 0.011577, 0.011581, 0.011573, 0.011583, 0.011581, 0.011585, 0.011578, 0.011585, 0.011583, 0.011585, 0.011576, 0.011587, 0.011586, 0.011589, 0.011582, 0.011591, 0.011589, 0.011591, 0.011581, 0.011591, 0.011589, 0.011592, 0.011584,
		0.01159, 0.011585, 0.011586, 0.011577, 0.011586, 0.011587, 0.011592, 0.011586, 0.011594, 0.011591, 0.011594, 0.011585, 0.011595, 0.011593, 0.011595, 0.011589, 0.011595, 0.011591, 0.011593, 0.011583, 0.011593, 0.011591, 0.011596, 0.011589, 0.011596, 0.011593, 0.011594, 0.011585, 0.011593,
		0.01159, 0.011594, 0.011587, 0.011593, 0.011588, 0.011588, 0.011579, 0.011589, 0.011589, 0.011595, 0.011589, 0.011596, 0.011593, 0.011596, 0.011587, 0.011597, 0.011596, 0.011599, 0.011593, 0.011599, 0.011595, 0.011596, 0.011586, 0.011596, 0.011595, 0.011601, 0.011594, 0.011601, 0.011599,
		0.0116, 0.011591, 0.011601, 0.011596, 0.011599, 0.011593, 0.011598, 0.011594, 0.011594, 0.011584, 0.011594, 0.011594, 0.011599, 0.011593, 0.0116, 0.011596, 0.011599, 0.011591, 0.0116, 0.011597, 0.011601, 0.011594, 0.0116, 0.011596, 0.011597, 0.011587, 0.011596, 0.011595, 0.0116,
		0.011593, 0.011599, 0.011596, 0.011598, 0.011588, 0.011598, 0.011596, 0.011597, 0.01159, 0.011596, 0.011591, 0.011591, 0.01158, 0.01159, 0.01159, 0.011596, 0.011591, 0.0116, 0.011596, 0.011598, 0.01159, 0.0116, 0.011598, 0.011601, 0.011595, 0.011602, 0.011598, 0.0116, 0.011591,
		0.011599, 0.011599, 0.011604, 0.011598, 0.011605, 0.011603, 0.011605, 0.011595, 0.011603, 0.011601, 0.011605, 0.011597, 0.011603, 0.011598, 0.011599, 0.011588, 0.011599, 0.011599, 0.011604, 0.011599, 0.011606, 0.011603, 0.011605, 0.011595, 0.011605, 0.011603, 0.011607, 0.011601, 0.011607,
		0.011602, 0.011603, 0.011593, 0.011604, 0.011603, 0.011606, 0.011601, 0.011607, 0.011604, 0.011605, 0.011596, 0.011604, 0.011602, 0.011606, 0.011597, 0.011603, 0.011597, 0.011598, 0.011588, 0.011598, 0.011599, 0.011604, 0.011596, 0.011604, 0.011602, 0.011605, 0.011597, 0.011606, 0.011603,
		0.011608, 0.011602, 0.011608, 0.011604, 0.011605, 0.011595, 0.011605, 0.011604, 0.011609, 0.011603, 0.01161, 0.011606, 0.011607, 0.011599, 0.011609, 0.011605, 0.011609, 0.011601, 0.011606, 0.011601, 0.011602, 0.011592, 0.011603, 0.011602, 0.011606, 0.011602, 0.011608, 0.011605, 0.011607,
		0.011599, 0.011607, 0.011604, 0.011608, 0.011602, 0.011607, 0.011602, 0.011603, 0.011594, 0.011603, 0.011602, 0.011607, 0.011601, 0.011607, 0.011603, 0.011604, 0.011595, 0.011603, 0.011601, 0.011603, 0.011596, 0.011601, 0.011596, 0.011596, 0.011586, 0.011597, 0.011596, 0.011602, 0.011596,
		0.011605, 0.011602, 0.011604, 0.011595, 0.011604, 0.011603, 0.011607, 0.011601, 0.011608, 0.011603, 0.011604, 0.011595, 0.011605, 0.011605, 0.011609, 0.011602, 0.011609, 0.011606, 0.011608, 0.0116, 0.011608, 0.011606, 0.011609, 0.0116, 0.011607, 0.011603, 0.011603, 0.011593, 0.011604,
		0.011603, 0.011608, 0.011602, 0.011609, 0.011606, 0.011609, 0.0116, 0.01161, 0.011608, 0.011612, 0.011604, 0.01161, 0.011605, 0.011606, 0.011596, 0.011606, 0.011606, 0.011611, 0.011603, 0.01161, 0.011607, 0.011608, 0.011598, 0.011608, 0.011605, 0.011608, 0.0116, 0.011606, 0.0116,
		0.0116, 0.01159, 0.0116, 0.0116, 0.011606, 0.011599, 0.011607, 0.011604, 0.011606, 0.011598, 0.011608, 0.011605, 0.011609, 0.011603, 0.011609, 0.011605, 0.011606, 0.011597, 0.011607, 0.011605, 0.011609, 0.011603, 0.01161, 0.011607, 0.011608, 0.0116, 0.011607, 0.011606, 0.011608,
		0.011601, 0.011607, 0.011602, 0.011602, 0.011592, 0.011601, 0.011601, 0.011606, 0.0116, 0.011607, 0.011605, 0.011607, 0.011598, 0.011606, 0.011604, 0.011609, 0.011601, 0.011607, 0.011602, 0.011602, 0.011594, 0.011602, 0.011602, 0.011605, 0.011599, 0.011605, 0.011601, 0.011603, 0.011594,
		0.011603, 0.0116, 0.011601, 0.011594, 0.011599, 0.011595, 0.011594, 0.011584, 0.011595, 0.011594, 0.011599, 0.011593, 0.011602, 0.011598, 0.0116, 0.011593, 0.011601, 0.011602, 0.011604, 0.011597, 0.011604, 0.0116, 0.011601, 0.011591, 0.011601, 0.011601, 0.011605, 0.011598, 0.011606,
		0.011602, 0.011603, 0.011595, 0.011603, 0.011601, 0.011605, 0.011597, 0.011603, 0.011598, 0.011598, 0.011588, 0.011599, 0.011599, 0.011604, 0.011598, 0.011604, 0.011602, 0.011605, 0.011595, 0.011604, 0.011602, 0.011605, 0.011598, 0.011604, 0.011599, 0.011602, 0.01159, 0.011602, 0.0116,
		0.011603, 0.011598, 0.011605, 0.011601, 0.011602, 0.011593, 0.011602, 0.011599, 0.011602, 0.011594, 0.011599, 0.011593, 0.011595, 0.011584, 0.011595, 0.011595, 0.0116, 0.011594, 0.011601, 0.011598, 0.0116, 0.011592, 0.0116, 0.011598, 0.011602, 0.011595, 0.011602, 0.011597, 0.011598,
		0.011589, 0.011599, 0.011597, 0.011602, 0.011597, 0.011603, 0.011599, 0.011601, 0.011591, 0.0116, 0.011598, 0.011602, 0.011594, 0.011598, 0.011594, 0.011594, 0.011584, 0.011594, 0.011593, 0.011598, 0.011591, 0.0116, 0.011596, 0.011597, 0.011589, 0.011597, 0.011595, 0.011599, 0.011592,
		0.011598, 0.011592, 0.011593, 0.011583, 0.011592, 0.011592, 0.011595, 0.01159, 0.011597, 0.011592, 0.011594, 0.011584, 0.011592, 0.01159, 0.011594, 0.011585, 0.011591, 0.011585, 0.011584,
	};


	float Idac_cv_VBUS_4V[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000001, 0.000002, 0.000002, 0.000003, 0.000004, 0.000004, 0.000005, 0.000006, 0.000007, 0.000007, 0.000008, 0.000009, 0.000009, 0.00001,
		0.000011, 0.000012, 0.000012, 0.000013, 0.000014, 0.000014, 0.000015, 0.000016, 0.000017, 0.000017, 0.000018, 0.000019, 0.000019, 0.00002, 0.000021, 0.000022, 0.000022, 0.000023, 0.000024, 0.000024, 0.000025, 0.000026, 0.000026, 0.000027, 0.000028, 0.000029, 0.000029, 0.00003, 0.000031,
		0.000031, 0.000032, 0.000033, 0.000034, 0.000034, 0.000035, 0.000036, 0.000036, 0.000037, 0.000038, 0.000039, 0.000039, 0.00004, 0.000041, 0.000041, 0.000042, 0.000043, 0.000044, 0.000044, 0.000045, 0.000046, 0.000046, 0.000047, 0.000048, 0.000049, 0.000049, 0.00005, 0.000051, 0.000051,
		0.000052, 0.000053, 0.000054, 0.000054, 0.000055, 0.000056, 0.000056, 0.000057, 0.000058, 0.000059, 0.000059, 0.00006, 0.000061, 0.000061, 0.000062, 0.000063, 0.000064, 0.000064, 0.000065, 0.000066, 0.000066, 0.000067, 0.000068, 0.000069, 0.000069, 0.00007, 0.000071, 0.000071, 0.000072,
		0.000073, 0.000073, 0.000074, 0.000075, 0.000076, 0.000076, 0.000077, 0.000078, 0.000078, 0.000079, 0.00008, 0.000081, 0.000081, 0.000082, 0.000082, 0.000083, 0.000084, 0.000085, 0.000085, 0.000086, 0.000087, 0.000087, 0.000088, 0.000089, 0.00009, 0.00009, 0.000091, 0.000092, 0.000092,
		0.000093, 0.000094, 0.000095, 0.000095, 0.000096, 0.000097, 0.000097, 0.000098, 0.000099, 0.0001, 0.0001, 0.000101, 0.000102, 0.000102, 0.000103, 0.000104, 0.000105, 0.000105, 0.000106, 0.000107, 0.000107, 0.000108, 0.000109, 0.00011, 0.00011, 0.000111, 0.000112, 0.000112, 0.000113,
		0.000114, 0.000114, 0.000115, 0.000116, 0.000117, 0.000117, 0.000118, 0.000119, 0.000119, 0.00012, 0.000121, 0.000122, 0.000122, 0.000123, 0.000124, 0.000124, 0.000125, 0.000126, 0.000127, 0.000127, 0.000128, 0.000129, 0.000129, 0.00013, 0.000131, 0.000132, 0.000132, 0.000133, 0.000134,
		0.000134, 0.000135, 0.000136, 0.000137, 0.000137, 0.000138, 0.000139, 0.000139, 0.00014, 0.000141, 0.000142, 0.000142, 0.000143, 0.000144, 0.000144, 0.000145, 0.000146, 0.000147, 0.000147, 0.000148, 0.000149, 0.000149, 0.00015, 0.000151, 0.000152, 0.000152, 0.000153, 0.000154, 0.000154,
		0.000155, 0.000156, 0.000157, 0.000157, 0.000158, 0.000159, 0.000159, 0.00016, 0.000161, 0.000161, 0.000162, 0.000163, 0.000164, 0.000164, 0.000165, 0.000166, 0.000166, 0.000167, 0.000168, 0.000169, 0.000169, 0.00017, 0.000171, 0.000171, 0.000172, 0.000173, 0.000173, 0.000174, 0.000175,
		0.000175, 0.000176, 0.000177, 0.000178, 0.000178, 0.000179, 0.00018, 0.00018, 0.000181, 0.000182, 0.000182, 0.000183, 0.000184, 0.000185, 0.000185, 0.000186, 0.000187, 0.000187, 0.000188, 0.000189, 0.00019, 0.00019, 0.000191, 0.000192, 0.000192, 0.000193, 0.000194, 0.000195, 0.000195,
		0.000196, 0.000197, 0.000197, 0.000198, 0.000199, 0.0002, 0.0002, 0.000201, 0.000202, 0.000202, 0.000203, 0.000204, 0.000205, 0.000205, 0.000206, 0.000207, 0.000207, 0.000208, 0.000209, 0.00021, 0.00021, 0.000211, 0.000212, 0.000212, 0.000213, 0.000214, 0.000215, 0.000215, 0.000216,
		0.000217, 0.000217, 0.000218, 0.000219, 0.00022, 0.00022, 0.000221, 0.000222, 0.000222, 0.000223, 0.000224, 0.000225, 0.000225, 0.000226, 0.000227, 0.000227, 0.000228, 0.000229, 0.000229, 0.00023, 0.000231, 0.000232, 0.000232, 0.000233, 0.000234, 0.000234, 0.000235, 0.000236, 0.000237,
		0.000237, 0.000238, 0.000239, 0.000239, 0.00024, 0.000241, 0.000242, 0.000242, 0.000243, 0.000244, 0.000244, 0.000245, 0.000246, 0.000247, 0.000247, 0.000248, 0.000249, 0.000249, 0.00025, 0.000251, 0.000252, 0.000252, 0.000253, 0.000254, 0.000254, 0.000255, 0.000256, 0.000257, 0.000257,
		0.000258, 0.000259, 0.000259, 0.00026, 0.000261, 0.000262, 0.000262, 0.000263, 0.000263, 0.000264, 0.000265, 0.000266, 0.000266, 0.000267, 0.000268, 0.000269, 0.000269, 0.00027, 0.000271, 0.000271, 0.000272, 0.000273, 0.000273, 0.000274, 0.000275, 0.000276, 0.000276, 0.000277, 0.000278,
		0.000278, 0.000279, 0.00028, 0.000281, 0.000281, 0.000282, 0.000283, 0.000283, 0.000284, 0.000285, 0.000286, 0.000286, 0.000287, 0.000288, 0.000288, 0.000289, 0.00029, 0.000291, 0.000291, 0.000292, 0.000293, 0.000293, 0.000294, 0.000295, 0.000296, 0.000296, 0.000297, 0.000298, 0.000298,
		0.000299, 0.0003, 0.000301, 0.000301, 0.000302, 0.000303, 0.000303, 0.000304, 0.000305, 0.000306, 0.000306, 0.000307, 0.000308, 0.000308, 0.000309, 0.00031, 0.000311, 0.000311, 0.000312, 0.000313, 0.000313, 0.000314, 0.000315, 0.000316, 0.000316, 0.000317, 0.000318, 0.000318, 0.000319,
		0.00032, 0.00032, 0.000321, 0.000322, 0.000323, 0.000323, 0.000324, 0.000325, 0.000325, 0.000326, 0.000327, 0.000328, 0.000328, 0.000329, 0.00033, 0.000331, 0.000331, 0.000332, 0.000333, 0.000333, 0.000334, 0.000335, 0.000336, 0.000336, 0.000337, 0.000338, 0.000338, 0.000339, 0.00034,
		0.000341, 0.000341, 0.000342, 0.000343, 0.000343, 0.000344, 0.000345, 0.000345, 0.000346, 0.000347, 0.000348, 0.000348, 0.000349, 0.00035, 0.00035, 0.000351, 0.000352, 0.000353, 0.000353,
	};

	float Idac_cv_VBUS_24V[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000001, 0.000002, 0.000002, 0.000003, 0.000004, 0.000004, 0.000005, 0.000006, 0.000007, 0.000007, 0.000008, 0.000009, 0.000009, 0.00001,
		0.000011, 0.000012, 0.000012, 0.000013, 0.000014, 0.000014, 0.000015, 0.000016, 0.000017, 0.000017, 0.000018, 0.000019, 0.000019, 0.00002, 0.000021, 0.000022, 0.000022, 0.000023, 0.000024, 0.000024, 0.000025, 0.000026, 0.000027, 0.000027, 0.000028, 0.000029, 0.000029, 0.00003, 0.000031,
		0.000032, 0.000032, 0.000033, 0.000034, 0.000034, 0.000035, 0.000036, 0.000037, 0.000037, 0.000038, 0.000039, 0.000039, 0.00004, 0.000041, 0.000042, 0.000042, 0.000043, 0.000044, 0.000044, 0.000045, 0.000046, 0.000047, 0.000047, 0.000048, 0.000049, 0.000049, 0.00005, 0.000051, 0.000052,
		0.000052, 0.000053, 0.000054, 0.000054, 0.000055, 0.000056, 0.000057, 0.000057, 0.000058, 0.000059, 0.000059, 0.00006, 0.000061, 0.000062, 0.000062, 0.000063, 0.000064, 0.000064, 0.000065, 0.000066, 0.000067, 0.000067, 0.000068, 0.000069, 0.000069, 0.00007, 0.000071, 0.000072, 0.000072,
		0.000073, 0.000074, 0.000074, 0.000075, 0.000076, 0.000077, 0.000077, 0.000078, 0.000079, 0.000079, 0.00008, 0.000081, 0.000081, 0.000082, 0.000083, 0.000083, 0.000084, 0.000085, 0.000086, 0.000086, 0.000087, 0.000088, 0.000088, 0.000089, 0.00009, 0.000091, 0.000091, 0.000092, 0.000093,
		0.000093, 0.000094, 0.000095, 0.000096, 0.000096, 0.000097, 0.000098, 0.000098, 0.000099, 0.0001, 0.000101, 0.000101, 0.000102, 0.000103, 0.000103, 0.000104, 0.000105, 0.000106, 0.000106, 0.000107, 0.000108, 0.000108, 0.000109, 0.00011, 0.000111, 0.000111, 0.000112, 0.000113, 0.000113,
		0.000114, 0.000115, 0.000116, 0.000116, 0.000117, 0.000118, 0.000118, 0.000119, 0.00012, 0.000121, 0.000121, 0.000122, 0.000123, 0.000123, 0.000124, 0.000125, 0.000126, 0.000126, 0.000127, 0.000128, 0.000128, 0.000129, 0.00013, 0.000131, 0.000131, 0.000132, 0.000133, 0.000133, 0.000134,
		0.000135, 0.000136, 0.000136, 0.000137, 0.000138, 0.000138, 0.000139, 0.00014, 0.000141, 0.000141, 0.000142, 0.000143, 0.000143, 0.000144, 0.000145, 0.000146, 0.000146, 0.000147, 0.000148, 0.000148, 0.000149, 0.00015, 0.000151, 0.000151, 0.000152, 0.000153, 0.000153, 0.000154, 0.000155,
		0.000156, 0.000156, 0.000157, 0.000158, 0.000158, 0.000159, 0.00016, 0.000161, 0.000161, 0.000162, 0.000163, 0.000163, 0.000164, 0.000165, 0.000166, 0.000166, 0.000167, 0.000168, 0.000168, 0.000169, 0.00017, 0.000171, 0.000171, 0.000172, 0.000172, 0.000173, 0.000174, 0.000175, 0.000175,
		0.000176, 0.000177, 0.000177, 0.000178, 0.000179, 0.00018, 0.00018, 0.000181, 0.000182, 0.000182, 0.000183, 0.000184, 0.000185, 0.000185, 0.000186, 0.000187, 0.000187, 0.000188, 0.000189, 0.00019, 0.00019, 0.000191, 0.000192, 0.000192, 0.000193, 0.000194, 0.000195, 0.000195, 0.000196,
		0.000197, 0.000197, 0.000198, 0.000199, 0.0002, 0.0002, 0.000201, 0.000202, 0.000202, 0.000203, 0.000204, 0.000205, 0.000205, 0.000206, 0.000207, 0.000207, 0.000208, 0.000209, 0.00021, 0.00021, 0.000211, 0.000212, 0.000212, 0.000213, 0.000214, 0.000215, 0.000215, 0.000216, 0.000217,
		0.000217, 0.000218, 0.000219, 0.00022, 0.00022, 0.000221, 0.000222, 0.000222, 0.000223, 0.000224, 0.000225, 0.000225, 0.000226, 0.000227, 0.000227, 0.000228, 0.000229, 0.00023, 0.00023, 0.000231, 0.000232, 0.000232, 0.000233, 0.000234, 0.000235, 0.000235, 0.000236, 0.000237, 0.000237,
		0.000238, 0.000239, 0.00024, 0.00024, 0.000241, 0.000242, 0.000242, 0.000243, 0.000244, 0.000245, 0.000245, 0.000246, 0.000247, 0.000247, 0.000248, 0.000249, 0.00025, 0.00025, 0.000251, 0.000252, 0.000252, 0.000253, 0.000254, 0.000255, 0.000255, 0.000256, 0.000257, 0.000257, 0.000258,
		0.000259, 0.00026, 0.00026, 0.000261, 0.000262, 0.000262, 0.000263, 0.000264, 0.000264, 0.000265, 0.000266, 0.000267, 0.000267, 0.000268, 0.000269, 0.000269, 0.00027, 0.000271, 0.000272, 0.000272, 0.000273, 0.000274, 0.000274, 0.000275, 0.000276, 0.000277, 0.000277, 0.000278, 0.000279,
		0.000279, 0.00028, 0.000281, 0.000282, 0.000282, 0.000283, 0.000284, 0.000284, 0.000285, 0.000286, 0.000287, 0.000287, 0.000288, 0.000289, 0.000289, 0.00029, 0.000291, 0.000292, 0.000292, 0.000293, 0.000294, 0.000294, 0.000295, 0.000296, 0.000297, 0.000297, 0.000298, 0.000299, 0.000299,
		0.0003, 0.000301, 0.000302, 0.000302, 0.000303, 0.000304, 0.000304, 0.000305, 0.000306, 0.000307, 0.000307, 0.000308, 0.000309, 0.000309, 0.00031, 0.000311, 0.000312, 0.000312, 0.000313, 0.000314, 0.000314, 0.000315, 0.000316, 0.000317, 0.000317, 0.000318, 0.000319, 0.000319, 0.00032,
		0.000321, 0.000322, 0.000322, 0.000323, 0.000324, 0.000324, 0.000325, 0.000326, 0.000327, 0.000327, 0.000328, 0.000329, 0.000329, 0.00033, 0.000331, 0.000332, 0.000332, 0.000333, 0.000334, 0.000334, 0.000335, 0.000336, 0.000337, 0.000337, 0.000338, 0.000339, 0.000339, 0.00034, 0.000341,
		0.000342, 0.000342, 0.000343, 0.000344, 0.000344, 0.000345, 0.000346, 0.000347, 0.000347, 0.000348, 0.000349, 0.000349, 0.00035, 0.000351, 0.000352, 0.000352, 0.000353, 0.000354, 0.000354,
	};

	for (int i = 0; i < 512; i++)
	{
		vbus_i[0][i] = Ivbus_VBUS_4V[i];
		vbus_i[5][i] = Ivbus_VBUS_24V[i];
		dac_cv_i[0][i] = Idac_cv_VBUS_4V[i];
		dac_cv_i[5][i] = Idac_cv_VBUS_24V[i];
	}

#endif


	for (int i = 4; i <= 24; i += 4)
		s.AppendFormat(_T("Ivbus_@VBUS=%dV,"), i);
	for (int i = 4; i <= 24; i += 4)
		s.AppendFormat(_T("Idac_cv_@VBUS=%dV,"), i);
	s += _T("\r\n");
	nTestCnt = 0;
	for (int vbus_idx = 0; vbus_idx < 6; vbus_idx += 1)
	{
		float vbus = vbus_idx * 4 + 4;
		set_sm_vi(viSM25, vbus, dVbusIlim);
		set_sm_on(viSM25);

		for (int i = 0; i < 512; i++)
		{
			nTestCnt++;
			CString sText;
			sText.Format(_T("DAC_CV[8_0]=0x%03X"), i);

#ifndef NO_DEVICE_SIM
			//MyDelay(5);
			wdata1 = i & 0xFF;
			wdata2 = (i >> 8) & 0xFF;
			rt7800_wr(0x40004070, wdata1, wdata2);

			get_sm_voltage(viSM25);
			get_sm_current(viSM25);
			get_sm_voltage(viSM27);
			get_sm_current(viSM27);

			vbus_i[vbus_idx][i] = m_dSourceMeterI[0];
			dac_cv_i[vbus_idx][i] = m_dSourceMeterI[2];

			nTestCnt = 512;
#endif
			update_test_progress(99 * ((vbus_idx + 1) * 512 + i) / 6 / 512 + 1);
			//update_test_progress(100 * i / 512 + 1);

			MSG msg;
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				DispatchMessage(&msg);

		}

	}


	s.Format(_T(""));
	for (int i = 4; i <= 24; i += 4)
		s.AppendFormat(_T("Ivbus_VBUS=%dV,"), i);
	for (int i = 4; i <= 24; i += 4)
		s.AppendFormat(_T("Idac_cv_VBUS=%dV,"), i);
	s += _T("\n");
	for (int i = 0; i < 512; i++)
	{
		for (int vbus_idx = 0; vbus_idx < 6; vbus_idx += 1)
			s.AppendFormat(_T("%f,"), vbus_i[vbus_idx][i]);
		for (int vbus_idx = 0; vbus_idx < 6; vbus_idx += 1)
			s.AppendFormat(_T("%f,"), dac_cv_i[vbus_idx][i]);
		s += _T("\n");
	}

	CStdioFile file;
	CString sFile2 = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	sFile2.AppendFormat(_T("protect_dac_cv.csv"));

#ifndef NO_DEVICE_SIM
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
	}
	file.WriteString(s);
	file.Close();
	int debug = 1;
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	float X_label[512];
	for (int i = 0; i < 512; i++)
		X_label[i] = i;

	nTestCnt = 512;
	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("DAC_CV[8:0]"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}uA"), _T("uA") },
		//{ _T("{value}uA"), _T("△ uA") },
		//{ _T("{value}uA"), _T("diff uA") },
		{ _T(""), _T("") }	// end marker
	};

	for (int i = 0; i < 6; i++)
	for (int j = 0; j < 512; j++)
		dac_cv_i[i][j] = dac_cv_i[i][j] * 1E6;

	float diff[512];
	for (int i = 0; i < 512; i++)
		diff[i] = dac_cv_i[5][i] - dac_cv_i[0][i];
	HC_X_AXIS Xaxis[] = {
		{ _T("IDAC_CV_VBUS=4V"), 0, dac_cv_i[0], _T("uA") },
		//{ _T("IDAC_CV_VBUS=8V"), 0, dac_cv_i[1], _T("uA") },
		//{ _T("IDAC_CV_VBUS=12V"), 0, dac_cv_i[2], _T("uA") },
		//{ _T("IDAC_CV_VBUS=16V"), 0, dac_cv_i[3], _T("uA") },
		//{ _T("IDAC_CV_VBUS=20V"), 0, dac_cv_i[4], _T("uA") },
		{ _T("IDAC_CV_VBUS=24V"), 0, dac_cv_i[5], _T("uA") },
		//{ _T("difference"), 1, diff, _T("△ uA") },
		//{ _T("difference"), 1, diff, _T("diff uA") },
		{ _T(""), 0, NULL, _T("") }
	};




	//------------------------------------------------------------
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	sFile2 = sTestItemDir;
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	//update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	//------------------------------------------------------------
	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);


	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_cc1(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("CC1");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");

	update_test_progress(0);
	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC CC1"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CC1");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[12000];
	for (int j = 0; j < 12000; j++)
		//nResult[j] = 0;
		nResult[j] = j;

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[12000];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[12000];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM26, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM26, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("CC1"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 2.7; testv += 0.001)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;
		//sText.Format(_T("DAC_CV[8_0]=0x%03X"), i);

		// ADC stable check
		//------------------------------
		if (testv == 1)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004028) & 0x3FF;

		}
		//------------------------------

		for (int loop = 0; loop < 4; loop++)
		{
			adc_code = rt7800_rd2(0x40004028) & 0x3FF;
			int adc_code1 = rt7800_rd(0x40004028) & 0x3FF;
			nResult[nTestCnt] = adc_code;

			nTestCnt++;

			MSG msg;
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				DispatchMessage(&msg);
		}

		update_test_progress(99 * testv / 2.7);
	}

#endif
	update_test_progress(100);

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

	for (int i = 0; i <= 2700; i++)
	{
		for (int j = 0; j < 4; j++)
			s.AppendFormat(_T("%.3f,%d\n"), (float)i*0.001, nResult[i * 4 + j]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

	//if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	//{
	//	MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
	//}
	//file.WriteString(s);
	//file.Close();

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


#ifdef NO_DEVICE_SIM
	nTestCnt = 28;
	for (int i = 0; i < 28; i++)
		X_label[i] = i * 0.1;
#endif

	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[12000];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------



	sCurve.Format(_T("<div id=\"id_result_%s\">  ")
		_T("<script src=\"waveform/ADC CC1/gen_result_adc_cc1.js\"></script>")
		_T("</div>\n"), (LPCTSTR)sTestNameID);

	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//save_html_file(m_sTop, _T("sub_test_VBUS_OVP.html"));
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	return true;

}

HRESULT CATEView::On_test_adc_cc2(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("CC1");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC CC2"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CC2");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 30 * sin((float)0.1*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("CC2"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 2.7; testv += 0.1)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		CString sText;

		MyDelay(50);

		// ADC stable check
		//------------------------------
		if (testv == 1)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x4000402C) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x4000402C) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 2.7);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif
	update_test_progress(100);

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

	for (int i = 0; i <= 27; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i*0.1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

	//if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	//{
	//	MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
	//}
	//file.WriteString(s);
	//file.Close();

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


#ifdef NO_DEVICE_SIM
	nTestCnt = 28;
	for (int i = 0; i < 28; i++)
		X_label[i] = i * 0.1;
#endif

	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);



	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------



	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	return true;

}

HRESULT CATEView::On_test_adc_ain1(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("AIN1");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg;
	for (int i = 0; i < 10; i++)
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC AIN1"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("AIN1");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 30 * sin((float)0.1*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("AIN1"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 2.7; testv += 0.1)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 1)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004040) & 0x3FF;

		}
		//------------------------------
		adc_code = rt7800_rd2(0x40004040) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 2.7);

		nTestCnt++;
		//MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

	for (int i = 0; i <= 27; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i*0.1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

	//if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	//{
	//	MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
	//}
	//file.WriteString(s);
	//file.Close();

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


#ifdef NO_DEVICE_SIM
	nTestCnt = 28;
	for (int i = 0; i < 28; i++)
		X_label[i] = i * 0.1;
#endif

	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------


	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_ain2_temp(IHTMLElement *pElement)
{
	m_bChamberMode = true;
	CString sProgress;
	set_chamber(-25);
	MyDelay(60000);
	for (double temp = -25; temp <= 125; temp += 30)
	{
		//update_html_div(_T("id_test_progress_time"), sProgress)
		m_dTemperature = temp;
		set_chamber(temp);
		update_test_progress(0);
		MyDelay(40000);
		On_test_adc_ain2(pElement);
	}
	set_chamber(25);
	MyDelay(5*60000);
	update_test_progress(100);
	m_bChamberMode = false;
	return S_OK;
}
HRESULT CATEView::On_test_adc_ain2(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("AIN2");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC AIN2"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("AIN2");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[21800];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 39 * sin((float)0.18*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[21800];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	//	set_scope_time(time_offset, time_scale);
	//	set_scope_trigger(2, false, 2.5);
	//	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	//	set_scope_channel(2, true, 5, -5.0, _T("AIN2"));
	//	set_scope_channel(3, true, 1, 1.5, _T(""));
	//	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 2.7; testv += 0.001)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		//X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 1)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004044) & 0x3FF;

		}
		//------------------------------
		for (int loop = 0; loop < 4; loop++)
		{
			adc_code = rt7800_rd2(0x40004044) & 0x3FF;
			nResult[nTestCnt] = adc_code;

			update_test_progress(99 * testv / 5.4);
			nTestCnt++;

		}
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}
	for (float testv = 2.7; testv >= 0; testv -= 0.001)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		//X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;

		for (int loop = 0; loop < 4; loop++)
		{
			adc_code = rt7800_rd2(0x40004044) & 0x3FF;
			nResult[nTestCnt] = adc_code;

			update_test_progress(99 * (2.7 - testv) / 5.4 + 50);
			nTestCnt++;
		}
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

	int buf_idx = 0;
	for (int i = 0; i <= 2700; i++)
	{
		for (int j = 0; j < 4; j++)
			s.AppendFormat(_T("%.3f,%d\n"), (float)i*0.001, nResult[buf_idx++]);
	}
	for (int i = 2700; i >= 0; i--)
	{
		for (int j = 0; j < 4; j++)
			s.AppendFormat(_T("%.3f,%d\n"), (float)i*0.001, nResult[buf_idx++]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();


	if (m_bChamberMode)
	{
		sTestFileTag.AppendFormat(_T("_temperature=%.0foC"), m_dTemperature);
	}

	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

	//if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	//{
	//	MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
	//}
	//file.WriteString(s);
	//file.Close();

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------

	//return S_OK;

#ifdef NO_DEVICE_SIM
	nTestCnt = 28;
	for (int i = 0; i < 28; i++)
		X_label[i] = i * 0.1;
#endif

	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------


	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_vsys5(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("VSYS5");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC VSYS5"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VSYS5");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 30 * sin((float)0.3*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("VSYS5"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 6; testv += 0.25)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 3)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x4000403C) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x4000403C) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 6);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 0.25;
#endif

	for (int i = 0; i <= nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i*0.25, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------

	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_ts(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("TS");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC TS"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("TS");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 380 * cos((float)0.23*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("TS"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------

	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);

	float StableResult[1000];
	for (float testv = 0; testv <= 6; testv += 0.25)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		CString sText;
		MyDelay(50);


		// ADC stable check
		//------------------------------
		if (testv == 3)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004050) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x40004050) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 6);

		nTestCnt++;
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 0.25;
#endif

	for (int i = 0; i <= nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i*0.25, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------



	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_vconn(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("VCONN");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC VCONN"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VCONN");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 300 * cos((float)0.12*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("VCONN"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 6; testv += 0.25)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 3)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x4000404C) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x4000404C) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 6);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 0.25;
#endif

	for (int i = 0; i <= nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i*0.25, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------


	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_vbrl(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("VBRL");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC VBRL"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VBRL");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 30 * sin((float)0.3*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.5;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("VBRL"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 24; testv += 1)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;

		// ADC stable check
		//------------------------------
		if (testv == 10)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004038) & 0x3FF;

		}
		//------------------------------


		adc_code = rt7800_rd2(0x40004038) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 24);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 1;
#endif

	for (int i = 0; i < nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i * 1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------



	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_vbp(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("VBP");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC VBP"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VBP");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 30 * sin((float)0.3*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("VBP"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 24; testv += 1)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 10)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004048) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x40004048) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 24);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 1;
#endif

	for (int i = 0; i < nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i * 1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------

	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_csp(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("CSP");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC CSP"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CSP");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 10 * sin((float)0.9*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("CSP"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM27, 0, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 24; testv += 1)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 10)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004054) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x40004054) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 24);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 1;
#endif

	for (int i = 0; i < nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i * 1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------


	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_cs(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("CSP");
	m_sChName[2] = _T("CSN");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC CS"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CSP");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float adc_code[20][300];
	float icsn[13][300];	// e-load i
	float vcsn[13][300];	// scope CH3
	float icsp[13][300];	// sm27 i
	float vcsp[13][300];	// scope CH2

	for (int i = 0; i < 13; i++)
	for (int j = 0; j < 250; j++)
	{
		adc_code[i][j] = 100 * sin((float)0.1*i) + j - 30 * cos((float)0.29*j);
		icsn[i][j] = 100 * sin((float)0.1*i) + j - 30 * cos((float)0.29*j);
		vcsn[i][j] = 10 * sin((float)0.31*i) + j - 100 * cos((float)0.89*j);
		icsp[i][j] = 30 * sin((float)0.19*i) + j + 30 * cos((float)0.79*j);
		vcsp[i][j] = 30 * sin((float)0.13*i) + j - 80 * cos((float)0.19*j);
	}

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;

	double dVbusIlim = 0.3;
	int GP_discharge = 0;
	float dMeterResult[300];
	MSG msg;
	int v_idx = 0;
	int i_idx = 0;
	CString sText;
	int temp;

	int i_idx_total = 0;

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload(0);
	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VBUS
	set_sm_vi(viSM27, 2, 0.3);		// CSP
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);
	set_eload_on();

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 1E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_mode_auto();
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 10, -5.0, _T("CSP"));
	set_scope_channel(3, true, 5, 1.5, _T("CSN"));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------


	set_sm_vi(viSM27, 2, dVbusIlim);
	set_sm_on(viSM27);
	MyDelay(300);

	i_idx = 0;
	v_idx = 0;
	//for (float testv=2; testv<=24; testv+=0.1)
	float StableResult[1000];
	for (float testv = 2; testv <= 24; testv += 0.5)
	{
		set_sm_vi(viSM27, testv, dVbusIlim);
		for (float load = 0; load <= 60E-3; load += 5E-3)
		{

			if (load == 0)
				X_label[v_idx] = testv;
			set_eload(load);
			//MyDelay(20);
			MyDelay(10);


			// ADC stable check
			//------------------------------
			if (testv == 10 && load == 20E-3)
			{
				for (int cnt = 0; cnt < 1000; cnt++)
					StableResult[cnt] = rt7800_rd2(0x40004030) & 0x3FF;

			}
			//------------------------------

			temp = 0;
			for (int acc = 0; acc < 3; acc++)
				temp = max((rt7800_rd2(0x40004030) & 0x3FF), temp);

			adc_code[i_idx][v_idx] = (float)temp;

			double dEload_i = get_eload_current();
			icsn[i_idx][v_idx] = dEload_i;
			if (dEload_i > 0.1)
			{
				ASSERT(false);
			}
			icsp[i_idx][v_idx] = get_sm_current(viSM27);
			set_scope_measure(2);
			vcsp[i_idx][v_idx] = m_Meas.dAverage;
			set_scope_measure(3);
			vcsn[i_idx][v_idx] = m_Meas.dAverage;

			update_test_progress(99 * testv / 24);
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				DispatchMessage(&msg);
			nTestCnt++;
			i_idx++;
		}
		i_idx_total = i_idx;
		i_idx = 0;
		v_idx++;

	}

#endif


#ifdef NO_DEVICE_SIM
	v_idx = 231;
	i_idx = 13;
	for (int i = 0; i < v_idx; i++)
		X_label[i] =  (float) 2+ i*0.1;
#endif

	s = _T("ICSP,ICSN,VCSP,VCSN,ADC_CODE[9:0]\n");
	for (int i = 0; i < i_idx_total; i++)
	for (int j = 0; j < v_idx; j++)
	{
		s.AppendFormat(_T("%.6f,%.6f,%.6f,%.6f,%.0f\n")
			, icsp[i][j]
			, icsn[i][j]
			, vcsp[i][j]
			, vcsn[i][j]
			, adc_code[i][j]
			);
	}



	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		v_idx,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("Eload=10uA") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis[] = {
		{ _T("Eload=5mA"), 0, adc_code[1], _T("") },
		{ _T("Eload=20mA"), 0, adc_code[4], _T("") },
		{ _T("Eload=30mA"), 0, adc_code[6], _T("") },
		{ _T("Eload=40mA"), 0, adc_code[8], _T("") },
		{ _T("Eload=50mA"), 0, adc_code[10], _T("") },
		{ _T("Eload=60mA"), 0, adc_code[12], _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------


	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_adc_vbus(IHTMLElement *pElement)
{
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("VX");
	m_sChName[2] = _T("");
	m_sChName[3] = _T("");
	update_test_progress(0);
	MSG msg2;
	for (int i = 0; i < 10; i++)
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);

	m_nTestIdx = 7;
	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].sCmd.MakeUpper() == _T("ADC VBUS"))
		{
			m_nTestIdx = i;
			break;
		}
		else if (i == m_nTotalTestItemCnt - 1)
		{
			ASSERT(FALSE);
			return S_OK;
		}


	}

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	int nResult[100];
	for (int j = 0; j < 100; j++)
		//nResult[j] = 0;
		nResult[j] = j + 10 * cos((float)0.83*j);

	CString sWave(_T(""));
	CString sImgFileName;


	float X_label[512];
	double last_tri_lev = 0;
	CString s;
	int adc_code;

	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);
	set_sm_vi(viSM27, 5.0, 0.3);	// VX: 5V, 300mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("VX"));
	set_scope_channel(3, true, 1, 1.5, _T(""));
	set_scope_channel(4, true, 0.5, 1.0, _T(""));
	//------------------------------



	set_sm_vi(viSM25, 0, dVbusIlim);
	MyDelay(300);
	float StableResult[1000];
	for (float testv = 0; testv <= 24; testv += 1)
	{
		set_sm_vi(viSM25, testv, dVbusIlim);
		X_label[nTestCnt] = testv;
		MyDelay(30);
		CString sText;


		// ADC stable check
		//------------------------------
		if (testv == 10)
		{
			for (int cnt = 0; cnt < 1000; cnt++)
				StableResult[cnt] = rt7800_rd2(0x40004034) & 0x3FF;

		}
		//------------------------------

		adc_code = rt7800_rd2(0x40004034) & 0x3FF;
		nResult[nTestCnt] = adc_code;

		update_test_progress(99 * testv / 24);
		nTestCnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			DispatchMessage(&msg);

	}

#endif

	CString sV = m_TestList[m_nTestIdx].sCmd + _T(" (V)");
	s.Format(_T("%s,%s[9:0]\n")
		, (LPCTSTR)sV
		, (LPCTSTR)m_TestList[m_nTestIdx].sCmd
		);

#ifdef NO_DEVICE_SIM
	nTestCnt = 25;
	for (int i = 0; i < 25; i++)
		X_label[i] = i * 1;
#endif

	for (int i = 0; i < nTestCnt; i++)
	{
		s.AppendFormat(_T("%.1f,%d\n"), (float)i * 1, nResult[i]);
	}

	//CStdioFile file;
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");

	save_text_file(sFile2, s);

#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM27);
#endif

	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------


	CString sX_Title = m_TestList[m_nTestIdx].sCmd + _T(" voltage");
	HC_OUTLINE ol = {
		sTestFileTag,
		sX_Title,
		nTestCnt,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};

	CString sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	float fResult[100];
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = (float)nResult[i];
	HC_X_AXIS Xaxis[] = {
		{ sY_title, 0, fResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);


	//--------------------------------------------------------------------------------
	// stable check
	//--------------------------------------------------------------------------------
	float stable_label[1000];
	for (int i = 0; i < 1000; i++)
		stable_label[i] = i;
	HC_OUTLINE ol2 = {
		_T("ADC Stable check"),
		sX_Title,
		1000,
		stable_label,
		true
	};
	HC_Y_AXIS Yaxis2[] = {
		{ _T("{value}"), _T("code") },
		{ _T(""), _T("") }	// end marker
	};
	sY_title = m_TestList[m_nTestIdx].sCmd + _T(" code");
	HC_X_AXIS Xaxis2[] = {
		{ sY_title, 0, StableResult, _T("") },
		{ _T(""), 0, NULL, _T("") }
	};
	CString sTestNameID2 = sTestNameID + _T("_stable");
	sCurve = gen_js_chart(sTestNameID2, ol2, Yaxis2, Xaxis2);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T("_stable.js");
	save_text_file(sFile2, sCurve);
	//--------------------------------------------------------------------------------

	sCurve.Format(_T("<div id=\"id_result_%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//--------------------------------------------------------------------------------

	m_sTop.Append(sCurve);
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	save_html_file(m_sTop, sTestFile);

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);
#endif

	update_test_progress(100);
	return true;

}

HRESULT CATEView::update_html_div(CString sID, CString sContent)
{
	//--------------------------------------------------------------------------------
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	HRESULT hResult = GetElement((LPCTSTR)sID, &pElement2);
	if (hResult != S_OK)
		return hResult;
	bstr = sContent;
	pElement2->put_outerHTML(bstr);
	//--------------------------------------------------------------------------------
	return hResult;
}

int CATEView::FindStringLine(CString s, CString* sLine, int* ValueMap)
{
	int nLine = 0;
	int p1 = 0;
	int p2;
	CString sTemp = s;
	ValueMap[0] = 0;
	if (s.Left(2) == _T("0:"))
	{
		sLine[0] = _T("");
		nLine = 1;
	}
	while (p1 != -1)
	{
		p1 = sTemp.Find(_T("\r\n"));
		if (p1 != -1)
		{
			sLine[nLine] = sTemp.Left(p1);
			sTemp = sTemp.Mid(p1 + 2);
			nLine++;
		}
		else
			sLine[nLine] = sTemp;
	}
	nLine++;
	for (int i = 0; i < nLine; i++)
	{
		p1 = sLine[i].Find(_T(":"));
		if (p1 == -1)
			ValueMap[i] = -1;
		else
		{
			sTemp = sLine[i].Left(p1 - 1);
			ValueMap[i] = wcstoull(sTemp, 0, 2);
		}
	}
	return nLine;

}

HRESULT CATEView::On_test_vbus_ovp(IHTMLElement *pElement)
{
	update_all_gui_reg(_T(""));
	return S_OK;




	int addr;
	CString s;
	update_test_progress(0);
#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GC2A");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 2;
	log_add(_T("test: VBUS OVP"));

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	struct { LPCTSTR sText; double dOffset; double dTimeScale; } arrZoom[] =
	{
		{ _T("500ms"), 16E-3, 500E-3 },
		{ _T("50ms"), 0, 50E-3 },
		{ _T("50ms(2)"), 16E-3, 50E-3 },
		{ _T("10ms"), 0, 10E-3 },
		{ _T("5ms"), 16E-3, 5E-3 },
		{ _T("200us"), 0, 200E-6 },
		{ _T("10us"), 0, 1E-5 },
		{ _T("10us(2)"), 78E-6, 10E-6 },
		{ _T(""), -1, -1 },
	};
	int nZoomCnt;
	for (int i = 0; i < 1000; i++)
	if (arrZoom[i].dTimeScale < 0)
	{
		nZoomCnt = i;
		break;
	}

	struct _VBUS_OV_SEL_
	{
		LPCTSTR  sLev;
		int		reg;
	};
	_VBUS_OV_SEL_ vbus_ov[] =
	{
		{ _T("6V"), 0x40 },
		{ _T("7V"), 0x4A },
		{ _T("8V"), 0x55 },
		{ _T("9V"), 0x60 },
		{ _T("10V"), 0x6A },
		{ _T("11V"), 0x75 },
		{ _T("12V"), 0x7F },
		{ _T("13V"), 0x8A },
		{ _T("14V"), 0x95 },
		{ _T("15V"), 0x9F },
		{ _T("16V"), 0xAA },
		{ _T("17V"), 0xB5 },
		{ _T("18V"), 0xBF },
		{ _T("19V"), 0xCA },
		{ _T("20V"), 0xD4 },
		{ _T("21V"), 0xDF },
		{ _T("22V"), 0xEA },
		{ _T("23V"), 0xF4 },
		{ _T("24V"), 0xFF },
		{ _T(""), 0x0 },
	};



	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VBUS: 5V, 100mA
	//set_sm_off(viSM27);
	set_sm_vi(viSM26, 5.0, dVbusIlim);	// VCSP: 5V, 200mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	rt7800_wr(0x40004094, 0xF0, 0x03);
	rt7800_wr(0x400040A0, 0x02);	//BUSOV_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//BUSOV_RST = 0
	rt7800_wr(0x4000902C, 0x02);
	rt7800_wr(0x4000902C, 0x82);	//GP_Charge = 1, GP_CNTL = 0, GP_BUS_OV_EN = 1

	GP_discharge = rt7800_rd(0x4000902C);
	if (GP_discharge != 0x82)
	{
		ASSERT(false);
		return S_OK;
	}

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 10, 10.0, _T("GC2A"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	set_scope_mode_normal();
	MyDelay(300);
#endif

	double last_tri_lev = 0;
	for (int i = 0; i < 1000; i++)
	{
		if (vbus_ov[i].sLev == _T(""))
			break;
		nTestCnt++;
		CString sText;
		sText.Format(_T("VBUS_OV[7_0]=0x%02X target=%s"), vbus_ov[i].reg, (LPCTSTR)vbus_ov[i].sLev);

#ifndef NO_DEVICE_SIM
		set_sm_on(viSM25);
		MyDelay(50);
		wdata1 = ((vbus_ov[i].reg & 0x3F) << 2);
		wdata2 = ((vbus_ov[i].reg & 0xc0) >> 6);
		rt7800_wr(0x4000902C, 0x02);
		rt7800_wr(0x40004094, wdata1, wdata2);	// VBUS_OV[7:0]
		rt7800_wr(0x400040A0, 0x02);			// BUSOV_RST = 1
		rt7800_wr(0x400040A0, 0x00);			// BUSOV_RST = 0
		rt7800_wr(0x4000902C, 0x02);			// GC2a_Charge=1
		rt7800_wr(0x4000902C, 0x82);			// GC2a_Charge=1, GC2a_BUSOV_EN = 1

		set_scope_annotation(1, sText);
		is_scope_triggered();
		set_scope_capture_single();
		MyDelay(200);
#endif


		sImgFileName = sText;

		for (double dVbusLev = 4; dVbusLev <= 24; dVbusLev += 0.1)
		{
			if (dVbusLev == 23)
			{
				ASSERT(false);
				MessageBox(_T("Can't find GC2A falling edge"), _T("Error"), MB_OK | MB_ICONERROR);
			}
			if (dVbusLev < last_tri_lev - 2)
				dVbusLev = last_tri_lev - 2;

#ifndef NO_DEVICE_SIM
			set_sm_vi(viSM25, dVbusLev, dVbusIlim);
			MyDelay(10);
			GP_discharge = rt7800_rd(0x4000902C) & 0x01;
#else
			GP_discharge = 1;
#endif
			if (GP_discharge)
			{

				MyDelay(300);
				last_tri_lev = dVbusLev;
#ifndef NO_DEVICE_SIM
				if (!is_scope_triggered())
				{
					set_scope_mode_auto();
					set_scope_capture_run();
					MyDelay(50);
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
					set_scope_mode_normal();
				}
				else
				{
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
				}

				dMeterResult[i] = dVbusLev;
				fResult[i] = (m_dCH[0][1] + m_dCH[0][0]) / 2;

				get_sm_voltage(viSM25);
				get_sm_current(viSM25);
				//get_sm_voltage(viSM26);
				//get_sm_current(viSM26);
				get_sm_voltage(viSM27);
				get_sm_current(viSM27);

				get_eload_voltage();
				get_eload_current();


				//sText.Format(_T("VBUS_OV[7_0]=0x%02X target=%s"), vbus_ov[i].reg, (LPCTSTR) vbus_ov[i].sLev);
				sText.AppendFormat(_T(" resoult=%.2fV"), dVbusLev);
				set_scope_annotation(1, sText);
				save_scope_img(sImgFileName);
#endif


				sImgFileName.AppendFormat(_T(".png"));

#ifndef NO_DEVICE_SIM
				save_scope_data(1, sImgFileName);
				save_scope_data(2, sImgFileName);
				save_scope_data(3, sImgFileName);
				save_scope_data(4, sImgFileName);
#endif

				update_test_progress(100 * i / 19 + 1);
				sWave += gen_html_waveform(sImgFileName, i + 1, 19);
				//MyDelay(30);
				MSG msg;
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
					DispatchMessage(&msg);

#ifndef NO_DEVICE_SIM
				set_sm_vi(viSM25, 4.0, dVbusIlim);
				set_sm_off(viSM25);
#endif
				break;
			}
		}

	}


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	float X_label[80];
	float fTarget[50];
	float fDiff[50];

#ifdef NO_DEVICE_SIM
	nTestCnt = 19;
	float ftemp[] = { 6.20, 7.10, 8.20, 9.20, 10.20, 11.20, 12.20, 13.20, 14.20, 15.20, 16.20, 17.30, 18.20, 19.20, 20.20, 21.10, 21.10, 21.10, 21.10 };
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = ftemp[i];
#endif

	for (int i = 0; i < nTestCnt; i++)
		X_label[i] = (float)vbus_ov[i].reg;

	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("VBUS_OV[7:0]"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T("{value}delta V"), _T("Voltage") },
		{ _T(""), _T("") }	// end marker
	};

	for (int i = 0; i < 50; i++)
		fTarget[i] = i + 6;

	for (int i = 0; i < nTestCnt; i++)
		fDiff[i] = fTarget[i] - dMeterResult[i];

	HC_X_AXIS Xaxis[] = {
		{ _T("Target"), 0, fTarget, _T("V") },
		{ _T("VBUS_OVP_scope"), 0, fResult, _T("V") },
		{ _T("VBUS_OVP_SM"), 0, dMeterResult, _T("V") },
		{ _T("error"), 1, fDiff, _T("delta V") },
		{ _T(""), 0, NULL, _T("") }
	};

	//------------------------------
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	s = _T("Target,VBUS_OV[7:0],VBUS_OV (v)\n");
	for (int i = 0; i < nTestCnt; i++)
		s.AppendFormat(_T("%s,0x%02X,%.3f\n"), (LPCTSTR)vbus_ov[i].sLev, vbus_ov[i].reg, dMeterResult[i]);
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------


	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop += sCurve;
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//sJS_Title.MakeLower();
	//CString sTestFile = _T("sub_test_") + sJS_Title + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return true;

}

HRESULT CATEView::On_test_vx_ovp(IHTMLElement *pElement)
{


#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VX");
	m_sChName[1] = _T("GC1");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 2;

	m_sSourceMeterName[0] = _T("");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	struct _VBUS_OV_SEL_
	{
		LPCTSTR  sLev;
		int		reg;
	};
	_VBUS_OV_SEL_ vbus_ov[] =
	{
		{ _T("6V"), 0x40 },
		{ _T("7V"), 0x4A },
		{ _T("8V"), 0x55 },
		{ _T("9V"), 0x60 },
		{ _T("10V"), 0x6A },
		{ _T("11V"), 0x75 },
		{ _T("12V"), 0x7F },
		{ _T("13V"), 0x8A },
		{ _T("14V"), 0x95 },
		{ _T("15V"), 0x9F },
		{ _T("16V"), 0xAA },
		{ _T("17V"), 0xB5 },
		{ _T("18V"), 0xBF },
		{ _T("19V"), 0xCA },
		{ _T("20V"), 0xD4 },
		{ _T("21V"), 0xDF },
		{ _T("22V"), 0xEA },
		{ _T("23V"), 0xF4 },
		{ _T("24V"), 0xFF },
		{ _T(""), 0x0 },
	};



	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);
	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);
	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);

	set_sm_off(viSM26);
	set_sm_vi(viSM26, 6.0, dVbusIlim);	// VBUS: 5V, 100mA
	set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_on(viSM26);
	set_sm_on(viSM27);

	MyDelay(200);
	rt7800_wr(0x40004090, 0xFC, 0x03);
	rt7800_wr(0x400040A0, 0x01);	//BRLOV_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//BRLOV_RST = 0
	rt7800_wr(0x40009028, 0x02);	//GC1_Charge = 1
	rt7800_wr(0x40009028, 0x42);	//GC1_Charge = 1, GC1_BRLOV_EN = 1

	GP_discharge = rt7800_rd(0x40009028);
	if (GP_discharge != 0x42)
	{
		ASSERT(false);
		return S_OK;
	}

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 10.0, _T("VX"));
	set_scope_channel(2, true, 10, 10.0, _T("GC1"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	set_scope_mode_normal();
	MyDelay(300);
#endif

	double last_tri_lev = 0;
	for (int i = 0; i < 1000; i++)
	{
		if (vbus_ov[i].sLev == _T(""))
			break;
		nTestCnt++;
		CString sText;
		sText.Format(_T("VBRL_OV[7_0]=0x%02X target=%s"), vbus_ov[i].reg, (LPCTSTR)vbus_ov[i].sLev);

#ifndef NO_DEVICE_SIM
		set_sm_on(viSM26);
		MyDelay(50);
		wdata1 = ((vbus_ov[i].reg & 0x3F) << 2);
		wdata2 = ((vbus_ov[i].reg & 0xc0) >> 6);
		rt7800_wr(0x40009028, 0x02);
		rt7800_wr(0x40004094, wdata1, wdata2);	// VBUS_OV[7:0]
		rt7800_wr(0x400040A0, 0x02);			// BUSOV_RST = 1
		rt7800_wr(0x400040A0, 0x00);			// BUSOV_RST = 0
		rt7800_wr(0x40009028, 0x02);			// GC1_Charge=1
		rt7800_wr(0x40009028, 0x42);			// GC1_Charge=1, GC1_BRLOV_EN = 1

		set_scope_annotation(1, sText);
		is_scope_triggered();
		set_scope_capture_single();
		MyDelay(200);
#endif


		sImgFileName = sText;

		for (double dVbusLev = 4; dVbusLev <= 24; dVbusLev += 0.1)
		{
			if (dVbusLev == 23)
			{
				ASSERT(false);
				MessageBox(_T("Can't find GC1 falling edge"), _T("Error"), MB_OK | MB_ICONERROR);
			}
			if (dVbusLev < last_tri_lev - 2)
				dVbusLev = last_tri_lev - 2;

#ifndef NO_DEVICE_SIM
			set_sm_vi(viSM26, dVbusLev, dVbusIlim);
			MyDelay(10);
			GP_discharge = rt7800_rd(0x40009028) & 0x01;
#else
			GP_discharge = 1;
#endif
			if (GP_discharge)
			{

				MyDelay(300);
				last_tri_lev = dVbusLev;
#ifndef NO_DEVICE_SIM
				if (!is_scope_triggered())
				{
					set_scope_mode_auto();
					set_scope_capture_run();
					MyDelay(50);
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
					set_scope_mode_normal();
				}
				else
				{
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
				}

				dMeterResult[i] = dVbusLev;
				fResult[i] = (m_dCH[0][1] + m_dCH[0][0]) / 2;

				get_sm_voltage(viSM26);
				get_sm_current(viSM26);
				//get_sm_voltage(viSM27);
				//get_sm_current(viSM27);

				//get_eload_voltage();
				//get_eload_current();


				//sText.Format(_T("VBUS_OV[7_0]=0x%02X target=%s"), vbus_ov[i].reg, (LPCTSTR) vbus_ov[i].sLev);
				sText.AppendFormat(_T(" resoult=%.2fV"), dVbusLev);
				set_scope_annotation(1, sText);
				save_scope_img(sImgFileName);
#endif


				sImgFileName.AppendFormat(_T(".png"));

#ifndef NO_DEVICE_SIM
				save_scope_data(1, sImgFileName);
				save_scope_data(2, sImgFileName);
				save_scope_data(3, sImgFileName);
				save_scope_data(4, sImgFileName);
#endif

				update_test_progress(100 * i / 19 + 1);
				sWave += gen_html_waveform(sImgFileName, i + 1, 19);
				//MyDelay(30);
				MSG msg;
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
					DispatchMessage(&msg);

#ifndef NO_DEVICE_SIM
				set_sm_vi(viSM26, 4.0, dVbusIlim);
				set_sm_off(viSM26);
#endif
				break;
			}
		}

	}


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	float X_label[80];
	float fTarget[50];
	float fDiff[50];

#ifdef NO_DEVICE_SIM
	nTestCnt = 19;
	float ftemp[] = { 6.20, 7.10, 8.20, 9.20, 10.20, 11.20, 12.20, 13.20, 14.20, 15.20, 16.20, 17.30, 18.20, 19.20, 20.20, 21.10, 21.10, 21.10, 21.10 };
	for (int i = 0; i < nTestCnt; i++)
		fResult[i] = ftemp[i];
#endif

	for (int i = 0; i < nTestCnt; i++)
		X_label[i] = (float)vbus_ov[i].reg;

	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("VBRL_OV[7:0]"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T("{value}delta V"), _T("Voltage") },
		{ _T(""), _T("") }	// end marker
	};

	for (int i = 0; i < 50; i++)
		fTarget[i] = i + 6;

	for (int i = 0; i < nTestCnt; i++)
		fDiff[i] = fTarget[i] - dMeterResult[i];

	HC_X_AXIS Xaxis[] = {
		{ _T("Target"), 0, fTarget, _T("V") },
		{ _T("VBRL_OVP_scope"), 0, fResult, _T("V") },
		{ _T("VBRL_OVP_SM"), 0, dMeterResult, _T("V") },
		{ _T("error"), 1, fDiff, _T("delta V") },
		{ _T(""), 0, NULL, _T("") }
	};

	//------------------------------
	CString s(_T(""));
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	s = _T("Target,VBRL_OV[7:0],VBUS_OV (v)\n");
	for (int i = 0; i < nTestCnt; i++)
		s.AppendFormat(_T("%s,0x%02X,%.3f\n"), (LPCTSTR)vbus_ov[i].sLev, vbus_ov[i].reg, dMeterResult[i]);
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------


	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop += sCurve;
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//sJS_Title.MakeLower();
	//CString sTestFile = _T("sub_test_") + sJS_Title + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return S_OK;

}


HRESULT CATEView::On_test_quick_cs_ocp(IHTMLElement *pElement)
{


#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GP");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 2;

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CSP");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	double dVbusIlim = 0.3;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 10.0, _T("VBUS"));
	set_scope_channel(2, true, 5, 5, _T("GP"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);
	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);
	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);

	set_sm_vi(viSM25, 5.0, dVbusIlim);	// VBUS: 5V, 100mA
	set_sm_vi(viSM26, 24, 0.3);		// CSP: 24V
	//set_sm_vi(viSM27, 0.75, 0.2);	
	set_sm_on(viSM25);
	set_sm_on(viSM26);
	//set_sm_on(viSM27);
	set_eload(0);
	set_eload_on();

	MyDelay(200);

	//0x4000_4060[5:4] = 2'b11 (DB_QK[1:0])
	//0x4000_4060[15:14] = 2'b11 (VCS_OC2[1:0])
	rt7800_wr(0x40004060, 0x30, 0xC0);

	rt7800_wr(0x400040A0, 0xFF);	//VOC_QK_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//VOC_QK_RST = 0
	rt7800_wr(0x40009034, 0x02);	//GP_Charge = 1
	rt7800_wr(0x40009034, 0x02, 0x04);	//GP_Charge = 1, QP_OCQK_EN = 1

	GP_discharge = rt7800_rd(0x40009034);
	if (GP_discharge != 0x02)
	{
		ASSERT(false);
		return S_OK;
	}

	set_scope_mode_normal();
	MyDelay(300);
#endif

	int vcsoc_idx = 0;
	int icsn_idx = 0;

	float vcs_buf[4][24];
	float icsn_buf[4][24];
	int vcs_oc2;
	CString sText;
	float X_label[80];
	float fTarget[50];
	float fDiff[50];
	int x_label_idx = 0;
	int vcsp_idx = 0;
	for (int i = 0; i < 4; i++)
	{
		vcsp_idx = 0;
		for (double vcsp = 24; vcsp >= 2; vcsp -= 1)
		{
			if (i == 0)
				X_label[x_label_idx++] = vcsp;
			vcs_oc2 = i;
			sText.Format(_T("VCS_OC2=0x%01X, CSP=%.1fV target=%dmV"), vcs_oc2, vcsp, vcs_oc2 * 50 + 50);

			set_sm_off(viSM25);
			set_sm_on(viSM25);
			set_sm_vi(viSM26, vcsp, dVbusIlim);
			set_sm_on(viSM26);
			set_eload(0);
			MyDelay(50);
			wdata1 = 0x30;
			wdata2 = (vcs_oc2 << 6);
			rt7800_wr(0x40004060, wdata1, wdata2);	// VCS_OC2[1:0]
			rt7800_wr(0x400040A0, 0x40);	//VOC_QK_RST = 1
			rt7800_wr(0x400040A0, 0x00);	//VOC_QK_RST = 0
			rt7800_wr(0x40009034, 0x02);	//GP_Charge = 1
			rt7800_wr(0x40009034, 0x02, 0x04);	//GP_Charge = 1, GP_OCQK_EN = 1

			GP_discharge = rt7800_rd(0x40009034);

			set_scope_annotation(1, sText);
			if (GP_discharge & 0x01)
			{
				set_scope_mode_auto();
			}
			else
			{
				set_scope_mode_normal();
				is_scope_triggered();
				set_scope_capture_single();
			}

			sImgFileName = sText;
			MyDelay(10);
			for (float icsn = 0; icsn <= 0.5; icsn += 0.005)
			{
				ASSERT(icsn < 0.5);

				set_eload(icsn);
				MyDelay(1);

				GP_discharge = rt7800_rd(0x40009034) & 0x01;
				if (GP_discharge)
				{

					MyDelay(30);
					set_scope_measure(3);
					set_scope_measure(4);

					vcs_buf[i][vcsp_idx] = m_dCH[4][2] - m_dCH[3][2];
					icsn_buf[i][vcsp_idx] = get_eload_current();

					//get_sm_current(viSM26);

					sText.AppendFormat(_T(" resoult=%.1fmV"), icsn_buf[icsn_idx][i] * 1000.0);
					set_scope_annotation(1, sText);
					save_scope_img(sImgFileName);
					sImgFileName.AppendFormat(_T(".png"));

					save_scope_data(1, sImgFileName);
					save_scope_data(2, sImgFileName);
					save_scope_data(3, sImgFileName);
					save_scope_data(4, sImgFileName);

					update_test_progress(99 * (i * 24 + vcsp) / 24 / 4);
					sWave += gen_html_waveform(sImgFileName, i + 1, 19);
					MSG msg;
					while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
						DispatchMessage(&msg);

					break;
				}


			}	// icsn increasing

			vcsp_idx++;

		} //for (double vcsp = 24; vcsp >= 2; vcsp -= 1)

	} // for VCS_OC2[1:] setting


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("VCSP"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T(""), _T("") }	// end marker
	};

	for (int i = 0; i < 4; i++)
		fTarget[i] = (i + 1) * 0.050;


	HC_X_AXIS Xaxis[] = {
		{ _T("VCS_OC (VCS_OC2[1:0]=50mV)"), 0, icsn_buf[0], _T("mV") },
		{ _T("VCS_OC (VCS_OC2[1:0]=100mV)"), 0, icsn_buf[1], _T("mV") },
		{ _T("VCS_OC (VCS_OC2[1:0]=150mV)"), 0, icsn_buf[2], _T("mV") },
		{ _T("VCS_OC (VCS_OC2[1:0]=200mV)"), 0, icsn_buf[3], _T("mV") },
		{ _T(""), 0, NULL, _T("") }
	};

	//------------------------------
	CString s(_T(""));
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	s = _T("Target(V),VCS_OC2[1:0],VCSP(V),ICSN(mA),VCS_OC(V)\n");
	for (int i = 0; i < 4; i++)
	{
		x_label_idx = 0;
		for (double vcsp = 24; vcsp >= 2; vcsp -= 1)
		{
			X_label[x_label_idx] = vcsp;
			s.AppendFormat(_T("%.3fV,0x%01X,%.3f,%.1f,%.3f\n")
				, (i + 1) * 0.050
				, i
				, vcsp
				, icsn_buf[i][x_label_idx]
				, vcs_buf[i][x_label_idx]
				);
			x_label_idx++;
		}
	}
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------


	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop += sCurve;
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//sJS_Title.MakeLower();
	//CString sTestFile = _T("sub_test_") + sJS_Title + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return S_OK;

}


HRESULT CATEView::On_test_accurate_cs_ocp(IHTMLElement *pElement)
{


#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GP");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 2;

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CSP");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;


	struct _TEST_ITEM_SEL_
	{
		LPCTSTR  sLev;
		int		reg;
	};
	_TEST_ITEM_SEL_ ti[] =
	{
		{ _T("5mV"), 0x15 },
		{ _T("10mV"), 0x2B },
		{ _T("15mV"), 0x40 },
		{ _T("20mV"), 0x55 },
		{ _T("25mV"), 0x6A },
		{ _T("30mV"), 0x80 },
		{ _T("35mV"), 0x95 },
		{ _T("40mV"), 0xAA },
		{ _T("45mV"), 0xBF },
		{ _T("50mV"), 0xD5 },
		{ _T("55mV"), 0xEA },
		{ _T("60mV"), 0xFF },
		{ _T(""), 0x0 },
	};
	int ti_cnt;
	for (int i = 0; i < 100; i++)
	if (ti[i].reg == 0)
	{
		ti_cnt = i;
		break;
	}

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	double dVbusIlim = 0.3;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 10.0, _T("VBUS"));
	set_scope_channel(2, true, 5, 5, _T("GP"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);
	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);
	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);

	set_sm_vi(viSM25, 5.0, dVbusIlim);	// VBUS: 5V, 100mA
	set_sm_vi(viSM26, 24, 0.3);		// CSP: 24V
	//set_sm_vi(viSM27, 0.75, 0.2);	
	set_sm_on(viSM25);
	set_sm_on(viSM26);
	//set_sm_on(viSM27);
	set_eload(0);
	set_eload_on();

	MyDelay(200);

	rt7800_wr(0x40004060, 0x0C, 0x3F);
	rt7800_wr(0x400040A0, 0x20);	//VOC_ACC_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//VOC_ACC_RST = 0
	rt7800_wr(0x40009034, 0x02);	//GP_Charge = 1
	rt7800_wr(0x40009034, 0x02, 0x02);	//GP_Charge = 1, GP_OCACC_EN = 1

	GP_discharge = rt7800_rd(0x40009034);
	if (GP_discharge != 0x02)
	{
		ASSERT(false);
		return S_OK;
	}

	set_scope_mode_normal();
	MyDelay(300);
#endif

	int vcsoc_idx = 0;
	int icsn_idx = 0;

	float vcs_buf[4][24];
	float icsn_buf[4][24];
	int vcs_oc2;
	CString sText;
	float X_label[80];
	float fTarget[50];
	float fDiff[50];
	int x_label_idx = 0;
	int vcsp_idx = 0;
	for (int i = 0; i < ti_cnt; i++)
	{
		vcsp_idx = 0;
		for (double vcsp = 24; vcsp >= 2; vcsp -= 1)
		{
			if (i == 0)
				X_label[x_label_idx++] = vcsp;
			vcs_oc2 = ti[i].reg;
			sText.Format(_T("VCS_OC[7_0]=0x%01X, CSP=%.1fV target=%s"), ti[i].reg, vcsp, (LPCTSTR)ti[i].sLev);

			set_sm_off(viSM25);
			set_sm_on(viSM25);
			set_sm_vi(viSM26, vcsp, dVbusIlim);
			set_sm_on(viSM26);
			set_eload(0);
			MyDelay(50);
			wdata1 = 0x0C | ((vcs_oc2 & 0x03) << 6);
			wdata2 = (vcs_oc2 >> 2) & 0xFF;
			rt7800_wr(0x40004060, wdata1, wdata2);	// VCS_OC[1:0]
			rt7800_wr(0x400040A0, 0x20);	//VOC_ACC_RST = 1
			rt7800_wr(0x400040A0, 0x00);	//VOC_ACC_RST = 0
			rt7800_wr(0x40009034, 0x02);	//GP_Charge = 1
			rt7800_wr(0x40009034, 0x02, 0x02);	//GP_Charge = 1, GP_OCACC_EN = 1

			GP_discharge = rt7800_rd(0x40009034);

			set_scope_annotation(1, sText);
			if (GP_discharge & 0x01)
			{
				set_scope_mode_auto();
			}
			else
			{
				set_scope_mode_normal();
				is_scope_triggered();
				set_scope_capture_single();
			}

			sImgFileName = sText;
			MyDelay(10);
			for (float icsn = 0; icsn <= 0.5; icsn += 0.001)
			{
				ASSERT(icsn < 0.5);

				set_eload(icsn);
				MyDelay(1);

				GP_discharge = rt7800_rd(0x40009034) & 0x01;
				if (GP_discharge)
				{

					MyDelay(30);
					set_scope_measure(3);
					set_scope_measure(4);

					vcs_buf[i][vcsp_idx] = m_dCH[4][2] - m_dCH[3][2];
					icsn_buf[i][vcsp_idx] = get_eload_current();

					//get_sm_current(viSM26);

					sText.AppendFormat(_T(" resoult=%.1fmV"), icsn_buf[icsn_idx][i] * 1000.0);
					set_scope_annotation(1, sText);
					save_scope_img(sImgFileName);
					sImgFileName.AppendFormat(_T(".png"));

					save_scope_data(1, sImgFileName);
					save_scope_data(2, sImgFileName);
					save_scope_data(3, sImgFileName);
					save_scope_data(4, sImgFileName);

					update_test_progress(99 * (i * 24 + vcsp) / 24 / 4);
					sWave += gen_html_waveform(sImgFileName, i + 1, 19);
					MSG msg;
					while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
						DispatchMessage(&msg);

					break;
				}


			}	// icsn increasing

			vcsp_idx++;

		} //for (double vcsp = 24; vcsp >= 2; vcsp -= 1)

	} // for VCS_OC2[1:] setting


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("VCSP"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T(""), _T("") }	// end marker
	};

	CString sDataTitle[20];
	for (int i = 0; i < ti_cnt; i++)
		sDataTitle[i].Format(_T("VCS_OC (VCS_OC[7:0]=0x%02X %s"), ti[i].reg, (LPCTSTR)ti[i].sLev);
	HC_X_AXIS Xaxis[] = {
		{ sDataTitle[0], 0, icsn_buf[0], _T("mV") },
		{ sDataTitle[1], 0, icsn_buf[1], _T("mV") },
		{ sDataTitle[2], 0, icsn_buf[2], _T("mV") },
		{ sDataTitle[3], 0, icsn_buf[3], _T("mV") },
		{ sDataTitle[4], 0, icsn_buf[4], _T("mV") },
		{ sDataTitle[5], 0, icsn_buf[5], _T("mV") },
		{ sDataTitle[6], 0, icsn_buf[6], _T("mV") },
		{ sDataTitle[7], 0, icsn_buf[7], _T("mV") },
		{ sDataTitle[8], 0, icsn_buf[8], _T("mV") },
		{ sDataTitle[9], 0, icsn_buf[9], _T("mV") },
		{ sDataTitle[10], 0, icsn_buf[10], _T("mV") },
		{ sDataTitle[11], 0, icsn_buf[11], _T("mV") },
		{ _T(""), 0, NULL, _T("") }
	};

	//------------------------------
	CString s(_T(""));
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	s = _T("Target(V),VCS_OC2[1:0],VCSP(V),ICSN(mA),VCS_OC(V)\n");
	for (int i = 0; i < ti_cnt; i++)
	{
		x_label_idx = 0;
		for (double vcsp = 24; vcsp >= 2; vcsp -= 1)
		{
			X_label[x_label_idx] = vcsp;
			s.AppendFormat(_T("%.3fV,0x%01X,%.3f,%.1f,%.3f\n")
				, (i + 1) * 0.050
				, i
				, vcsp
				, icsn_buf[i][x_label_idx]
				, vcs_buf[i][x_label_idx]
				);
			x_label_idx++;
		}
	}
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------


	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop += sCurve;
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//sJS_Title.MakeLower();
	//CString sTestFile = _T("sub_test_") + sJS_Title + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return S_OK;

}


HRESULT CATEView::On_test_icsp_icsn(IHTMLElement *pElement)
{


#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GP");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 2;

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("CSP");
	m_sSourceMeterName[2] = _T("CSN");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	double dVbusIlim = 0.3;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);
	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);
	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);

	set_sm_vi(viSM25, 5.0, dVbusIlim);	// VBUS: 5V, 100mA
	set_sm_vi(viSM26, 2, 0.1);
	set_sm_vi(viSM27, 0.75, 0.1);
	set_sm_on(viSM25);
	set_sm_on(viSM26);
	set_sm_on(viSM27);

	MyDelay(200);
	rt7800_wr(0x40004060, 0x0C, 0x3F);
	rt7800_wr(0x400040A0, 0x20);	//VOC_ACC_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//VOC_ACC_RST = 0
	rt7800_wr(0x40009034, 0x02);	//GP_Charge = 1

	GP_discharge = rt7800_rd(0x40009034);
	if (GP_discharge != 0x02)
	{
		ASSERT(false);
		return S_OK;
	}

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 10.0, _T("VBUS"));
	set_scope_channel(2, true, 10, 10.0, _T("GP"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	MyDelay(300);
#endif

	int vcsoc_idx = 0;
	int icsn_idx = 0;

	//double last_tri_lev = 0;
	int vcs_oc2;
	int nCurvePt = 0;
	float vcsn;
	float dlim = 0.2;
	float icsp[20][300];
	float icsn[20][300];
	int diff_idx = 0;
	int vcsp_idx = 0;
	float X_label[300];
	for (float vcsp = 2; vcsp <= 24; vcsp += 0.1)
	{
		diff_idx = 0;
		update_test_progress(99 * vcsp / 24);

		X_label[diff_idx] = vcsp;
		for (float diff = 0; diff <= 0.06; diff += 0.005)
		{
			set_sm_vi(viSM26, vcsp, dlim);
			vcsn = vcsp - diff;
			set_sm_vi(viSM27, vcsn, dlim);

			MyDelay(22);
			icsp[diff_idx][vcsp_idx] = 1000.0 * get_sm_current(viSM26);
			icsn[diff_idx][vcsp_idx] = 1000.0 * get_sm_current(viSM27);

			diff_idx++;
		}
		vcsp_idx++;
	}


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
	set_sm_off(viSM26);
	set_sm_off(viSM27);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("ICSP ICSN"),
		vcsp_idx,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}mA"), _T("mA") },
		{ _T(""), _T("") }	// end marker
	};



	HC_X_AXIS Xaxis[] = {
		{ _T("ICSP (VCSP-VCSN = 5mV)"), 0, icsp[1], _T("V") },
		{ _T("ICSP (VCSP-VCSN = 20mV)"), 0, icsp[4], _T("V") },
		{ _T("ICSP (VCSP-VCSN = 40mV)"), 0, icsp[8], _T("V") },
		{ _T("ICSP (VCSP-VCSN = 60mV)"), 0, icsp[12], _T("V") },
		{ _T("ICSN (VCSP-VCSN = 5mV)"), 0, icsn[1], _T("V") },
		{ _T("ICSN (VCSP-VCSN = 20mV)"), 0, icsn[4], _T("V") },
		{ _T("ICSN (VCSP-VCSN = 40mV)"), 0, icsn[8], _T("V") },
		{ _T("ICSN (VCSP-VCSN = 60mV)"), 0, icsn[12], _T("V") },
		{ _T(""), 0, NULL, _T("") }
	};

	//------------------------------
	CString s(_T(""));
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	for (int i = 0; i < diff_idx; i++)
	{
		if (i == 0)
			s.AppendFormat(_T("VCSP(V),,ICSP mA(diff=%dmV),ICSN mA(diff=%dmV),,"), i * 5, i * 5);
		else
			s.AppendFormat(_T("ICSP mA(diff=%dmV),ICSN mA(diff=%dmV),,"), i * 5, i * 5);
	}
	s += _T("\n");

	int v_idx = 0;
	for (float vcsp = 2; vcsp <= 24; vcsp += 0.1)
	{
		for (int i = 0; i < diff_idx; i++)
		{
			if (i == 0)
				s.AppendFormat(_T("%.1fV,,%.4f,%.4f,,"), vcsp, icsp[i][v_idx], icsn[i][v_idx]);
			else
				s.AppendFormat(_T("%.4f,%.4f,,"), icsp[i][v_idx], icsn[i][v_idx]);
		}
		s += _T("\n");
		v_idx++;
	}
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;
	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);
	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------


	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");
	m_sTop += sCurve;
	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//sJS_Title.MakeLower();
	//CString sTestFile = _T("sub_test_") + sJS_Title + _T(".htm");
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	update_test_progress(100);
	return S_OK;

}

HRESULT CATEView::On_test_vbus_uvp(IHTMLElement *pElement)
{


#ifdef NO_DEVICE_SIM
	m_sChName[0] = _T("VBUS");
	m_sChName[1] = _T("GP");
	m_sChName[2] = _T("VCSN");
	m_sChName[3] = _T("VCSP");
#endif

	update_test_progress(0);
	MSG msg2;
	MyDelay(100);
	while (PeekMessage(&msg2, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg2);
	m_nTestIdx = 3;
	log_add(_T("test: VBUS UVP"));

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("DAC_CV");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}

	initial_top_html();
	//return true;

	int wdata1, wdata2;
	int nTestCnt = 0;
	float fResult[100];
	for (int j = 0; j < 100; j++)
		fResult[j] = 0;
	CString sWave(_T(""));
	CString sImgFileName;

	struct { LPCTSTR sText; double dOffset; double dTimeScale; } arrZoom[] =
	{
		{ _T("500ms"), 16E-3, 500E-3 },
		{ _T("50ms"), 0, 50E-3 },
		{ _T("50ms(2)"), 16E-3, 50E-3 },
		{ _T("10ms"), 0, 10E-3 },
		{ _T("5ms"), 16E-3, 5E-3 },
		{ _T("200us"), 0, 200E-6 },
		{ _T("10us"), 0, 1E-5 },
		{ _T("10us(2)"), 78E-6, 10E-6 },
		{ _T(""), -1, -1 },
	};
	int nZoomCnt;
	for (int i = 0; i < 1000; i++)
	if (arrZoom[i].dTimeScale < 0)
	{
		nZoomCnt = i;
		break;
	}

	struct _VBUS_OV_SEL_
	{
		LPCTSTR  sLev;
		int		reg;
	};
	_VBUS_OV_SEL_ vbus_uv[] =
	{
		{ _T("4V"), 0x33 },
		{ _T("5V"), 0x40 },
		{ _T("6V"), 0x4D },
		{ _T("7V"), 0x59 },
		{ _T("8V"), 0x66 },
		{ _T("9V"), 0x73 },
		{ _T("10V"), 0x80 },
		{ _T("11V"), 0x8C },
		{ _T("12V"), 0x99 },
		{ _T("13V"), 0xA6 },
		{ _T("14V"), 0xB3 },
		{ _T("15V"), 0xBF },
		{ _T("16V"), 0xCC },
		{ _T(""), 0x0 },
	};



	double dVbusIlim = 0.2;
	int GP_discharge = 0;
	float dMeterResult[50];
	for (int i = 0; i < 50; i++)
		dMeterResult[i] = 0;

#ifndef NO_DEVICE_SIM
	set_sm_beep(viSM25, true);
	set_sm_beep(viSM27, true);

	set_sm_beep_once(viSM25);
	set_sm_beep_once(viSM27);

	set_sm_beep(viSM25, false);
	set_sm_beep(viSM27, false);

	set_eload_off();
	set_sm_off(viSM25);
	set_sm_vi(viSM25, 5.0, 0.3);	// VCSP: 5V, 100mA
	//set_sm_off(viSM27);
	//set_sm_vi(viSM26, 6.0, dVbusIlim);	// VBUS: 6V, 200mA

	//set_sm_vi(viSM27, 0.75, 0.1);	// DAC_DV: 0.75V, 100mA
	set_sm_vi(viSM27, 5, 0.1);	// DAC_DV: 0.75V, 100mA

	set_sm_on(viSM25);
	set_sm_off(viSM25);
	set_sm_on(viSM25);
	//set_sm_off(viSM26);
	set_sm_on(viSM27);

	MyDelay(100);

	wdata1 = 0xfc;
	wdata2 = 0x03;
	rt7800_wr(0x4000902c, 0x00);
	rt7800_wr(0x40004098, wdata1, wdata2);	// VBUS_UV[7:0]
	rt7800_wr(0x400040A0, 0xff);	//BUSUV_RST = 1
	rt7800_wr(0x400040A0, 0x00);	//BUSUV_RST = 0
	rt7800_wr(0x4000902C, 0x02);
	rt7800_wr(0x4000902C, 0x02, 0x01);	//GP_Charge = 1, GP_CNTL = 0, GP_BUS_UV_EN = 1

	GP_discharge = rt7800_rd(0x4000902C);
	if (GP_discharge != 0x02)
	{
		ASSERT(false);
		return S_OK;
	}

	//set scope
	//------------------------------
	double time_offset = 0.000;
	double time_scale = 500E-6;
	set_scope_time(time_offset, time_scale);
	set_scope_trigger(2, false, 2.5);
	set_scope_channel(1, true, 5, 15.0, _T("VBUS"));
	set_scope_channel(2, true, 5, -5.0, _T("GP"));
	set_scope_channel(3, true, 1, 1.5, _T("CSN"));
	set_scope_channel(4, true, 1, 2.0, _T("CSP"));
	//------------------------------

	set_scope_mode_normal();
	MyDelay(300);
#endif

	double last_tri_lev = 17;
	for (int i = 0; i < 1000; i++)
	{
		if (vbus_uv[i].sLev == _T(""))
			break;
		nTestCnt++;
		CString sText;
		sText.Format(_T("VBUS_UV[7_0]=0x%02X target=%s"), vbus_uv[i].reg, (LPCTSTR)vbus_uv[i].sLev);

#ifndef NO_DEVICE_SIM
		set_sm_vi(viSM25, 17, dVbusIlim);
		set_sm_on(viSM25);
		MyDelay(50);
		wdata1 = ((vbus_uv[i].reg & 0x3F) << 2);
		wdata2 = ((vbus_uv[i].reg & 0xc0) >> 6);
		rt7800_wr(0x4000902C, 0x02);
		rt7800_wr(0x40004098, wdata1, wdata2);	// VBUS_UV[7:0]
		rt7800_wr(0x400040A0, 0x04);			// BUSUV_RST = 1
		rt7800_wr(0x400040A0, 0x00);			// BUSUV_RST = 0
		rt7800_wr(0x4000902C, 0x02);			// GC2a_Charge=1
		rt7800_wr(0x4000902C, 0x02, 0x01);		// GC2a_Charge=1, GC2a_VBUSUV_EN=1

		set_scope_annotation(1, sText);
		is_scope_triggered();
		set_scope_capture_single();
		MyDelay(200);
#endif


		sImgFileName = sText;

		for (double dVbusLev = 17; dVbusLev > 3; dVbusLev -= 0.1)
		{
			if (dVbusLev > last_tri_lev + 2)
				dVbusLev = last_tri_lev + 2;

#ifndef NO_DEVICE_SIM
			set_sm_vi(viSM25, dVbusLev, dVbusIlim);
			MyDelay(10);
			GP_discharge = rt7800_rd(0x4000902C);
#else
			GP_discharge = 1;
#endif
			if (GP_discharge & 0x01)
			{

				MyDelay(100);
				last_tri_lev = dVbusLev;
#ifndef NO_DEVICE_SIM
				if (!is_scope_triggered())
				{
					set_scope_mode_auto();
					set_scope_capture_run();
					MyDelay(50);
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
					set_scope_mode_normal();
				}
				else
				{
					set_scope_measure(1);
					set_scope_measure(2);
					set_scope_measure(3);
					set_scope_measure(4);
				}

				dMeterResult[i] = dVbusLev;
				fResult[i] = (m_dCH[0][1] + m_dCH[0][0]) / 2;

				get_sm_voltage(viSM25);
				get_sm_current(viSM25);
				//get_sm_voltage(viSM26);
				//get_sm_current(viSM26);
				get_sm_voltage(viSM27);
				get_sm_current(viSM27);

				get_eload_voltage();
				get_eload_current();


				//sText.Format(_T("VBUS_OV[7_0]=0x%02X target=%s"), vbus_ov[i].reg, (LPCTSTR) vbus_ov[i].sLev);
				sText.AppendFormat(_T(" resoult=%.2fV"), dVbusLev);
				set_scope_annotation(1, sText);
				save_scope_img(sImgFileName);
#endif


				sImgFileName.AppendFormat(_T(".png"));

#ifndef NO_DEVICE_SIM
				save_scope_data(1, sImgFileName);
				save_scope_data(2, sImgFileName);
				save_scope_data(3, sImgFileName);
				save_scope_data(4, sImgFileName);
#endif

				update_test_progress(100 * i / 13 + 1);
				sWave += gen_html_waveform(sImgFileName, i + 1, 13);
				//MyDelay(30);
				MSG msg;
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
					DispatchMessage(&msg);

#ifndef NO_DEVICE_SIM
				set_sm_vi(viSM25, 17.0, dVbusIlim);
				set_sm_off(viSM25);
#endif
				break;
			}
		}

	}


#ifndef NO_DEVICE_SIM
	set_sm_off(viSM25);
#endif


	//------------------------------------------------------------
	// gen javascript curve
	//------------------------------------------------------------
	float X_label[80];
	float fTarget[50];
	float fDiff[50];

#ifdef NO_DEVICE_SIM
	nTestCnt = 13;
	float ftemp[] = { 6.20, 7.10, 8.20, 9.20, 10.20, 11.20, 12.20, 13.20, 14.20, 15.20, 16.20, 17.30, 18.20, 19.20, 20.20, 21.10, 21.10, 21.10, 21.10 };
	for (int i = 0; i < nTestCnt; i++)
	{
		fResult[i] = ftemp[i];
		dMeterResult[i] = 30 * sin((float)i*0.3);
	}
#endif

	for (int i = 0; i < nTestCnt; i++)
		X_label[i] = (float)vbus_uv[i].reg;

	CString sJS_Title = m_TestList[m_nTestIdx].sText;
	sJS_Title.Replace(_T(' '), _T('_'));
	HC_OUTLINE ol = {
		//_T("VBUS_OVP"),
		sJS_Title,
		_T("VBUS_UV[7:0]"),
		nTestCnt,
		X_label,
		true
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T("{value}delta V"), _T("Voltage") },
		{ _T(""), _T("") }	// end marker
	};

	for (int i = 0; i < 50; i++)
		fTarget[i] = i + 4;

	for (int i = 0; i < nTestCnt; i++)
		fDiff[i] = fTarget[i] - dMeterResult[i];

	HC_X_AXIS Xaxis[] = {
		{ _T("Target"), 0, fTarget, _T("V") },
		{ _T("VBUS_UVP_scope"), 0, fResult, _T("V") },
		{ _T("VBUS_UVP_SM"), 0, dMeterResult, _T("V") },
		{ _T("error"), 1, fDiff, _T("delta V") },
		{ _T(""), 0, NULL, _T("") }
	};


	//------------------------------
	CString s(_T(""));
	CString sTestItemDir = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	CString sFile2 = sTestItemDir;
	CString sTestFileTag = m_TestList[m_nTestIdx].sCmd;
	sTestFileTag.Replace(_T(' '), _T('_'));
	sTestFileTag.MakeLower();
	sFile2 += sTestFileTag + _T(".csv");
	s = _T("Target,VBUS_UV[7:0],VBUS_UV (v)\n");
	for (int i = 0; i < nTestCnt; i++)
		s.AppendFormat(_T("%s,0x%02X,%.3f\n"), (LPCTSTR)vbus_uv[i].sLev, vbus_uv[i].reg, dMeterResult[i]);
	save_text_file(sFile2, s);


	CString sCurve(_T(""));
	CString sTestNameID = _T("id_result_") + sTestFileTag;

	sCurve = gen_js_chart(sTestNameID, ol, Yaxis, Xaxis);

	sFile2 = sTestItemDir + _T("gen_result_") + sTestFileTag + _T(".js");
	save_text_file(sFile2, sCurve);

	int npos = sFile2.Find(_T("\\"), 1);
	sFile2 = sFile2.Right(sFile2.GetLength() - npos - 1);
	sCurve.Format(_T("<script src=\"%s\"></script>\n"), (LPCTSTR)sFile2);
	sCurve.AppendFormat(_T("<div id=\"%s\"></div>\n"), (LPCTSTR)sTestNameID);
	update_html_div(sTestNameID, sCurve);
	//------------------------------




	//CString sCurve(_T("<div id=\"id_measure_result\"></div>\n"));
	//sCurve += gen_js_chart(_T("VBUS_UVP"), ol, Yaxis, Xaxis);
	//sCurve += gen_js_chart(sJS_Title, ol, Yaxis, Xaxis);

	CString sConn = change_evb_connection_zoom(m_TestList[m_nTestIdx].sCmd);
	m_sTop += sConn;

	m_sTop += sCurve;
	//m_sTop.Append(sCurve);

	m_sTop += _T("<div style=\"background:#6900b3; width:100%; height:10pt\"></div>\n");

	m_sTop.Append(sWave);

	CString sTestFile = _T("sub_test_") + sTestFileTag + _T(".htm");
	//save_html_file(m_sTop, _T("sub_test_VBUS_UVP.html"));
	save_html_file(m_sTop, sTestFile);

	set_sm_beep(viSM25, true);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep_once(viSM25);
	MyDelay(100);
	set_sm_beep(viSM25, false);

	Refresh();


	VARIANT varargStart;
	varargStart.vt = VT_BOOL;
	varargStart.boolVal = true;

	IHTMLElement *pElement1;
	HRESULT hResult = GetElement(L"id_result_vbus_uvp", &pElement1);
	pElement1->scrollIntoView(varargStart);

	update_test_progress(100);
	return true;

}

HRESULT CATEView::OnClickFolder(IHTMLElement *phtmlElement)
{
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	phtmlElement->get_innerText(&bstr);
	phtmlElement->get_innerHTML(&bstr);
	phtmlElement->get_outerHTML(&bstr);
	phtmlElement->get_title(&bstr);
	phtmlElement->get_tagName(&bstr);




	phtmlElement->get_outerText(&bstr);
	bstr = _T("<INPUT id=edit_id class=folder style=\"CURSOR: hand; TEXT - DECORATION: none; COLOR: #0f0f00\" value=99a9 name=fname>");
	phtmlElement->put_outerHTML(bstr);
	phtmlElement->get_isTextEdit(&pVar);
	phtmlElement->get_onafterupdate(&ptrVal);
	phtmlElement->get_id(&bstr);
	phtmlElement->get_id(&bstr);

	//Disp = NULL;
	//phtmlElement->get_all(&pDocDisp);
	//VARIANT* pvar;
	//phtmlElement->getAttribute(bstr, NULL, pvar);
	//return S_OK;


	if (bstr)
	{
		CString strNewDir;
		strNewDir = m_strCurrDir + _T("\\") + CString(bstr);
		SetCurrentDirectory(strNewDir);
		RefreshDir();
	}
	return S_OK;

}

HRESULT CATEView::OnClickFile(IHTMLElement *phtmlElement)
{
	CComBSTR bstr;
	phtmlElement->get_innerText(&bstr);
	if (bstr)
	{
		CString strFullPath;
		strFullPath = m_strCurrDir + _T("\\") + CString(bstr);
		ShellExecute(NULL, NULL, strFullPath, NULL, NULL, SW_SHOWNORMAL);
	}
	return S_OK;
}

HRESULT CATEView::OnInstrumentLink(IHTMLElement *phtmlElement)
{
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	phtmlElement->get_innerText(&bstr);
	phtmlElement->get_innerHTML(&bstr);
	phtmlElement->get_outerHTML(&bstr);
	phtmlElement->get_title(&bstr);
	phtmlElement->get_tagName(&bstr);
	phtmlElement->get_id(&bstr);


	//GetItemValue(_T("id_0x12_bit4_1"));


	//phtmlElement->get_className(&bstr);
	CString s(bstr);
	if (s.Find(_T("_link_")) == -1)
		return S_OK;

	int link_target = _tcstoul(s.Right(2), 0, 10);

	CComPtr<IHTMLElement> spfileList;
	CString strTable(_T("change id ok"));
	IHTMLElement *pElement2;

	CString sDeviceName;
	if (link_target == 25)	// source meter 1
	{
		sDeviceName = sm_init(25, &viSM25);
		if (sDeviceName.GetLength() > 8)
		{
			HRESULT hResult = GetElement(L"instrument_status_25", &pElement2);
			bstr = _T("instrument_status_linked");
			pElement2->put_className(bstr);

			CComBSTR bstrTable = sDeviceName;
			pElement2->put_innerHTML(bstrTable);
		}
	}
	else if (link_target == 26)	// source meter 2
	{
		sDeviceName = sm_init(26, &viSM26);
		if (sDeviceName.GetLength() > 8)
		{
			HRESULT hResult = GetElement(L"instrument_status_26", &pElement2);
			bstr = _T("instrument_status_linked");
			pElement2->put_className(bstr);
			CComBSTR bstrTable = sDeviceName;
			pElement2->put_innerHTML(bstrTable);
		}
	}
	else if (link_target == 27)	// source meter 3
	{
		sDeviceName = sm_init(27, &viSM27);
		if (sDeviceName.GetLength() > 8)
		{
			HRESULT hResult = GetElement(L"instrument_status_27", &pElement2);
			bstr = _T("instrument_status_linked");
			pElement2->put_className(bstr);

			CComBSTR bstrTable = sDeviceName;
			pElement2->put_innerHTML(bstrTable);
		}
	}
	else if (link_target == 3)	// Scope
	{
		//GetElementInterface(_T("instrument_status_3"), IID_IHTMLElement, (void **) &spfileList);
		//spfileList->put_className(bstr);

		//CComBSTR bstrTable = strTable;
		//spfileList->put_innerHTML(bstrTable);
		//pElement2->put_innerHTML(bstrTable);
		//pElement2->put_className(bstr);
		//if (spfileList == NULL)
		//	return S_OK;

		//OnBnClickedButtonLoadScope(); // original test
		CString sScopeName = scope_init();
		if (sScopeName.GetLength() > 8)
		{
			HRESULT hResult = GetElement(L"instrument_status_3", &pElement2);
			bstr = _T("instrument_status_linked");
			pElement2->put_className(bstr);

			CComBSTR bstrTable = sScopeName;
			pElement2->put_innerHTML(bstrTable);
		}
	}
	else if (link_target == 4)	// E-Load
	{
		CString sEloadName = eload_init();
		HRESULT hResult = GetElement(L"instrument_status_4", &pElement2);
		bstr = _T("instrument_status_linked");
		pElement2->put_className(bstr);

		CComBSTR bstrTable = sEloadName;
		pElement2->put_innerHTML(bstrTable);
	}

	else if (link_target == 5)	// Chamber 4350
	{
		CString sChamberName = chamber_init();
		HRESULT hResult = GetElement(L"instrument_status_5", &pElement2);
		bstr = _T("instrument_status_linked");
		pElement2->put_className(bstr);

		CComBSTR bstrTable = sChamberName;
		pElement2->put_innerHTML(bstrTable);
	}

	return S_OK;

}


HRESULT CATEView::OnBitModify(IHTMLElement *phtmlElement)
{
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	phtmlElement->get_innerText(&bstr);
	//phtmlElement->get_innerHTML(&bstr);
	phtmlElement->get_outerHTML(&bstr);
	//phtmlElement->get_title(&bstr);
	//phtmlElement->get_tagName(&bstr);


	//GetItemValue(_T("id_0x12_bit4_1"));

	//for (int i = 0; i < 5; i++)
	//{
	//	//rt7800_rd(0x01020304);
	//	rt7800_rd(0x14020304);
	//}

	phtmlElement->get_className(&bstr);
	CString s(bstr);
	bool bit_en = (s == _T("btn_not_sel"));

	CString new_class = bit_en ? L"btn_blue_sel" : L"btn_not_sel";
	bstr = new_class;
	phtmlElement->put_className(bstr);

	//s.Format(_T("<a class=\"%s\" id=\"id_0x12_bit4_0\" href=\"#register\">reg_0x12[4] = 0</a>"), 
	//	bit_en ? L"btn_blue_sel" : L"btn_not_sel");
	//phtmlElement->get_outerText(&bstr);

	//s.Format(_T("<a class=\"btn_blue_sel\" id=\"id_0x12_bit4_0\" href=\"#register\">reg_0x12[4] = 0</a>"));
	//bstr = _T("<INPUT id=edit_id class=folder style=\"CURSOR: hand; TEXT - DECORATION: none; COLOR: #0f0f00\" value=99a9 name=fname>");

	IHTMLElement *pElement2;
	CString csValue;
	HRESULT hResult = GetElement(L"id_0x12_bit4_1", &pElement2);
	if (pElement2 != NULL)
	{
		bit_en = false;
		new_class = bit_en ? L"btn_blue_sel" : L"btn_not_sel";
		bstr = new_class;
		pElement2->put_className(bstr);
		//CString itemtype = L"value";  
		//VARIANT value;  
		//BSTR bstrType = itemtype.AllocSysString();  
		//pElement2->getAttribute(bstrType,0,&value);  
		//::SysFreeString(bstrType);  
		//if (value.vt == VT_BSTR)  
		//{  
		//    csValue.Format (L"%s",(const char*)(_bstr_t)(_variant_t)value);  
		//}  
	}

	bstr = s;
	//phtmlElement->put_outerHTML(bstr);
	phtmlElement->get_isTextEdit(&pVar);
	phtmlElement->get_onafterupdate(&ptrVal);
	phtmlElement->get_id(&bstr);
	phtmlElement->get_id(&bstr);

	//Disp = NULL;
	//phtmlElement->get_all(&pDocDisp);
	//VARIANT* pvar;
	//phtmlElement->getAttribute(bstr, NULL, pvar);
	//return S_OK;


	if (bstr)
	{
		CString strNewDir;
		strNewDir = m_strCurrDir + _T("\\") + CString(bstr);
		SetCurrentDirectory(strNewDir);
		RefreshDir();
	}
	return S_OK;

}

CString CATEView::GetItemValue(CString itemID)
{
	CString csValue = L"";
	IHTMLElement *pElementName = NULL;
	HRESULT hResult = GetElement(itemID, &pElementName);
	if (pElementName != NULL)
	{

		CString itemtype = L"checked";
		if (itemID == _T("id_dac_temp_enable"))
			itemtype = L"checked";
		else
			itemtype = L"value";
		VARIANT value;
		BSTR bstrType = itemtype.AllocSysString();
		BSTR bstr2;
		VARIANT varCheck;
		pElementName->get_outerHTML(&bstr2);
		pElementName->getAttribute(bstrType, 0, &value);

		//CComQIPtr<IHTMLInputElement> pInputElem(pElementName);
		//if (pInputElem)
		//{
		//	pInputElem->get_checked(&varCheck.boolVal);
		//}

		::SysFreeString(bstrType);
		if (value.vt == VT_BSTR || value.vt==VT_BOOL)
		{
			csValue = (CString)value;
			//csValue.Format(L"%s", (const char*)(_bstr_t)(_variant_t)value);
		}
	}
	return csValue;
}



BOOL CATEView::OnInitTestItem()
{

	i2c_init();


	m_bChamberMode = false;

	viSM25 = NULL;
	viSM26 = NULL;
	viSM27 = NULL;
	viChamber = NULL;
	viLoad = NULL;

	m_dVrect = 0;
	m_dIout = 0;
	m_dVout = 0;
	m_dRP = 0;
	m_dTS = 0;
	m_dFreq = 0;
	m_dELoadV = 0;
	m_dELoadI = 0;
	m_dELoadI_set = 0;

	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 3; j++)
		m_dCH[i][j] = 0;

	m_sSourceMeterName[0] = _T("VBUS");
	m_sSourceMeterName[1] = _T("VX");
	m_sSourceMeterName[2] = _T("VCSP");
	for (int i = 0; i < 3; i++)
	{
		m_dSourceMeterV[i] = 0;
		m_dSourceMeterI[i] = 0;
	}


	_TestList tl[] =
	{
		{ 0, 0, 0, 0, TEST_MODE::FOLDER, 0, 0, 0, 0, _T("Test Group"), _T(""), _T("") },

		{ 1, 1, 0, 0, TEST_MODE::FOLDER, 0, 0, 0, 0, _T("Protection Test"), _T(""), _T("") },
		{ 1, 1, 1, 0, TEST_MODE::PROTECT_VBUS_OVP, 0, 0, 0, 0, _T("VBUS OVP"), _T("VBUS OVP"), _T("evb\\protect_vbus_ovp") },
		{ 1, 1, 2, 0, TEST_MODE::PROTECT_VBUS_UVP, 0, 0, 0, 0, _T("VBUS UVP"), _T("VBUS UVP"), _T("evb\\protect_vbus_uvp") },
		{ 1, 1, 3, 0, TEST_MODE::PROTECT_VX_OVP, 0, 0, 0, 0, _T("VX OVP"), _T("VX OVP"), _T("evb\\protect_vx_ovp") },
		{ 1, 1, 4, 0, TEST_MODE::PROTECT_QUICK_CS_OCP, 0, 0, 0, 0, _T("QUICK CS OCP"), _T("QUICK CS OCP"), _T("evb\\protect_quick_cs_ocp") },
		{ 1, 1, 5, 0, TEST_MODE::PROTECT_ACCURATE_CS_OCP, 0, 0, 0, 0, _T("Accurate CS OCP"), _T("Accurate CS OCP"), _T("evb\\protect_accurate_cs_ocp") },
		{ 1, 1, 6, 0, TEST_MODE::PROTECT_ACCURATE_CS_OCP, 0, 0, 0, 0, _T("ICSP ICSN"), _T("ICSP ICSN"), _T("evb\\protect_icsp_icsn") },
		{ 1, 1, 7, 0, TEST_MODE::PROTECT_DAC_CV, 0, 0, 0, 0, _T("DAC CV"), _T("DAC CV"), _T("evb\\protect_dac_cv") },

		{ 1, 2, 0, 0, TEST_MODE::FOLDER, 0, 0, 0, 0, _T("ADC Sweep"), _T(""), _T("") },
		{ 1, 2, 1, 0, TEST_MODE::ADC_CC1, 0, 0, 0, 0, _T("ADC CC1"), _T("ADC CC1"), _T("evb\\adc_cc1") },
		{ 1, 2, 2, 0, TEST_MODE::ADC_CC2, 0, 0, 0, 0, _T("ADC CC2"), _T("ADC CC2"), _T("evb\\adc_cc2") },
		{ 1, 2, 3, 0, TEST_MODE::ADC_CS, 0, 0, 0, 0, _T("ADC CS"), _T("ADC CS"), _T("evb\\adc_cs") },
		{ 1, 2, 4, 0, TEST_MODE::ADC_VBUS, 0, 0, 0, 0, _T("ADC VBUS"), _T("ADC VBUS"), _T("evb\\adc_vbus") },
		{ 1, 2, 5, 0, TEST_MODE::ADC_VBRL, 0, 0, 0, 0, _T("ADC VBRL"), _T("ADC VBRL"), _T("evb\\adc_vbrl") },
		{ 1, 2, 6, 0, TEST_MODE::ADC_VSYS5, 0, 0, 0, 0, _T("ADC VSYS5"), _T("ADC VSYS5"), _T("evb\\adc_vsys5") },
		{ 1, 2, 7, 0, TEST_MODE::ADC_AIN1, 0, 0, 0, 0, _T("ADC AIN1"), _T("ADC AIN1"), _T("evb\\adc_ain1") },
		{ 1, 2, 8, 0, TEST_MODE::ADC_AIN2, 0, 0, 0, 0, _T("ADC AIN2"), _T("ADC AIN2"), _T("evb\\adc_ain2") },
		{ 1, 2, 9, 0, TEST_MODE::ADC_VBP, 0, 0, 0, 0, _T("ADC VBP"), _T("ADC VBP"), _T("evb\\adc_vbp") },
		{ 1, 2, 10, 0, TEST_MODE::ADC_VCONN, 0, 0, 0, 0, _T("ADC VCONN"), _T("ADC VCONN"), _T("evb\\adc_vconn") },
		{ 1, 2, 11, 0, TEST_MODE::ADC_TS, 0, 0, 0, 0, _T("ADC TS"), _T("ADC TS"), _T("evb\\adc_ts") },
		{ 1, 2, 12, 0, TEST_MODE::ADC_CSP, 0, 0, 0, 0, _T("ADC CSP"), _T("ADC CSP"), _T("evb\\adc_csp") },


		{ -1, -1, -1, 0, TEST_MODE::CC_LOAD, 0, 0, 0, 0, _T(")") },
	};

	for (int i = 0; i < 200; i++)

		m_nTotalTestItemCnt = 0;
	for (int i = 0; i < 200; i++)
	{
		if (tl[i].lev1 < 0)
		{
			m_nTotalTestItemCnt = i;
			break;
		}
		m_TestList[i].lev1 = tl[i].lev1;
		m_TestList[i].lev2 = tl[i].lev2;
		m_TestList[i].lev3 = tl[i].lev3;
		m_TestList[i].lev4 = tl[i].lev4;
		//m_TestList[i].bTestItem = tl[i].bTestItem;
		m_TestList[i].sText = tl[i].sText;
		m_TestList[i].Iload1 = tl[i].Iload1;
		m_TestList[i].Iload2 = tl[i].Iload2;
		m_TestList[i].tm = tl[i].tm;
		m_TestList[i].para1 = tl[i].para1;
		m_TestList[i].para2 = tl[i].para2;
		m_TestList[i].sCmd = tl[i].sCmd;
		m_TestList[i].sFolderPath = tl[i].sFolderPath;
	}


	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


/* Initialize the oscilloscope to a known state.
* --------------------------------------------------------------- */
void CATEView::initialize(void)
{
	/* Clear the interface. */
	err = viClear(vi);
	if (err != VI_SUCCESS) error_handler();

	/* Get and display the device's *IDN? string. */
	do_query_string("*IDN?");
	printf("Oscilloscope *IDN? string: %s\n", str_result);

	/* Clear status and load the default setup. */
	do_command("*CLS");
	do_command("*RST");
}

/* Capture the waveform.
* --------------------------------------------------------------- */
void CATEView::capture(void)
{
	int num_bytes;
	FILE *fp;

	/* Use auto-scale to automatically configure oscilloscope. */
	do_command(":AUToscale");

	/* Set trigger mode (EDGE, PULSe, PATTern, etc., and input source. */
	do_command(":TRIGger:MODE EDGE");
	do_query_string(":TRIGger:MODE?");
	printf("Trigger mode: %s\n", str_result);

	/* Set EDGE trigger parameters. */
	do_command(":TRIGger:EDGE:SOURCe CHANnel1");
	do_query_string(":TRIGger:EDGE:SOURce?");
	printf("Trigger edge source: %s\n", str_result);

	do_command(":TRIGger:EDGE:LEVel 1.5");
	do_query_string(":TRIGger:EDGE:LEVel?");
	printf("Trigger edge level: %s\n", str_result);

	do_command(":TRIGger:EDGE:SLOPe POSitive");
	do_query_string(":TRIGger:EDGE:SLOPe?");
	printf("Trigger edge slope: %s\n", str_result);

	/* Save oscilloscope configuration. */

	/* Read system setup. */
	num_bytes = do_query_ieeeblock(":SYSTem:SETup?");
	printf("Read setup string query (%d bytes).\n", num_bytes);

	/* Write setup string to file. */
	fp = fopen("d:\\scope\\config\\setup.stp", "wb");
	num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes,
		fp);
	fclose(fp);
	printf("Wrote setup string (%d bytes) to ", num_bytes);
	printf("d:\\scope\\config\\setup.stp.\n");

	/* Change settings with individual commands:

	/* Set vertical scale and offset. */
	do_command(":CHANnel1:SCALe 1.00");		// 1V
	do_query_string(":CHANnel1:SCALe?");
	printf("Channel 1 vertical scale: %s\n", str_result);

	do_command(":CHANnel1:OFFSet -1.5");
	do_query_string(":CHANnel1:OFFSet?");
	printf("Channel 1 offset: %s\n", str_result);

	// label on
	do_command(":DISPlay:LABel ON");
	do_command(":CHANnel1:LABel \"VRECT\"");
	do_command(":CHANnel2:LABel \"VOUT\"");
	do_command(":CHANnel3:LABel \"COMM2\"");
	do_command(":CHANnel4:LABel \"IOUT\"");
	do_command(":DISPlay:ANNotation ON");
	do_command(":DISPlay:ANNotation:COLor CH2");
	do_command(":DISPlay:ANNotation:TEXT \"TEXT this is a annotation for chanel 2\"");
	do_command(":DISPlay:ANNotation:COLor CH1");
	do_command(":DISPlay:ANNotation:TEXT \"this is another annotation for chanel 1\"");

	//do_command(":DISPlay:PERSistence INF");
	//do_command(":DISPlay:PERSistence 10");
	//do_command(":DISPlay:CLEar");
	//do_command(":DISPlay:PERSistence MIN");

	/* Set horizontal scale and offset. */
	do_command(":TIMebase:SCALe 0.0000200");
	do_query_string(":TIMebase:SCALe?");
	printf("Timebase scale: %s\n", str_result);

	do_command(":TIMebase:POSition 0.0");
	do_query_string(":TIMebase:POSition?");
	printf("Timebase position: %s\n", str_result);

	/* Set the acquisition type (NORMal, PEAK, AVERage, or HRESolution). */
	do_command(":ACQuire:TYPE NORMal");
	do_query_string(":ACQuire:TYPE?");
	printf("Acquire type: %s\n", str_result);

	/* Or, configure by loading a previously saved setup. */

	//	/* Read setup string from file. */
	//	fp = fopen ("d:\\scope\\config\\setup.stp", "rb");
	//	num_bytes = fread (ieeeblock_data, sizeof(unsigned char),
	//		IEEEBLOCK_SPACE, fp);
	//	fclose (fp);
	//	printf("Read setup string (%d bytes) from file ", num_bytes);
	//	printf("d:\\scope\\config\\setup.stp.\n");
	//
	//	/* Restore setup string. */
	//	num_bytes = do_command_ieeeblock(":SYSTem:SETup", num_bytes);
	//	printf("Restored setup string (%d bytes).\n", num_bytes);

	/* Capture an acquisition using :DIGitize. */
	do_command(":DIGitize CHANnel1,CHANnel2,CHANnel3,CHANnel4");
}

/* Analyze the captured waveform.
* --------------------------------------------------------------- */
void CATEView::analyze(void)
{
	double wav_format;
	double acq_type;
	double wav_points;
	double avg_count;
	double x_increment;
	double x_origin;
	double x_reference;
	double y_increment;
	double y_origin;
	double y_reference;

	FILE *fp;
	int num_bytes;   /* Number of bytes returned from instrument. */
	int i;

	/* Make a couple of measurements.
	* ------------------------------------------------------------- */
	do_command(":MEASure:SOURce CHANnel1");
	do_query_string(":MEASure:SOURce?");
	printf("Measure source: %s\n", str_result);

	do_command(":MEASure:FREQuency");
	do_query_number(":MEASure:FREQuency?");
	printf("Frequency: %.4f kHz\n", num_result / 1000);

	do_command(":MEASure:VAMPlitude");
	do_query_number(":MEASure:VAMPlitude?");
	printf("Vertical amplitude: %.2f V\n", num_result);

	/* Download the screen image.
	* ------------------------------------------------------------- */
	do_command(":HARDcopy:INKSaver OFF");

	/* Read screen image. */
	num_bytes = do_query_ieeeblock(":DISPlay:DATA? PNG, COLor");
	printf("Screen image bytes: %d\n", num_bytes);

	/* Write screen image bytes to file. */
	fp = fopen("d:\\scope\\data\\screen.png", "wb");
	num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes,
		fp);
	fclose(fp);
	printf("Wrote screen image (%d bytes) to ", num_bytes);
	printf("d:\\scope\\data\\screen.bmp.\n");

	/* Download waveform data.
	* ------------------------------------------------------------- */

	/* Set the waveform points mode. */
	do_command(":WAVeform:POINts:MODE RAW");
	do_query_string(":WAVeform:POINts:MODE?");
	printf("Waveform points mode: %s\n", str_result);

	/* Get the number of waveform points available. */
	do_query_string(":WAVeform:POINts?");
	printf("Waveform points available: %s\n", str_result);

	/* Set the waveform source. */
	do_command(":WAVeform:SOURce CHANnel1");
	do_query_string(":WAVeform:SOURce?");
	printf("Waveform source: %s\n", str_result);

	/* Choose the format of the data returned (WORD, BYTE, ASCII): */
	do_command(":WAVeform:FORMat BYTE");
	do_query_string(":WAVeform:FORMat?");
	printf("Waveform format: %s\n", str_result);

	/* Display the waveform settings: */
	do_query_numbers(":WAVeform:PREamble?");

	wav_format = dbl_results[0];
	if (wav_format == 0.0)
	{
		printf("Waveform format: BYTE\n");
	}
	else if (wav_format == 1.0)
	{
		printf("Waveform format: WORD\n");
	}
	else if (wav_format == 2.0)
	{
		printf("Waveform format: ASCii\n");
	}

	acq_type = dbl_results[1];
	if (acq_type == 0.0)
	{
		printf("Acquire type: NORMal\n");
	}
	else if (acq_type == 1.0)
	{
		printf("Acquire type: PEAK\n");
	}
	else if (acq_type == 2.0)
	{
		printf("Acquire type: AVERage\n");
	}
	else if (acq_type == 3.0)
	{
		printf("Acquire type: HRESolution\n");
	}

	wav_points = dbl_results[2];
	printf("Waveform points: %e\n", wav_points);

	avg_count = dbl_results[3];
	printf("Waveform average count: %e\n", avg_count);

	x_increment = dbl_results[4];
	printf("Waveform X increment: %e\n", x_increment);

	x_origin = dbl_results[5];
	printf("Waveform X origin: %e\n", x_origin);

	x_reference = dbl_results[6];
	printf("Waveform X reference: %e\n", x_reference);

	y_increment = dbl_results[7];
	printf("Waveform Y increment: %e\n", y_increment);

	y_origin = dbl_results[8];
	printf("Waveform Y origin: %e\n", y_origin);

	y_reference = dbl_results[9];
	printf("Waveform Y reference: %e\n", y_reference);

	/* Read waveform data. */
	num_bytes = do_query_ieeeblock(":WAVeform:DATA?");
	printf("Number of data values: %d\n", num_bytes);

	/* Open file for output. */
	fp = fopen("d:\\scope\\data\\waveform_data.csv", "wb");

	/* Output waveform data in CSV format. */
	for (i = 0; i < num_bytes - 1; i++)
	{
		/* Write time value, voltage value. */
		fprintf(fp, "%9f, %6f\n",
			x_origin + ((float)i * x_increment),
			(((float)ieeeblock_data[i] - y_reference) * y_increment)
			+ y_origin);
	}

	/* Close output file. */
	fclose(fp);
	printf("Waveform format BYTE data written to ");
	printf("d:\\scope\\data\\waveform_data.csv.\n");
}

/* Send a command to the instrument.
* --------------------------------------------------------------- */
void CATEView::do_command(char *command)
{
	char message[80];

	strcpy(message, command);
	strcat(message, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();

	check_instrument_errors();
}

void CATEView::do_command(CString command)
{
	CW2A pszA(command);
	char* buf_command = pszA;
	do_command(buf_command);
}

void CATEView::do_command(ViSession vi, char *command)
{
	char message[80];

	strcpy(message, command);
	strcat(message, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler(vi);

	check_instrument_errors(vi);
}

void CATEView::do_command(ViSession vi, CString command)
{
	CW2A pszA(command);
	char* buf_command = pszA;
	do_command(vi, buf_command);
}

/* Command with IEEE definite-length block.
* --------------------------------------------------------------- */
int CATEView::do_command_ieeeblock(char *command, int num_bytes)
{
	char message[80];
	unsigned long data_length;

	strcpy(message, command);
	strcat(message, " #8%08d");
	err = viPrintf(vi, message, num_bytes);
	if (err != VI_SUCCESS) error_handler();

	err = viBufWrite(vi, ieeeblock_data, num_bytes, &data_length);
	if (err != VI_SUCCESS) error_handler();

	check_instrument_errors();

	return(data_length);
}

/* Query for a string result.
* --------------------------------------------------------------- */
void CATEView::do_query_string(char *query)
{
	char message[80];

	strcpy(message, query);
	strcat(message, "\n");

	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();

	err = viScanf(vi, "%t", str_result);
	if (err != VI_SUCCESS) error_handler();

	//szData[dwRead] = '\0';
	CA2W swIDN(str_result);
	CString csIDN(swIDN);
	sResult = csIDN;
	m_dResult = _tcstod(sResult, 0);

	check_instrument_errors();
}

void CATEView::do_query_string(ViSession vi, char *query)
{
	char message[80];

	strcpy(message, query);
	strcat(message, "\n");

	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler(vi);

	err = viScanf(vi, "%t", str_result);

	//szData[dwRead] = '\0';
	CA2W swIDN(str_result);
	CString csIDN(swIDN);
	sResult = csIDN;
	m_dResult = _tcstod(sResult, 0);

	if (err != VI_SUCCESS) error_handler(vi);

	check_instrument_errors(vi);
}

void CATEView::do_query_string(ViSession vi, CString query)
{
	CW2A pszA(query);
	char* buf_command = pszA;
	do_query_string(vi, buf_command);
}
void CATEView::do_query_string(CString query)
{
	CW2A pszA(query);
	char* buf_command = pszA;
	do_query_string(buf_command);
}

/* Query for a number result.
* --------------------------------------------------------------- */
void CATEView::do_query_number(char *query)
{
	char message[80];

	strcpy(message, query);
	strcat(message, "\n");

	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();

	err = viScanf(vi, "%lf", &num_result);
	if (err != VI_SUCCESS) error_handler();


	//CA2W swIDN( str_result );
	//CString csIDN( swIDN ); 
	//sResult = csIDN;
	m_dResult = num_result;

	check_instrument_errors();
}

/* Query for numbers result.
* --------------------------------------------------------------- */
void CATEView::do_query_numbers(char *query)
{
	char message[80];

	strcpy(message, query);
	strcat(message, "\n");

	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();

	err = viScanf(vi, "%,10lf\n", dbl_results);
	if (err != VI_SUCCESS) error_handler();

	check_instrument_errors();
}

/* Query for an IEEE definite-length block result.
* --------------------------------------------------------------- */
int CATEView::do_query_ieeeblock(char *query)
{
	char message[80];
	int data_length;

	strcpy(message, query);
	strcat(message, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();

	data_length = IEEEBLOCK_SPACE;
	err = viScanf(vi, "%#b\n", &data_length, ieeeblock_data);
	if (err != VI_SUCCESS) error_handler();

	if (data_length == IEEEBLOCK_SPACE)
	{
		printf("IEEE block buffer full: ");
		printf("May not have received all data.\n");
	}

	check_instrument_errors();

	return(data_length);
}

/* Check for instrument errors.
* --------------------------------------------------------------- */
void CATEView::check_instrument_errors()
{
	char str_err_val[256] = { 0 };
	char str_out[800] = "";

	err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
	if (err != VI_SUCCESS) error_handler();

	return;

	while (strncmp(str_err_val, "+0,No error", 3) != 0)
	{
		strcat(str_out, ", ");
		strcat(str_out, str_err_val);
		err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
		if (err != VI_SUCCESS) error_handler();
	}

	if (strcmp(str_out, "") != 0)
	{
		printf("INST Error%s\n", str_out);
		err = viFlush(vi, VI_READ_BUF);
		if (err != VI_SUCCESS) error_handler();
		err = viFlush(vi, VI_WRITE_BUF);
		if (err != VI_SUCCESS) error_handler();
	}
}

void CATEView::check_instrument_errors(ViSession vi)
{
	char str_err_val[256] = { 0 };
	char str_out[800] = "";

	err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
	if (err != VI_SUCCESS) error_handler(vi);

	return;

	//while(strncmp(str_err_val, "+0,No error", 3) != 0 )
	//{
	//	strcat(str_out, ", ");
	//	strcat(str_out, str_err_val);
	//	err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
	//	if (err != VI_SUCCESS) error_handler();
	//}

	//if (strcmp(str_out, "") != 0)
	//{
	//	printf("INST Error%s\n", str_out);
	//	err = viFlush(vi, VI_READ_BUF);
	//	if (err != VI_SUCCESS) error_handler();
	//	err = viFlush(vi, VI_WRITE_BUF);
	//	if (err != VI_SUCCESS) error_handler();
	//}
}

/* Handle VISA errors.
* --------------------------------------------------------------- */
void CATEView::error_handler()
{
	char err_msg[1024] = { 0 };

	viStatusDesc(vi, err, err_msg);
	printf("VISA Error: %s\n", err_msg);
	if (err < VI_SUCCESS)
	{
		//exit(1);
	}
}

void CATEView::error_handler(ViSession vi)
{
	char err_msg[1024] = { 0 };

	viStatusDesc(vi, err, err_msg);
	printf("VISA Error: %s\n", err_msg);
	if (err < VI_SUCCESS)
	{
		//exit(1);
	}
}

void CATEView::set_eload_dynamic(double dIloadA, double dIloadA_time, double dIloadB, double dIloadB_time)
{
	//err = viClear(viLoad);
	//if (err != VI_SUCCESS) error_handler();

	/* Get and display the device's *IDN? string. */
	//do_query_string(viLoad, "*IDN?");
	//printf("Oscilloscope *IDN? string: %s\n", str_result);

	/* Clear status and load the default setup. */
	//do_command(viLoad, "*CLS");
	//do_command(viLoad, "*RST");
	//do_command(viLoad, "CURR:LEV 0.123;\n");
	//do_command(viLoad, "*OUTP OFF\nSOUR:CURR:LEV:IMM 0.02\n");
	//do_command(viLoad, "SOUR:INP ON\n");



	//do_query_string(viLoad, "MEAS:CURR?\n");
	//printf("string: %s\n", str_result);

	//do_query_string(viLoad, "MEAS:VOLT?\n");
	//printf("string: %s\n", str_result);

	do_command(viLoad, "CURR:TRAN:MODE CONTinuous\n");
	//do_command(viLoad, "SOUR:CURR:TRAN:MODE TOGGle\n");
	do_command(viLoad, "TRIGger:SOURce BUS\n");

	CString s;
	s.Format(_T("SOUR:CURR:TRAN:ALEV %f\n"), dIloadA);
	do_command(viLoad, s);
	s.Format(_T("SOUR:CURR:TRAN:BLEV %f\n"), dIloadB);
	do_command(viLoad, s);

	s.Format(_T("SOUR:CURR:TRAN:AWID %f\n"), dIloadA_time);
	do_command(viLoad, s);
	s.Format(_T("SOUR:CURR:TRAN:BWID %f\n"), dIloadB_time);
	do_command(viLoad, s);

	//do_command(viLoad, "SOUR:CURR:TRAN:AWID 0.100\n");	// A width = 100ms

	//do_command(viLoad, "SOUR:CURR:TRAN:ALEV 0.100\n");
	//do_command(viLoad, "SOUR:CURR:TRAN:BLEV 0.400\n");

	//do_command(viLoad, "SOUR:CURR:TRAN:AWID 0.100\n");	// A width = 100ms
	//do_command(viLoad, "SOUR:CURR:TRAN:BWID 0.200\n");	// B width = 200ms

	//do_command(viLoad, "SOUR:CURR:SLEW:POS MAX\n");
	//do_command(viLoad, "SOUR:CURR:SLEW:NEG MIN\n");
	//do_command(viLoad, "TRIG:SOUR\n");
	do_command(viLoad, "TRAN ON\n");
	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");
	//do_command(viLoad, "TRAN OFF\n");
	//do_command(viLoad, "TRAN ON\n");
	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");

	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");
	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");

	do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");

	//do_command(viLoad, "SOUR:INP OFF\n");
}


void CATEView::set_eload_on()
{
	do_command(viLoad, "*TRG\n");
	do_command(viLoad, "SOUR:INP ON\n");
}
void CATEView::set_eload_off()
{
	do_command(viLoad, "SOUR:INP OFF\n");
	do_command(viLoad, "TRAN OFF\n");
}

void CATEView::set_eload_reset()
{

	err = viOpen(defaultRM, VISA_LOAD_ADDR, VI_NULL, VI_NULL, &viLoad);
	if (err != VI_SUCCESS) error_handler();


	err = viSetAttribute(viLoad, VI_ATTR_TMO_VALUE, 15000);

	err = viClear(viLoad);
	if (err != VI_SUCCESS) error_handler();

	do_query_string(viLoad, "*IDN?");
	printf("Oscilloscope *IDN? string: %s\n", str_result);

	/* Clear status and load the default setup. */
	do_command(viLoad, "*CLS");
	do_command(viLoad, "*RST");
	do_command(viLoad, "CURR:LEV 0.123;\n");
	do_command(viLoad, "*OUTP OFF\nSOUR:CURR:LEV:IMM 0.02\n");
	//do_command(viLoad, "SOUR:INP ON\n");

}


double CATEView::get_eload_voltage()
{
	do_query_string(viLoad, "MEAS:VOLT?\n");
	printf("string: %s\n", str_result);
	return m_dResult;
}

double CATEView::get_eload_current()
{
	do_query_string(viLoad, "MEAS:CURR?\n");
	printf("string: %s\n", str_result);
	return m_dResult;
}

void CATEView::set_eload(double Iload)
{
	//err = viClear(viLoad);
	//if (err != VI_SUCCESS) error_handler();

	///* Get and display the device's *IDN? string. */
	//do_query_string(viLoad, "*IDN?");
	//printf("Oscilloscope *IDN? string: %s\n", str_result);

	///* Clear status and load the default setup. */
	//do_command(viLoad, "*CLS");
	//do_command(viLoad, "*RST");
	//do_command(viLoad, "CURR:LEV 0.123;\n");
	//do_command(viLoad, "*OUTP OFF\nSOUR:CURR:LEV:IMM 0.02\n");
	//do_command(viLoad, "SOUR:INP ON\n");

	CString s;
	s.Format(_T("SOUR:CURR:LEV:IMM %f\n"), Iload);
	m_dELoadI_set = Iload * 1000.0;
	do_command(viLoad, s);

	//do_command(viLoad, "TRAN OFF\n");

	do_query_string(viLoad, "MEAS:CURR?\n");
	printf("string: %s\n", str_result);

	do_query_string(viLoad, "MEAS:VOLT?\n");
	printf("string: %s\n", str_result);

	//do_command(viLoad, "CURR:TRAN:MODE CONTinuous\n");
	////do_command(viLoad, "SOUR:CURR:TRAN:MODE TOGGle\n");
	//do_command(viLoad, "TRIGger:SOURce BUS\n");

	//do_command(viLoad, "SOUR:CURR:TRAN:ALEV 0.100\n");
	//do_command(viLoad, "SOUR:CURR:TRAN:BLEV 0.400\n");

	//do_command(viLoad, "SOUR:CURR:TRAN:AWID 0.100\n");	// A width = 100ms
	//do_command(viLoad, "SOUR:CURR:TRAN:BWID 0.200\n");	// B width = 200ms

	//do_command(viLoad, "SOUR:CURR:SLEW:POS MAX\n");
	//do_command(viLoad, "SOUR:CURR:SLEW:NEG MIN\n");
	////do_command(viLoad, "TRIG:SOUR\n");
	//do_command(viLoad, "TRAN ON\n");
	////do_command(viLoad, "SOUR:CURRent:TRIGGer\n");
	//do_command(viLoad, "TRAN OFF\n");
	//do_command(viLoad, "TRAN ON\n");
	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");

	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");
	//do_command(viLoad, "SOUR:CURRent:TRIGGer\n");

	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");
	//do_command(viLoad, "*TRG\n");






	//do_command(viLoad, "SOUR:INP OFF\n");
}
void CATEView::OnBnClickedButtonLoadTest()
{
	err = viOpenDefaultRM(&defaultRM);

	//#define VISA_ADDRESS "USB0::0x0957::0x17A4::MY53160387::0::INSTR"
	//err = viOpen(defaultRM, VISA_ADDRESS, VI_NULL, VI_NULL, &vi);


	set_eload_reset();
	set_eload(0.125);
	set_eload_on();
	double vol = get_eload_voltage();
	double iout = get_eload_current();
	set_eload_on();
	set_eload_off();
	set_eload_on();
	set_eload_dynamic(0.100, 0.3, 0.200, 0.01);
	vol = get_eload_voltage();
	iout = get_eload_current();
	set_eload_off();
	set_eload_dynamic(0.400, 0.03, 0.500, 0.001);
	set_eload(0.325);
	set_eload_on();

}

//void CATEView::log_add(CString s)
//{
//	m_sLog.Append(s);
//}

void CATEView::set_scope_time(double time_offset, double time_scale)
{
	CString s(_T(":TIMebase:SCALe "));
	s.AppendFormat(_T("%f"), time_scale);
	//do_command(":TIMebase:SCALe 0.0000200");	
	do_command(s);

	do_query_string(":TIMebase:SCALe?");
	printf("Timebase scale: %s\n", str_result);

	s.Format(_T(":TIMebase:POSition "));
	s.AppendFormat(_T("%f"), time_offset);
	do_command(s);

	//do_command(":TIMebase:POSition 0.0");
	do_query_string(":TIMebase:POSition?");
	printf("Timebase position: %s\n", str_result);
}


void CATEView::set_scope_trigger_glitch(int nChannel, bool bPolarity, double dLevel, bool bGreater, double dTime)
{

	CString s;
	s.Format(_T(":TRIGger:GLITch:SOURce CHANnel%d"), nChannel);
	do_command(s);

	if (bGreater)
	{
		s.Format(_T(":TRIGger:GLITch:QUALifier GRE"));
		do_command(s);
		s.Format(_T(":TRIGger:GLITch:GRE %f"), dTime);
		do_command(s);
	}
	else
	{
		s.Format(_T(":TRIGger:GLITch:QUALifier LESS"));
		do_command(s);
		s.Format(_T(":TRIGger:GLITch:LESSthan %f"), dTime);
		do_command(s);
	}


	s.Format(_T(":TRIGger:GLITch:LEVel %f"), dLevel);
	do_command(s);


	if (bPolarity)
		s.Format(_T(":TRIGger:GLITch:POLarity POSitive"));
	else
		s.Format(_T(":TRIGger:GLITch:POLarity NEG"));
	do_command(s);


	s.Format(_T(":TRIGger:MODE GLITch"));
	do_command(s);


	///* Set EDGE trigger parameters. */
	//s.Format(_T(":TRIGger:EDGE:SOURCe CHANnel%d"), nChannel);
	////do_command(":TRIGger:EDGE:SOURCe CHANnel1");
	//do_command(s);

	//do_query_string(":TRIGger:EDGE:SOURce?");
	//printf("Trigger edge source: %s\n", str_result);

	//s.Format(_T(":TRIGger:EDGE:LEVel %f"), dTrigger_level);
	////do_command(":TRIGger:EDGE:LEVel 1.5");
	//do_command(s);

	//do_query_string(":TRIGger:EDGE:LEVel?");
	//printf("Trigger edge level: %s\n", str_result);

	//if (bPosEdge)
	//	do_command(":TRIGger:EDGE:SLOPe POSitive");
	//else
	//	do_command(":TRIGger:EDGE:SLOPe NEG");

	//do_query_string(":TRIGger:EDGE:SLOPe?");
	//printf("Trigger edge slope: %s\n", str_result);

}

void CATEView::set_scope_trigger(int nChannel, bool bPosEdge, double dTrigger_level)
{
	CString s;
	/* Set EDGE trigger parameters. */
	s.Format(_T(":TRIGger:EDGE:SOURCe CHANnel%d"), nChannel);
	//do_command(":TRIGger:EDGE:SOURCe CHANnel1");
	do_command(s);

	do_query_string(":TRIGger:EDGE:SOURce?");
	printf("Trigger edge source: %s\n", str_result);

	s.Format(_T(":TRIGger:EDGE:LEVel %f"), dTrigger_level);
	//do_command(":TRIGger:EDGE:LEVel 1.5");
	do_command(s);

	do_query_string(":TRIGger:EDGE:LEVel?");
	printf("Trigger edge level: %s\n", str_result);

	if (bPosEdge)
		do_command(":TRIGger:EDGE:SLOPe POSitive");
	else
		do_command(":TRIGger:EDGE:SLOPe NEG");

	do_query_string(":TRIGger:EDGE:SLOPe?");
	printf("Trigger edge slope: %s\n", str_result);

	s.Format(_T(":TRIGger:MODE EDGE"));
	do_command(s);

}


void CATEView::set_scope_channel(int nChannel, bool bEnable, double dScale, double dPosition_offset, CString sLabel)
{
	CString s;

	if (!bEnable)
	{
		s.Format(_T(":CHANnel%d:DISPlay 0"), nChannel);
		do_command(s);
		return;
	}

	/* Set vertical scale and offset. */
	do_command(":DISPlay:LABel ON");
	s.Format(_T(":CHANnel%d:SCALe %f"), nChannel, dScale);		// 1V
	//do_command(":CHANnel1:SCALe 1.00");		// 1V
	do_command(s);

	s.Format(_T(":CHANnel%d:DISPlay 1"), nChannel);
	do_command(s);

	s.Format(_T(":CHANnel%d:OFFSet %f"), nChannel, dPosition_offset);
	//do_command(":CHANnel1:OFFSet -1.5");
	do_command(s);

	s.Format(_T(":CHANnel%d:LABel \""), nChannel);
	s.Append(sLabel);
	s.AppendFormat(_T("\""));
	//do_command(":CHANnel1:LABel \"VRECT\"");

	m_sChName[nChannel - 1] = sLabel;

	do_command(s);

}

void CATEView::set_scope_mode(bool bRoll, bool bAuto)
{
	if (bAuto)
		do_command(":TRIGger:SWEep AUTO");
	else
		do_command(":TRIGger:SWEep NORMal");

	if (bRoll)
		do_command(":TIMebase:MODE ROLL");
	else
		do_command(":TIMebase:MODE MAIN");

}


void CATEView::set_scope_capture_single()
{
	do_command(_T(":SINGle"));
}


void CATEView::set_scope_capture_run()
{
	do_command(_T(":RUN"));
}

void CATEView::set_scope_capture(int nChannelMask)
{
	//do_command(_T(":SINGle"));
	//return;

	CString s;
	s.Format(_T(":DIGitize "));
	bool bFirstCH = true;
	if (nChannelMask & 0x01)
	{
		s.AppendFormat(_T("CHANnel1"));
		bFirstCH = false;
	}

	if ((nChannelMask & 0x02) && bFirstCH)
	{
		s.AppendFormat(_T("CHANnel2"));
		bFirstCH = false;
	}
	else
		s.AppendFormat(_T(",CHANnel2"));

	if ((nChannelMask & 0x04) && bFirstCH)
	{
		s.AppendFormat(_T("CHANnel3"));
		bFirstCH = false;
	}
	else
		s.AppendFormat(_T(",CHANnel3"));

	if ((nChannelMask & 0x08) && bFirstCH)
		s.AppendFormat(_T("CHANnel4"));
	else
		s.AppendFormat(_T(",CHANnel4"));

	//do_command(":DIGitize CHANnel1,CHANnel2,CHANnel3,CHANnel4");
	do_command(s);
}

void CATEView::save_scope_img(CString sFileName)
{
	double wav_format;
	double acq_type;
	double wav_points;
	double avg_count;
	double x_increment;
	double x_origin;
	double x_reference;
	double y_increment;
	double y_origin;
	double y_reference;

	FILE *fp;
	int num_bytes;   /* Number of bytes returned from instrument. */
	int i;


	/* Download the screen image.
	* ------------------------------------------------------------- */
	do_command(":HARDcopy:INKSaver OFF");

	/* Read screen image. */
	num_bytes = do_query_ieeeblock(":DISPlay:DATA? PNG, COLor");
	printf("Screen image bytes: %d\n", num_bytes);

	/* Write screen image bytes to file. */

	//CString filename(_T("d:\\scope\\data\\"));
	CString filename(_T(""));
	filename = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");
	filename.Append(sFileName);
	filename.AppendFormat(_T(".png"));
	//fp = _wfopen ("d:\\scope\\data\\screen.png", L"wb");
	fp = _wfopen(filename, L"wb");
	num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes, fp);
	fclose(fp);
	printf("Wrote screen image (%d bytes) to ", num_bytes);
	printf("d:\\scope\\data\\screen.bmp.\n");

	//	/* Download waveform data.
	//	* ------------------------------------------------------------- */
	//
	//	/* Set the waveform points mode. */
	//	do_command(":WAVeform:POINts:MODE RAW");
	//	do_query_string(":WAVeform:POINts:MODE?");
	//	printf("Waveform points mode: %s\n", str_result);
	//
	//	/* Get the number of waveform points available. */
	//	do_query_string(":WAVeform:POINts?");
	//	printf("Waveform points available: %s\n", str_result);
	//
	//	/* Set the waveform source. */
	//	do_command(":WAVeform:SOURce CHANnel1");
	//	do_query_string(":WAVeform:SOURce?");
	//	printf("Waveform source: %s\n", str_result);
	//
	//	/* Choose the format of the data returned (WORD, BYTE, ASCII): */
	//	do_command(":WAVeform:FORMat BYTE");
	//	do_query_string(":WAVeform:FORMat?");
	//	printf("Waveform format: %s\n", str_result);
	//
	//	/* Display the waveform settings: */
	//	do_query_numbers(":WAVeform:PREamble?");
	//
	//	wav_format = dbl_results[0];
	//	if (wav_format == 0.0)
	//	{
	//		printf("Waveform format: BYTE\n");
	//	}
	//	else if (wav_format == 1.0)
	//	{
	//		printf("Waveform format: WORD\n");
	//	}
	//	else if (wav_format == 2.0)
	//	{
	//		printf("Waveform format: ASCii\n");
	//	}
	//
	//	acq_type = dbl_results[1];
	//	if (acq_type == 0.0)
	//	{
	//		printf("Acquire type: NORMal\n");
	//	}
	//	else if (acq_type == 1.0)
	//	{
	//		printf("Acquire type: PEAK\n");
	//	}
	//	else if (acq_type == 2.0)
	//	{
	//		printf("Acquire type: AVERage\n");
	//	}
	//	else if (acq_type == 3.0)
	//	{
	//		printf("Acquire type: HRESolution\n");
	//	}
	//
	//	wav_points = dbl_results[2];
	//	printf("Waveform points: %e\n", wav_points);
	//
	//	avg_count = dbl_results[3];
	//	printf("Waveform average count: %e\n", avg_count);
	//
	//	x_increment = dbl_results[4];
	//	printf("Waveform X increment: %e\n", x_increment);
	//
	//	x_origin = dbl_results[5];
	//	printf("Waveform X origin: %e\n", x_origin);
	//
	//	x_reference = dbl_results[6];
	//	printf("Waveform X reference: %e\n", x_reference);
	//
	//	y_increment = dbl_results[7];
	//	printf("Waveform Y increment: %e\n", y_increment);
	//
	//	y_origin = dbl_results[8];
	//	printf("Waveform Y origin: %e\n", y_origin);
	//
	//	y_reference = dbl_results[9];
	//	printf("Waveform Y reference: %e\n", y_reference);
	//
	//	/* Read waveform data. */
	//	num_bytes = do_query_ieeeblock(":WAVeform:DATA?");
	//	printf("Number of data values: %d\n", num_bytes);
	//
	//	/* Open file for output. */
	//	fp = fopen("d:\\scope\\data\\waveform_data.csv", "wb");
	//
	//	/* Output waveform data in CSV format. */
	//	for (i = 0; i < num_bytes - 1; i++)
	//	{
	//		/* Write time value, voltage value. */
	//		fprintf(fp, "%9f, %6f\n",
	//			x_origin + ((float)i * x_increment),
	//			(((float)ieeeblock_data[i] - y_reference) * y_increment) + y_origin);
	//	}
	//
	//	/* Close output file. */
	//	fclose(fp);
	//	printf("Waveform format BYTE data written to ");
	//	printf("d:\\scope\\data\\waveform_data.csv.\n");
}


void CATEView::save_scope_data(int nChannel, CString sFileName, int nPointCnt)
{
	double wav_format;
	double acq_type;
	double wav_points;
	double avg_count;
	double x_increment;
	double x_origin;
	double x_reference;
	double y_increment;
	double y_origin;
	double y_reference;

	FILE *fp;
	int num_bytes;   /* Number of bytes returned from instrument. */
	int i;

	/* Set the waveform points mode. */
	do_command(":WAVeform:POINts:MODE RAW");

	CString s;
	s.Format(_T(":WAVeform:POINts %d"), nPointCnt);
	do_command(s);

	s.Format(_T(":WAVeform:SOURce CHANnel%d"), nChannel);
	do_command(s);

	do_command(":WAVeform:FORMat BYTE");


	CString filename(_T(""));
	//filename = m_sTestWaveDir;
	filename = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");

	filename.Append(sFileName);
	filename.AppendFormat(_T("_"));
	filename.Append(m_sChName[nChannel - 1]);
	filename.AppendFormat(_T("_ch%d.csv"), nChannel);
	fp = _wfopen(filename, _T("wb"));

	float ydata;
	for (i = 0; i < num_bytes - 1; i++)
	{
		/* Write time value, voltage value. */
		ydata = (((float)ieeeblock_data[i] - y_reference) * y_increment) + y_origin;
		fprintf(fp, "%9f, %6f\n",
			x_origin + ((float)i * x_increment),
			ydata);
		m_dCH[nChannel - 1][i] = (float)ydata;
	}

	fclose(fp);
}



void CATEView::save_scope_data(int nChannel, CString sFileName)
{
	double wav_format;
	double acq_type;
	double wav_points;
	double avg_count;
	double x_increment;
	double x_origin;
	double x_reference;
	double y_increment;
	double y_origin;
	double y_reference;

	FILE *fp;
	int num_bytes;   /* Number of bytes returned from instrument. */
	int i;


	///* Download the screen image.
	//* ------------------------------------------------------------- */
	//do_command(":HARDcopy:INKSaver OFF");

	///* Read screen image. */
	//num_bytes = do_query_ieeeblock(":DISPlay:DATA? PNG, COLor");
	//printf("Screen image bytes: %d\n", num_bytes);

	///* Write screen image bytes to file. */

	////fp = _wfopen ("d:\\scope\\data\\screen.png", L"wb");
	//fp = _wfopen (filename, L"wb");
	//num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes, fp);
	//fclose (fp);
	//printf("Wrote screen image (%d bytes) to ", num_bytes);
	//printf("d:\\scope\\data\\screen.bmp.\n");

	/* Download waveform data.
	* ------------------------------------------------------------- */

	/* Set the waveform points mode. */
	do_command(":WAVeform:POINts:MODE RAW");
	do_query_string(":WAVeform:POINts:MODE?");
	printf("Waveform points mode: %s\n", str_result);

	do_command(":WAVeform:POINts 1000");

	/* Get the number of waveform points available. */
	do_query_string(":WAVeform:POINts?");
	printf("Waveform points available: %s\n", str_result);

	/* Set the waveform source. */
	CString s;
	s.Format(_T(":WAVeform:SOURce CHANnel%d"), nChannel);
	do_command(s);
	//do_command(":WAVeform:SOURce CHANnel1");
	do_query_string(":WAVeform:SOURce?");
	printf("Waveform source: %s\n", str_result);

	/* Choose the format of the data returned (WORD, BYTE, ASCII): */
	do_command(":WAVeform:FORMat BYTE");
	do_query_string(":WAVeform:FORMat?");
	printf("Waveform format: %s\n", str_result);

	/* Display the waveform settings: */
	do_query_numbers(":WAVeform:PREamble?");

	wav_format = dbl_results[0];
	if (wav_format == 0.0)
	{
		printf("Waveform format: BYTE\n");
	}
	else if (wav_format == 1.0)
	{
		printf("Waveform format: WORD\n");
	}
	else if (wav_format == 2.0)
	{
		printf("Waveform format: ASCii\n");
	}

	acq_type = dbl_results[1];
	if (acq_type == 0.0)
	{
		printf("Acquire type: NORMal\n");
	}
	else if (acq_type == 1.0)
	{
		printf("Acquire type: PEAK\n");
	}
	else if (acq_type == 2.0)
	{
		printf("Acquire type: AVERage\n");
	}
	else if (acq_type == 3.0)
	{
		printf("Acquire type: HRESolution\n");
	}

	wav_points = dbl_results[2];
	printf("Waveform points: %e\n", wav_points);

	avg_count = dbl_results[3];
	printf("Waveform average count: %e\n", avg_count);

	x_increment = dbl_results[4];
	printf("Waveform X increment: %e\n", x_increment);

	x_origin = dbl_results[5];
	printf("Waveform X origin: %e\n", x_origin);

	x_reference = dbl_results[6];
	printf("Waveform X reference: %e\n", x_reference);

	y_increment = dbl_results[7];
	printf("Waveform Y increment: %e\n", y_increment);

	y_origin = dbl_results[8];
	printf("Waveform Y origin: %e\n", y_origin);

	y_reference = dbl_results[9];
	printf("Waveform Y reference: %e\n", y_reference);

	/* Read waveform data. */
	num_bytes = do_query_ieeeblock(":WAVeform:DATA?");
	printf("Number of data values: %d\n", num_bytes);

	/* Open file for output. */
	//fp = fopen("d:\\scope\\data\\waveform_data.csv", "wb");
	//CString filename(_T("d:\\scope\\data\\"));
	CString filename(_T(""));
	//filename = m_sTestWaveDir;
	filename = m_sTestWaveDir + m_TestList[m_nTestIdx].sCmd + _T("\\");

	filename.Append(sFileName);
	filename.AppendFormat(_T("_"));
	filename.Append(m_sChName[nChannel - 1]);
	filename.AppendFormat(_T("_ch%d.csv"), nChannel);
	fp = _wfopen(filename, _T("wb"));


	/* Output waveform data in CSV format. */
	float ydata;
	for (i = 0; i < num_bytes - 1; i++)
	{
		/* Write time value, voltage value. */
		fprintf(fp, "%9f, %6f\n",
			x_origin + ((float)i * x_increment),
			(((float)ieeeblock_data[i] - y_reference) * y_increment) + y_origin);

		ydata = (((float)ieeeblock_data[i] - y_reference) * y_increment) + y_origin;
		m_CH_data[nChannel - 1][i] = ydata;
	}

	fclose(fp);
}



void CATEView::set_scope_clear_display()
{
	do_command(":DISPlay:CLEar");
	do_command(":DISPlay:PERSistence MIN");
}

void CATEView::set_scope_measure(int nChannel)
{
	CString s;
	s.Format(_T(":MEASure:SOURce CHANnel%d"), nChannel);
	do_command(s);
	////do_command(":MEASure:SOURce CHANnel1");
	do_query_string(":MEASure:SOURce?");
	printf("Measure source: %s\n", str_result);


	do_command(":SYSTem:MENU MEASure");
	do_command(":MEASure:SHOW ON");
	do_command(":MEASure:STATistics:DISPlay OFF");


	//do_command(":MEASure:FREQuency");
	//do_query_number(":MEASure:FREQuency?");
	//printf("Frequency: %.4f kHz\n", num_result / 1000);
	//m_Meas.dFreq = m_dResult;
	//m_dCH[nChannel - 1][2] = m_dResult;

	//do_command(":MEASure:VAMPlitude");
	//do_query_number(":MEASure:VAMPlitude?");
	//printf("Vertical amplitude: %.2f V\n", num_result);
	m_Meas.dAmplitue = m_dResult;

	//do_query_number(":MEASure:RIPPle?");
	//printf("Vertical amplitude: %.2f V\n", num_result);
	//m_Meas.dRipple = m_dResult;

	do_command(":MEASure:VAVerage");
	do_query_number(":MEASure:VAVerage?");
	printf("Vertical amplitude: %.3f V\n", num_result);
	m_Meas.dAverage = m_dResult;
	m_dCH[nChannel - 1][2] = m_dResult;

	do_query_number(":MEASure:VMAX?");
	//printf("Vertical amplitude: %.2f V\n", num_result);
	m_Meas.dMax = m_dResult;
	m_dCH[nChannel - 1][1] = m_dResult;

	do_query_number(":MEASure:VMIN?");
	//printf("Vertical amplitude: %.2f V\n", num_result);
	m_Meas.dMin = m_dResult;
	m_dCH[nChannel - 1][0] = m_dResult;

	//do_query_number(":MEASure:VPP?");
	//printf("Vertical amplitude: %.2f V\n", num_result);
	//m_Meas.dVPP = m_dResult;
}
//main_code

CString CATEView::scope_init()
{
	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) error_handler();
	err = viOpen(defaultRM, VISA_ADDRESS, VI_NULL, VI_NULL, &vi);
	if (err != VI_SUCCESS) error_handler();
	err = viSetAttribute(vi, VI_ATTR_TMO_VALUE, 2500);
	if (err != VI_SUCCESS) error_handler();
	if (err != VI_SUCCESS)
		return _T("");

	initialize();
	CString sName = sResult;
	set_scope_time(0, 1E-3);
	set_scope_trigger(1, true, 2.5);
	set_scope_channel(1, true, 5, 5.0, _T("COMM2"));
	set_scope_channel(2, true, 1, 3.5, _T("VOUT"));
	set_scope_channel(3, true, 2.0, 6.0, _T("VRECT"));
	set_scope_channel(4, true, 0.2, 0.5, _T("IOUT"));
	set_scope_mode(true, false);	// roll mode
	if (sName.GetLength() > 50)
		sName = sName.Left(50);
	return sName;
}

CString CATEView::eload_init()
{
	err = viOpenDefaultRM(&defaultRM);
	err = viOpen(defaultRM, VISA_LOAD_ADDR, VI_NULL, VI_NULL, &viLoad);
	if (err != VI_SUCCESS)
		return _T("");

	err = viClear(viLoad);
	if (err != VI_SUCCESS) error_handler();
	set_eload_reset();

	/* Get and display the device's *IDN? string. */
	do_query_string(viLoad, "*IDN?");
	CString sName = sResult;
	set_eload(0.125);
	set_eload_off();
	if (sName.GetLength() > 50)
		sName = sName.Left(50);
	return sName;
}

CString CATEView::chamber_init()
{

	err = viOpenDefaultRM(&defaultRM);
	err = viOpen(defaultRM, VISA_CHAMBER_ADDR, VI_NULL, VI_NULL, &viChamber);
	if (err != VI_SUCCESS)
		return _T("");

	//err = viClear(viChamber);
	//if (err != VI_SUCCESS) error_handler();
	//set_eload_reset();


	/* Get and display the device's *IDN? string. */
	//do_query_string(viChamber, "*IDN?");
	//CString sName = sResult;
	CString sName = _T("Link OK");
	//if (sName.GetLength() > 50)
		//sName = sName.Left(50);

	/* Clear status and load the default setup. */
	//do_command(viChamber, "*CLS");
	//do_command(viChamber, "*RST");
	//do_command(viChamber, "CURR:LEV 0.123;\n");
	//do_command(viChamber, "*OUTP OFF\nSOUR:CURR:LEV:IMM 0.02\n");

	//do_command(viChamber, "N0\r\n");
	//do_command(viChamber, "T-20,1\r\n");
	do_command(viChamber, "T25,1\r\n");

	return sName;
}

void CATEView::set_chamber(double temp)
{
	CString sTemp;
	sTemp.Format(_T("T%.0f,1\r\n"), temp);
	do_command(viChamber, sTemp);
}

CString CATEView::sm_init(int gpib_addr, ViSession* viSM)
{
	//get_sm_voltage(viSM);
	//get_sm_current(viSM);
	//return L"";

	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) error_handler();

	ViSession vi;
	double set_v;
	double set_i;
	if (gpib_addr == 25)
	{
		set_v = 5.0;
		set_i = 0.3;
		err = viOpen(defaultRM, VISA_SM25_ADDR, VI_NULL, VI_NULL, viSM);
	}
	else if (gpib_addr == 26)
	{
		set_v = 1.0;
		set_i = 0.01;
		err = viOpen(defaultRM, VISA_SM26_ADDR, VI_NULL, VI_NULL, viSM);
	}
	else if (gpib_addr == 27)
	{
		set_v = 1.0;
		set_i = 0.01;
		err = viOpen(defaultRM, VISA_SM27_ADDR, VI_NULL, VI_NULL, viSM);
	}
	else
		return _T("");

	if (err != VI_SUCCESS)
		return _T("");

	err = viClear(*viSM);
	if (err != VI_SUCCESS) error_handler();

	/* Get and display the device's *IDN? string. */
	do_query_string(*viSM, "*IDN?");
	CString sName = sResult;

	/* Clear status and load the default setup. */
	do_command(*viSM, "*CLS");
	do_command(*viSM, "*RST");

	do_command(*viSM, _T("SOUR:FUNC VOLT\n:SENSe:CURRent:RANGe:AUTO ON\n:SENSe:VOLTage:RANGe:AUTO ON\n"));
	do_command(*viSM, _T(":SOUR:VOLTage:RANGe 20\n"));
	//do_Icommand(*viSM, _T("SENSe:CURRent:RANGe:AUTO ON\n:SENSe:VOLTage:RANGe:AUTO\n"));
	//do_command(*viSM, _T("SENSe:VOLTage:RANGe:AUTO ON\n"));

	//do_command(*viSM, _T(":SYSTem:RCMode SING"));
	//do_command(viSM, "SOUR:CURR:LEV 0.123;\n");
	//do_command(viSM, ":OUTP OFF\nSOUR:CURR:LEV 0.0123\n");
	//do_command(viSM, "SOUR:VOLT:LEV:IMM 5.0\n");	// command ok

	//
	//do_command(viSM, "SOUR:VOLT:LEV:IMM 4.5\n");	// command ok
	//do_command(viSM, "SOUR:CURR:LEV 0.0125\n");	// no warning
	//do_command(viSM, ":OUTP ON");
	//do_command(viSM, ":OUTP OFF");

	//do_query_string(viSM, ":MEAS:VOLT?");
	//CString sVolt = sResult;
	//sVolt = L"+8.006929E-01,+9.910000E+37,+9.910000E+37,+7.411652E+03,+1.946000E+04";
	//do_query_string(viSM, ":MEAS:CURR?");
	//CString sCurr = sResult;
	//sCurr = L"+8.000000E-01,+5.154412E-06,+9.910000E+37,+7.424856E+03,+2.150800E+04";

	//------------------------------------------------------------
	//set_sm_vi(*viSM, 5.1, 0.03);

	//set_sm_vi(*viSM, 1.0, 0.010);
	//set_sm_vi(*viSM, 5.0, 0.050); ok
	set_sm_vi(*viSM, set_v, set_i);

	//set_sm_vi(*viSM, 4.9, 0.1);
	set_sm_on(*viSM);
	set_sm_off(*viSM);
	//set_sm_on(*viSM);

	double vSM = get_sm_voltage(*viSM);
	double iSM = get_sm_current(*viSM);
	//------------------------------------------------------------

	if (sName.GetLength() > 50)
		sName = sName.Left(50);
	return sName;
}

void CATEView::set_sm_on(ViSession viSM)
{
	do_command(viSM, ":OUTP ON\n");
}
void CATEView::set_sm_off(ViSession viSM)
{
	do_command(viSM, ":OUTP OFF\n");
}


void CATEView::set_sm_beep_once(ViSession viSM)
{
	do_command(viSM, _T(":SYSTem:BEEP:IMM 1000,0.1"));
}
void CATEView::set_sm_beep(ViSession viSM, bool bBeep)
{
	if (bBeep)
	{
		do_command(viSM, _T(":SYSTem:BEEPer:STAT ON"));
		do_command(viSM, _T(":SYSTem:BEEP:IMM 2000,0.2"));
	}
	else
		do_command(viSM, _T(":SYSTem:BEEPer:STAT OFF"));
}

void CATEView::set_sm_vi(ViSession viSM, double dVolt, double dIlim)
{
	CString s;
	s.Format(_T("SOUR:VOLT:LEV:IMM %f\nSENS:CURR:PROT:LEV %f\n"), dVolt, dIlim);
	//do_command(viSM, "SENS:CURR:PROT:LEV 0.200;\n");
	//do_command(viSM, ":OUTP OFF\nSOUR:CURR:LEV 0.0123\n");
	//do_command(viSM, "SOUR:VOLT:LEV:IMM 5.0\n");	// command ok
	do_command(viSM, s);

	//s.Format(_T("SOUR:CURR:LEV %f\n"), dIlim);
	//do_command(viSM, s);
}

double CATEView::get_sm_voltage(ViSession viSM)
{


	CString sVolt;
	if (viSM == NULL)
		sVolt = L"";
	else
		do_query_string(viSM, ":MEAS:VOLT?");
	sVolt = sResult;
	//sVolt = L"+8.006929E-01,+9.910000E+37,+9.910000E+37,+7.411652E+03,+1.946000E+04";
	int pos1 = sVolt.Find(L',');
	sVolt = sVolt.Left(pos1);
	double dVolt = _tcstod(sVolt, 0);

	if (viSM == viSM25)
		m_dSourceMeterV[0] = dVolt;
	else if (viSM == viSM26)
		m_dSourceMeterV[1] = dVolt;
	else
		m_dSourceMeterV[2] = dVolt;

	return dVolt;
}

double CATEView::get_sm_current(ViSession viSM)
{
	CString sCurr;
	if (viSM == NULL)
		sCurr = L"";
	else
		do_query_string(viSM, ":MEAS:CURR?");
	sCurr = sResult;
	//sCurr = L"+8.000000E-01,+5.154412E-06,+9.910000E+37,+7.424856E+03,+2.150800E+04";
	int pos1 = sCurr.Find(L',');
	int pos2 = sCurr.Find(L',', pos1 + 2);
	sCurr = sCurr.Mid(pos1 + 1, pos2 - pos1 - 1);
	double dCurr = _tcstod(sCurr, 0);

	if (viSM == viSM25)
		m_dSourceMeterI[0] = dCurr;
	else if (viSM == viSM26)
		m_dSourceMeterI[1] = dCurr;
	else
		m_dSourceMeterI[2] = dCurr;

	return dCurr;
}

void CATEView::initial_top_html()
{
	CString sWave;
	CTime	m_time = CTime::GetCurrentTime();

	// generate html front part
	//------------------------------------------------------------
	m_sTop = read_html_preset(_T("pre_define_css.pset"));
	m_sTop += gen_html_title(m_time);
	m_sTop += gen_html_test_list();
	//m_sTop += read_html_preset(_T("pre_define_fod.pset"));
	//sTop = read_html_preset(_T("pre_define.pset"));
	//------------------------------------------------------------


	m_sTestDir.Format(_T("RT7800_test_report"));
	//m_sTestDir.AppendFormat(_T("_%04d_%02d_%02d_%02d%02d%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	//if (GetFileAttributes(m_sTestDir) == INVALID_FILE_ATTRIBUTES) 
	//{

	//	CreateDirectory(m_sTestDir, NULL);
	//	copy_my_files();
	//}

	//if (!m_bCopyFileDone)
	//	copy_my_files();

	CString sOthersDir;
	m_sTestDir.AppendFormat(_T("\\"));

	//CString sTest_VBUS_conn = m_sTestDir + _T("evb\\vbus_ovp");
	CString sTest_VBUS_conn = m_sTestDir + m_TestList[m_nTestIdx].sFolderPath;
	//CString sConnPNG = gen_zoom_png_html(sTest_VBUS_conn, _T("id_evb_conn"), _T("Connection"), 0x808080);
	//m_sTop += sConnPNG;

	m_sTestWaveDir = m_sTestDir;
	m_sTestWaveDir.AppendFormat(_T("waveform"));
	m_sTestWaveDir.AppendFormat(_T("\\"));


	if (!m_bCopyFileDone)
	{
		CString sItemWaveFolder;
		for (int i = 0; i < m_nTotalTestItemCnt; i++)
		{
			if (m_TestList[i].tm != TEST_MODE::FOLDER)
			{
				sItemWaveFolder = m_sTestWaveDir + m_TestList[i].sCmd;

				if (GetFileAttributes(sItemWaveFolder) == INVALID_FILE_ATTRIBUTES)
					CreateDirectory(sItemWaveFolder, NULL);
			}
		}
	}

	sOthersDir = m_sTestDir;
	sOthersDir.AppendFormat(_T("\\others"));
	sOthersDir.AppendFormat(_T("\\"));
	m_bCopyFileDone = true;
}

void CATEView::OnBnClickedButtonLoadScope()
{
	CString sWave, sTop;
	CTime	m_time = CTime::GetCurrentTime();

	// generate html front part
	//------------------------------------------------------------
	sTop = read_html_preset(_T("pre_define_css.pset"));
	sTop += gen_html_title(m_time);

	sTop += gen_html_test_list();

	sTop += read_html_preset(_T("pre_define_fod.pset"));
	//sTop = read_html_preset(_T("pre_define.pset"));
	//------------------------------------------------------------


	//m_sTestDir.Format(_T("RT7800_test"));
	m_sTestDir.Format(_T("RT7800_test_report"));
	m_sTestDir.AppendFormat(_T("_%04d_%02d_%02d_%02d%02d%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
	copy_my_files();

	//bool bOld_path= true;
	CString sOthersDir;
	//bool bOld_path= false;
	//if (bOld_path)
	//{
	//	// for test
	//	m_sTestDir.Format(_T("RT1650_test_2015_09_22_133936"));

	//	m_sTestDir.AppendFormat(_T("\\"));

	//	m_sTestWaveDir = m_sTestDir;
	//	m_sTestWaveDir.AppendFormat( _T("\\waveform"));
	//	m_sTestWaveDir.AppendFormat(_T("\\"));
	//}
	//else
	//{
	//	CreateDirectory( m_sTestDir, NULL);
	m_sTestDir.AppendFormat(_T("\\"));

	m_sTestWaveDir = m_sTestDir;
	m_sTestWaveDir.AppendFormat(_T("waveform"));
	//	CreateDirectory( m_sTestWaveDir, NULL);
	m_sTestWaveDir.AppendFormat(_T("\\"));

	sOthersDir = m_sTestDir;
	sOthersDir.AppendFormat(_T("\\others"));
	//	CreateDirectory( sOthersDir, NULL);
	sOthersDir.AppendFormat(_T("\\"));
	//}

	//LPCTSTR sCopyFiles[] = {
	//	_T("Test_connection.png"),
	//	_T("function_block.png"),
	//	_T("typical_application.png"),
	//	_T("")
	//};

	//CString sCopyFileDest;
	//for(int i=0; i<100; i++)
	//{
	//	if (sCopyFiles[i] == _T(""))
	//		break;
	//	sCopyFileDest = sOthersDir + sCopyFiles[i];
	//	CopyFile( sCopyFiles[i], sCopyFileDest, FALSE);
	//}

	CString sFileName;
	double time_offset = 0.001;
	double time_scale = 0.002;
	CString sIout;

	struct { LPCTSTR sText; double dOffset; double dTimeScale; } arrZoom[] =
	{
		{ _T("500ms"), 16E-3, 500E-3 },
		{ _T("50ms"), 0, 50E-3 },
		{ _T("50ms(2)"), 16E-3, 50E-3 },
		{ _T("10ms"), 0, 10E-3 },
		{ _T("5ms"), 16E-3, 5E-3 },
		{ _T("200us"), 0, 200E-6 },
		{ _T("10us"), 0, 1E-5 },
		{ _T("10us(2)"), 78E-6, 10E-6 },
		{ _T(""), -1, -1 },
	};
	int nZoomCnt;
	for (int i = 0; i < 1000; i++)
	if (arrZoom[i].dTimeScale < 0)
	{
		nZoomCnt = i;
		break;
	}

	struct { LPCTSTR sText; double dOffset; double dTimeScale; } Zoom_LC[] =
	{
		{ _T("50ms"), 0, 50E-3 },
		{ _T("10ms"), 0, 10E-3 },
		{ _T("5ms"), 0, 5E-3 },
		{ _T("1ms"), 0, 1E-3 },
		{ _T("500us"), 0, 500E-6 },
		{ _T("100us"), 0, 100E-6 },
		{ _T("10us"), 0, 10E-6 },
		{ _T(""), -1, -1 },
	};
	int nZoom_LC = 7;

	struct { LPCTSTR sText; double dOffset; double dTimeScale; } arrDynamic[] =
	{
		{ _T("3_packet"), 16E-3, 50E-3 },
		{ _T("30_packet"), 16E-3, 500E-3 },
		{ _T("100_packet"), 16E-3, 1500E-3 },
		{ _T(""), -1, -1 },
	};

	for (int i = 0; i < 500; i++)
	{
		m_arrVrect[i] = 0;
		m_arrIout[i] = 0;
		m_arrVout[i] = 0;
		m_arrRP[i] = 0;
		m_arrTS[i] = 0;
		m_arrFreq[i] = 0;
	}

	int arr_idx = 0;
	//------------------------------------------------------------
	//for(double dIout=0; dIout<801; dIout+=100.0)
	//{
	//	sIout.Format( _T("steady_Iout=%.0fmA "), dIout);

	//	for(int i=0; i<8; i++)
	//	{
	//		sFileName = sIout;
	//		sFileName.AppendFormat( _T("zoom="));
	//		sFileName.Append( arrZoom[i].sText );

	//		sFileName.AppendFormat(_T(".png"));
	//		sWave = gen_html_waveform( sFileName, i+1, 8);
	//		sTop.Append( sWave);
	//	}
	//	sTop.AppendFormat( _T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));
	//}
	//return;
	//------------------------------------------------------------


	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) error_handler();

	err = viOpen(defaultRM, VISA_ADDRESS, VI_NULL, VI_NULL, &vi);
	if (err != VI_SUCCESS) error_handler();

	err = viSetAttribute(vi, VI_ATTR_TMO_VALUE, 15000);
	if (err != VI_SUCCESS) error_handler();

	err = viOpen(defaultRM, VISA_LOAD_ADDR, VI_NULL, VI_NULL, &viLoad);

	initialize();


	set_scope_time(time_offset, time_scale);

	set_scope_trigger(1, true, 2.5);

	set_scope_channel(1, true, 5, 5.0, _T("COMM2"));
	set_scope_channel(2, true, 1, 3.5, _T("VOUT"));
	set_scope_channel(3, true, 2.0, 6.0, _T("VRECT"));
	set_scope_channel(4, true, 0.2, 0.5, _T("IOUT"));

	set_scope_mode(true, false);	// roll mode
	set_scope_mode(false, true);	// auto mode
	set_scope_mode(false, false);	// normal mode


	if (!is_scope_triggered())
	{
		set_scope_mode(false, true);	// auto mode
		set_scope_measure(2);
		set_scope_measure(3);
		set_scope_measure(4);
		set_scope_mode(false, false);	// normal mode
	}
	else
	{
		set_scope_measure(2);
		set_scope_measure(3);
		set_scope_measure(4);
	}

	set_scope_trigger_glitch(1, false, 2.0, true, 100E-6);


	//------------------------------------------------------------
	// E-Load
	//------------------------------------------------------------
	set_eload_reset();
	set_eload(0.125);
	set_eload_off();
	double vol = get_eload_voltage();
	double iout = get_eload_current();
	//set_eload_dynamic(0.100, 0.3, 0.200, 0.01);
	//set_eload_off();
	//set_eload_dynamic(0.400, 0.03, 0.500, 0.001);
	//set_eload(0.325);
	//set_eload_on();
	//------------------------------------------------------------


	//------------------------------------------------------------
	// FOD Test
	//------------------------------------------------------------
	set_eload_on();
	for (double dIout = 0; dIout < 801; dIout += 8.0)
	{
		if (m_TestList[m_nTestIdx].tm != TEST_MODE::CC_LOAD)
			continue;

		dIout = m_TestList[m_nTestIdx].Iload1;

		sIout.Format(_T("Iload= %.0fmA"), dIout);
		ASSERT(sIout == m_TestList[m_nTestIdx].sText);

		set_eload(dIout / 1000.0);
		//MyDelay(1200);
		MyDelay(200);

		set_scope_annotation(1, sIout);
		read_ic_inform();
		m_arrVrect[arr_idx] = m_dVrect;
		m_arrIout[arr_idx] = m_dIout;
		m_arrVout[arr_idx] = m_dVout;
		m_arrRP[arr_idx] = m_dRP;
		m_arrTS[arr_idx] = m_dTS;
		m_arrFreq[arr_idx] = m_dFreq;
		m_ELoadV[arr_idx] = get_eload_voltage();
		m_ELoadI[arr_idx] = get_eload_current();
		arr_idx++;


		for (int i = 0; i < nZoomCnt; i++)
		{
			if (arrZoom[i].dTimeScale < 0)
				break;

			sFileName = sIout;
			sFileName.AppendFormat(_T(" zoom="));
			sFileName.Append(arrZoom[i].sText);


			set_scope_time(arrZoom[i].dOffset, arrZoom[i].dTimeScale);
			set_scope_capture(0x7);

			set_scope_measure(1);
			set_scope_measure(2);
			set_scope_measure(3);
			set_scope_measure(4);


			read_ic_inform();
			m_dELoadV = get_eload_voltage();
			m_dELoadI = get_eload_current() * 1000.0;

			save_scope_img(sFileName);
			//MyDelay(4000);

			sFileName.AppendFormat(_T(".png"));

			//save_scope_data(1, sFileName);
			//save_scope_data(2, sFileName);
			//save_scope_data(3, sFileName);
			//save_scope_data(4, sFileName);

			//sWave = gen_html_waveform( sFileName, i+1, 8);
			sTop.Append(sWave);
		}
		sTop.AppendFormat(_T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));
	}
	set_eload_off();




	//------------------------------------------------------------
	// CC_Load Test
	//------------------------------------------------------------
	set_eload_on();
	for (double dIout = 0; dIout < 801; dIout += 10.0)
		//for(m_nTestIdx=0; m_nTestIdx<m_nTotalTestItemCnt; m_nTestIdx++)
	{
		//for(double dIout=0; dIout<601; dIout+=100.0)
		//{
		if (m_TestList[m_nTestIdx].tm != TEST_MODE::CC_LOAD)
			continue;

		dIout = m_TestList[m_nTestIdx].Iload1;

		sIout.Format(_T("Iload= %.0fmA"), dIout);
		ASSERT(sIout == m_TestList[m_nTestIdx].sText);

		set_eload(dIout / 1000.0);
		//MyDelay(1200);
		MyDelay(200);

		set_scope_annotation(1, sIout);
		read_ic_inform();
		m_arrVrect[arr_idx] = m_dVrect;
		m_arrIout[arr_idx] = m_dIout;
		m_arrVout[arr_idx] = m_dVout;
		m_arrRP[arr_idx] = m_dRP;
		m_arrTS[arr_idx] = m_dTS;
		m_arrFreq[arr_idx] = m_dFreq;
		m_ELoadV[arr_idx] = get_eload_voltage();
		m_ELoadI[arr_idx] = get_eload_current();
		arr_idx++;


		for (int i = 0; i < nZoomCnt; i++)
		{
			if (arrZoom[i].dTimeScale < 0)
				break;

			sFileName = sIout;
			sFileName.AppendFormat(_T(" zoom="));
			sFileName.Append(arrZoom[i].sText);


			set_scope_time(arrZoom[i].dOffset, arrZoom[i].dTimeScale);
			set_scope_capture(0x7);

			set_scope_measure(1);
			set_scope_measure(2);
			set_scope_measure(3);
			set_scope_measure(4);


			read_ic_inform();
			m_dELoadV = get_eload_voltage();
			m_dELoadI = get_eload_current() * 1000.0;

			save_scope_img(sFileName);
			//MyDelay(4000);

			sFileName.AppendFormat(_T(".png"));

			save_scope_data(1, sFileName);
			save_scope_data(2, sFileName);
			save_scope_data(3, sFileName);
			//save_scope_data(4, sFileName);

			sWave = gen_html_waveform(sFileName, i + 1, 8);
			sTop.Append(sWave);
		}
		sTop.AppendFormat(_T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));
	}
	set_eload_off();



	//------------------------------------------------------------
	// CC_Dynamic_Load Test
	//------------------------------------------------------------
	set_eload_on();
	for (m_nTestIdx = 0; m_nTestIdx < m_nTotalTestItemCnt; m_nTestIdx++)
	{
		if (m_TestList[m_nTestIdx].tm != TEST_MODE::CC_DYNAMIC_LOAD)
			continue;

		double dIout1 = m_TestList[m_nTestIdx].Iload1;
		double dIout2 = m_TestList[m_nTestIdx].Iload2;
		double time1 = m_TestList[m_nTestIdx].para1;
		double time2 = m_TestList[m_nTestIdx].para2;

		sIout.Format(_T("Iload= %.0fmA(%0.fms) to %.0fmA(%.0fms)"), dIout1, time1, dIout2, time2);
		ASSERT(sIout == m_TestList[m_nTestIdx].sText);

		set_eload_dynamic(dIout1 / 1000, time1 / 1000, dIout2 / 1000, time2 / 1000);

		//MyDelay(1200);
		MyDelay(200);

		set_scope_annotation(1, sIout);
		read_ic_inform();
		m_arrVrect[arr_idx] = m_dVrect;
		m_arrIout[arr_idx] = m_dIout;
		m_arrVout[arr_idx] = m_dVout;
		m_arrRP[arr_idx] = m_dRP;
		m_arrTS[arr_idx] = m_dTS;
		m_arrFreq[arr_idx] = m_dFreq;
		m_ELoadV[arr_idx] = get_eload_voltage();
		m_ELoadI[arr_idx] = get_eload_current();
		arr_idx++;


		for (int i = 0; i < nZoomCnt; i++)
		{
			if (arrZoom[i].dTimeScale < 0)
				break;

			sFileName = sIout;
			sFileName.AppendFormat(_T(" zoom="));
			sFileName.Append(arrZoom[i].sText);


			set_scope_time(arrZoom[i].dOffset, arrZoom[i].dTimeScale);
			set_scope_capture(0x7);

			set_scope_measure(1);
			set_scope_measure(2);
			set_scope_measure(3);
			set_scope_measure(4);


			read_ic_inform();
			m_dELoadV = get_eload_voltage();
			m_dELoadI = get_eload_current() * 1000.0;

			save_scope_img(sFileName);
			//MyDelay(4000);

			sFileName.AppendFormat(_T(".png"));
			save_scope_data(1, sFileName);
			save_scope_data(2, sFileName);
			save_scope_data(3, sFileName);
			//save_scope_data(4, sFileName);

			sWave = gen_html_waveform(sFileName, i + 1, 8);
			sTop.Append(sWave);
		}
		sTop.AppendFormat(_T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));
	}
	set_eload_off();

	//------------------------------------------------------------
	// Load Change : check Analog load transition 
	//------------------------------------------------------------

	set_scope_trigger_glitch(1, false, 2.0, true, 100E-6);
	set_scope_persistence_display();
	set_scope_clear_display();

	set_scope_trigger_edge(1, true, 0.5);
	set_scope_trigger_edge(4, true, 0.5);

	set_scope_channel(4, true, 0.2, 0.5, _T("IOUT"));

	set_eload_reset();
	set_eload_on();
	for (m_nTestIdx = 0; m_nTestIdx < m_nTotalTestItemCnt; m_nTestIdx++)
	{
		if (m_TestList[m_nTestIdx].tm != TEST_MODE::LOAD_CHANGE)
			continue;

		sIout.Format(_T("LC Iload= %.0fmA to %.0fmA"), m_TestList[m_nTestIdx].Iload1, m_TestList[m_nTestIdx].Iload2);
		ASSERT(sIout == m_TestList[m_nTestIdx].sText);

		set_eload_dynamic(
			m_TestList[m_nTestIdx].Iload1 / 1000.0, 0.5,
			m_TestList[m_nTestIdx].Iload2 / 1000.0, 0.5
			);

		MyDelay(200);

		set_scope_annotation(1, sIout);
		//read_ic_inform();
		//m_arrVrect[arr_idx] = m_dVrect;
		//m_arrIout[arr_idx] = m_dIout;
		//m_arrVout[arr_idx] = m_dVout;
		//m_arrRP[arr_idx] = m_dRP;
		//m_arrTS[arr_idx] = m_dTS;
		//m_arrFreq[arr_idx] = m_dFreq;
		//m_ELoadV[arr_idx] = get_eload_voltage();
		//m_ELoadI[arr_idx] = get_eload_current();
		//arr_idx ++;


		for (int i = 0; i < nZoom_LC; i++)
		{
			if (Zoom_LC[i].dTimeScale < 0)
				break;

			sFileName = sIout;
			sFileName.AppendFormat(_T(" zoom="));
			sFileName.Append(arrZoom[i].sText);

			set_scope_time(Zoom_LC[i].dOffset, Zoom_LC[i].dTimeScale);

			set_scope_capture(0xF);

			set_scope_measure(1);
			set_scope_measure(2);
			set_scope_measure(3);
			set_scope_measure(4);

			save_scope_img(sFileName);
			sFileName.AppendFormat(_T(".png"));

			save_scope_data(1, sFileName);
			save_scope_data(2, sFileName);
			save_scope_data(3, sFileName);
			save_scope_data(4, sFileName);

			sWave = gen_html_waveform(sFileName, i + 1, 8);
			sTop.Append(sWave);
		}
		sTop.AppendFormat(_T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));
	}
	set_eload_off();



	//------------------------------------------------------------
	// Find Edge test
	//------------------------------------------------------------
	set_eload_on();
	for (m_nTestIdx = 0; m_nTestIdx<m_nTotalTestItemCnt; m_nTestIdx++)
	{
		if (m_TestList[m_nTestIdx].tm != TEST_MODE::FIND_EDGE)
			continue;

		double dIout = m_TestList[m_nTestIdx].Iload1;
		int nCH = (int)m_TestList[m_nTestIdx].para1;

		bool bPosEdge = m_TestList[m_nTestIdx].para2 > 0;
		if (bPosEdge)
			sIout = _T("Find Edge ") + m_sChName[nCH - 1] + _T(" pos edge");
		else
			sIout = _T("Find Edge ") + m_sChName[nCH - 1] + _T(" neg edge");
		sIout.AppendFormat(_T(" %.0fmA"), dIout);
		ASSERT(sIout == m_TestList[m_nTestIdx].sText);

		set_eload(dIout / 1000.0);
		//MyDelay(1200);
		MyDelay(200);



		CString sCurTri;
		bool bEverFind = false;
		for (double dTri = 8.5; dTri > 5; dTri -= 0.1)
		{
			sCurTri = sIout;
			sCurTri.AppendFormat(_T(" lev=%.2fV"), dTri);
			set_scope_annotation(nCH, sCurTri);

			set_scope_trigger_edge(nCH, bPosEdge, dTri);
			bool bTriggered = false;
			for (int i = 0; i < nZoom_LC; i++)
			{
				if (Zoom_LC[i].dTimeScale < 0)
					break;

				sFileName = sIout;
				sFileName.AppendFormat(_T(" zoom="));
				sFileName.Append(Zoom_LC[i].sText);


				bTriggered = false;
				set_scope_time(Zoom_LC[i].dOffset, Zoom_LC[i].dTimeScale);

				is_scope_triggered();
				is_scope_triggered();
				MyDelay(10);
				set_scope_capture_single();
				is_scope_triggered();
				set_scope_capture_single();

				MyDelay(500);
				for (int wait = 0; wait < 50; wait++)
				{
					if (is_scope_triggered())
					{
						bTriggered = true;
						break;
					}
					MyDelay(100);
				}
				if (!bTriggered)
					break;

				set_scope_measure(1);
				set_scope_measure(2);
				set_scope_measure(3);
				set_scope_measure(4);
				read_ic_inform();
				m_dELoadV = get_eload_voltage();
				m_dELoadI = get_eload_current() * 1000.0;

				save_scope_img(sFileName);
				sFileName.AppendFormat(_T(".png"));


				sWave = gen_html_waveform(sFileName, i + 1, 8);
				sTop.Append(sWave);
			}
			if (bEverFind && !bTriggered)
				break;
			bEverFind |= bTriggered;

		}
		sTop.AppendFormat(_T("<table  bgcolor=\"#6900b3\" width=\"100%%\" height=\"10pt\"> <tr><td></td></tr> </table>\n\n"));

	}
	set_eload_off();




	sTop.AppendFormat(_T("</body>\n</html>"));

	CStdioFile file;
	CString sFile2 = m_sTestDir;
	sFile2.AppendFormat(_T("test_report.html"));
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
		return;
	}
	file.WriteString(sTop);
	file.Close();


	return;








	double test3[] = { 3, 3, 3, };
	CString s2;

	s2.Format(_T(""));

	int nLength = 80;
	s2.AppendFormat(_T("double m_dVrect[%d] = {	// unit: V\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_arrVrect[i]);
	s2.AppendFormat(_T("};\n\n"));

	s2.AppendFormat(_T("double m_dVout[%d] = {	// unit: mA\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_arrVout[i]);
	s2.AppendFormat(_T("};\n\n"));

	s2.AppendFormat(_T("double m_dIout[%d] = {	// unit: mA\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_arrIout[i]);
	s2.AppendFormat(_T("};\n\n"));

	s2.AppendFormat(_T("double m_dRP[%d] = {	// unit: mW\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_arrRP[i]);
	s2.AppendFormat(_T("};\n\n"));

	s2.AppendFormat(_T("\n// From E-load\n"));
	s2.AppendFormat(_T("double m_ELoadI[%d] = {	// unit: mA\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_ELoadI[i]);
	s2.AppendFormat(_T("};\n\n"));

	s2.AppendFormat(_T("double m_ELoadV[%d] = {	// unit: V\n"), nLength);
	for (int i = 0; i < 80; i++)
		s2.AppendFormat(_T("\t%.6f,\n"), m_ELoadV[i]);
	s2.AppendFormat(_T("};\n\n"));


	double dIout_start = 0.0;
	double Atime = 100E-3;
	double Btime = 200E-3;
	for (double dIout = 100; dIout < 501; dIout += 100.0)
	{
		sIout.Format(_T("dynamic_Iout=%.0fmA(%.0fms) -%.0fmA(%.0fms) ")
			, dIout_start, Atime * 1000, dIout, Btime * 1000);

		set_eload_dynamic(dIout_start / 1000.0, Atime, dIout / 1000.0, Btime);
		set_eload_on();

		set_scope_annotation(1, sIout);

		for (int i = 0; i < 1000; i++)
		{
			if (arrZoom[i].dTimeScale < 0)
				break;

			sFileName = sIout;
			sFileName.AppendFormat(_T("zoom="));
			sFileName.Append(arrZoom[i].sText);

			set_scope_time(arrZoom[i].dOffset, arrZoom[i].dTimeScale);
			set_scope_capture(0x7);
			save_scope_img(sFileName);
			MyDelay(4000);
		}
	}

	set_scope_mode(true, false);	// roll mode
	set_eload_off();

	viClose(vi);
	viClose(defaultRM);

}


void CATEView::set_scope_annotation(int nChannel, CString sNote)
{
	do_command(":DISPlay:ANNotation ON");
	CString s;
	s.Format(_T(":DISPlay:ANNotation:COLor CH%d"), nChannel);
	do_command(s);
	//s.Format(":DISPlay:ANNotation:TEXT \"TEXT this is a annotation for chanel 2\"");
	s.Format(_T(":DISPlay:ANNotation:TEXT \""));
	s.Append(sNote);
	s.AppendFormat(_T("\""));
	do_command(s);

}



void CATEView::read_ic_inform()
{
	g_bShow_i2c_log = false;
	bool m_bSTOP = false;
	MSG msg;
	CString s;


	return;

	//while(!m_bSTOP)
	{
		//while(0)
		//{
		//	int reg = i2c_rd(0x44, 0x10);
		//	if (reg != 0xF0)
		//		int tet1 = 3;
		//	if (m_bSTOP)
		//		break;
		//}
		//------------------------------------------------------------

		// Vrect
		//------------------------------------------------------------
		m_dVrect = 4.0 + (double)i2c_rd(0x44, 0x64) * (8 - 4) / 255;
		if (m_dVrect == 8.0)
			s.AppendFormat(_T("\tVrect >= 8.0 V\n"));
		else if (m_dVrect <= 4.0)
			s.AppendFormat(_T("\tVrect <= 4.0 V\n"));
		else
			s.AppendFormat(_T("\tVrect = %.2f V\n"), m_dVrect);
		//------------------------------------------------------------

		// Iout
		//------------------------------------------------------------
		double Iout = (double)i2c_rd(0x44, 0x67) * (2000 - 0) / 255;
		s.AppendFormat(_T("\tIout = %.2f mA\n"), Iout);
		m_dIout = Iout;
		//------------------------------------------------------------

		// Vout
		//------------------------------------------------------------
		bool bVoutEn = i2c_rd(0x44, 0x10) & 0x80;
		if (bVoutEn)
			s.AppendFormat(_T("\tVout enable : "));
		else
			s.AppendFormat(_T("\tVout disable : "));
		m_dVout = 3.0 + (double)i2c_rd(0x44, 0x66) * (6 - 3) / 255;
		if (m_dVout == 6.0)
			s.AppendFormat(_T("Vout >= 6.0 V\n"));
		else if (m_dVout <= 3.0)
			s.AppendFormat(_T("Vout <= 3.0 V\n"));
		else
			s.AppendFormat(_T("Vout = %.2f V\n"), m_dVout);
		//------------------------------------------------------------

		// CE & RP
		//------------------------------------------------------------
		int reg_0x78 = i2c_rd(0x44, 0x78);
		int CE = (reg_0x78 & 0x7F) + (reg_0x78 & 0x80)*-1;
		s.AppendFormat(_T("\tCE packet = %d\n"), CE);
		int RP = i2c_rd(0x44, 0x79);
		s.AppendFormat(_T("\tRP packet = %d\n"), RP);
		//------------------------------------------------------------


		// Received Power	
		//------------------------------------------------------------
		int power;
		for (int i = 0; i < 5; i++)
		{
			power = (i2c_rd(0x44, 0x7B) << 8) + i2c_rd(0x44, 0x7A);
			if (power < 0x7FFF)
				break;
		}
		s.AppendFormat(_T("\tReceived Power = %d mW\n"), power);
		m_dRP = power;
		//------------------------------------------------------------

		// Frequency
		//------------------------------------------------------------
		int freq_cnt = ((i2c_rd(0x44, 0x03) & 0x3F) << 8) + i2c_rd(0x44, 0x02);
		double freq;
		if (freq_cnt >= 1000)
			freq = 1000 / ((freq_cnt * 0.11) / 128);	// KHz
		else
			freq = 0;
		//s.AppendFormat( _T("\tFrequency = %.2f KHz\n"), freq);
		s.Format(_T("Freq = %.2f KHz, "), freq);
		m_dFreq = freq;
		//------------------------------------------------------------

		// Vts
		//------------------------------------------------------------
		int Vts_code = ((i2c_rd(0x44, 0x7E) & 0x0F) << 8) + i2c_rd(0x44, 0x7D);
		double Vts = (double)Vts_code * 2.0 / 1024;
		//double temperature = code2temp( Vts_code );
		double temperature = 0;
		//if (freq_cnt >= 1000)
		//	freq = 1000 / ((freq_cnt * 0.11) / 128);	// KHz
		//else
		//	freq = 0;
		m_dTS = temperature;
		//------------------------------------------------------------

		//log_add( s, RGB( 55, 0, 155) );
		s.AppendFormat(_T("CE=%02d, RP=%02X, Vrec=%.2f V, Iout=%.0f mA, Vout=%.2fV Power=%d mW, Vts=%.2fV, Temp=%.0f oC\n"),
			CE, RP, m_dVrect, m_dIout, m_dVout, power, Vts, temperature);
		//log_add( s, RGB( 55, 0, 155) );

		//CTime cur_Time = CTime::GetCurrentTime();
		//g_sLogAdd.Format( _T("[%02d:%02d:%02d] "), cur_Time.GetHour(), cur_Time.GetMinute(), cur_Time.GetSecond());
		//g_sLogAdd.Append( s );

		//if (!bDont_save_log)
		//	file.WriteString( g_sLogAdd );
		//for(int i=0; i<10; i++)
		//{
		//	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		//		DispatchMessage(&msg);
		//	MyDelay(10);
		//}
	}
}





CString CATEView::gen_html_waveform(CString sImgFile, int nImgNo, int nTotalImgCnt)
{
	CString s;
	CString sTestLev[10];

	// find test item level
	int lev;
	if (m_TestList[m_nTestIdx].lev4 != 0)
		lev = 4;
	else if (m_TestList[m_nTestIdx].lev3 != 0)
		lev = 3;
	else if (m_TestList[m_nTestIdx].lev2 != 0)
		lev = 2;
	else
		lev = 1;

	sTestLev[lev - 1] = m_TestList[m_nTestIdx].sText;
	int cur_lev = lev;
	for (int i = m_nTestIdx - 1; i >= 0; i--)
	{
		if (m_TestList[i].lev4 != 0)
			cur_lev = 4;
		else if (m_TestList[i].lev3 != 0)
			cur_lev = 3;
		else if (m_TestList[i].lev2 != 0)
			cur_lev = 2;
		else
			cur_lev = 1;
		if (cur_lev < lev)
		{
			lev = cur_lev;
			sTestLev[lev - 1] = m_TestList[i].sText;
			if (lev == 1)
				break;
		}
	}

	//sTestLev[0].Format( _T("Test with Normal Tx"));
	//sTestLev[1].Format( _T("Constant Iload"));
	//sTestLev[2].Format( _T("Iload= %.0fmA"), m_dELoadI_set);
	//sTestLev[3].Format( _T(""));

	s.Format(_T("\n<!-------------------------------------------------------------------------------->\n"));
	if (nImgNo == 1)
		s += _T("<table id=\"") + m_TestList[m_nTestIdx].sText + _T("\" class=\"waveform\"> \n");
	else
		s += _T("<table class=\"waveform\"> \n");
	//s = str_cast( s, sImgFile, );

	s.AppendFormat(
		_T("	<tr> \n")
		_T("		<td width=\"10px\"></td><td><img src=\"waveform\\"));

	s += m_TestList[m_nTestIdx].sCmd + _T("\\");
	s.Append(sImgFile);
	s.AppendFormat(_T("\" /></td> \n"));

	s.AppendFormat(_T("		<td>\n"));

	for (int i = 0; i < 10; i++)
	{
		if (sTestLev[i] == _T(""))
			break;
		s.AppendFormat(_T("		    <a href=\"report.htm #list_"));
		s.Append(sTestLev[i]);
		s.AppendFormat(_T("\">"));
		s.Append(sTestLev[i]);
		s.AppendFormat(_T("</a> <span class=\"divider\">/</span>\n"));
	}

	//_T("		    <a href="#">Normal Tx</a> <span class="divider">/</span>\n")
	//_T("		    <a href="#">Steady Iout</a> <span class="divider">/</span>\n")
	//_T("		    <a href="#">200mA</a> <span class="divider">/</span>\n")
	//s.Append( sImgFile);
	s = str_cast(s, _T("<h2>"), sImgFile);
	s.AppendFormat(
		_T("</h2>\n")
		_T("			\n")
		_T("		<table>\n")
		_T("		<tr>\n")
		_T("		<td class=\"lastcol\"> \n")
		_T("			<h1>RT7800</h1>\n")
		_T("			<h2> Waveform: %d of %d</h2>\n")
		_T("			<ul>\n")
		, nImgNo, nTotalImgCnt);
	s.AppendFormat(
		_T("			<li> %s: <span style=\"color:#f83\">%.3fV</span> ~ <span style=\"color:#f83\">%.3fV</span>, ripple= %.3fV</li>\n")
		, (LPCTSTR)m_sChName[0]
		, m_dCH[0][0], m_dCH[0][1], abs(m_dCH[0][0] - m_dCH[0][1]));
	s.AppendFormat(
		_T("			<li> %s: <span style=\"color:green\">%.3fV</span> ~ <span style=\"color:green\">%.3fV</span>, ripple= %.3fV</li>\n")
		, (LPCTSTR)m_sChName[1]
		, m_dCH[1][0], m_dCH[1][1], abs(m_dCH[1][0] - m_dCH[1][1]));
	s.AppendFormat(
		_T("			<li> %s: <span style=\"color:blue\">%.3fV</span> ~ <span style=\"color:blue\">%.3fV</span>, ripple= %.3fV</li>\n")
		, (LPCTSTR)m_sChName[2]
		, m_dCH[2][0], m_dCH[2][1], abs(m_dCH[2][0] - m_dCH[2][1]));

	s.AppendFormat(
		_T("			<li> %s: <span style=\"color:#d00\">%.3fV</span> ~ <span style=\"color:#d00\">%.3fV</span>, ripple= %.3fV</li>\n")
		, (LPCTSTR)m_sChName[3]
		, m_dCH[3][0], m_dCH[3][1], abs(m_dCH[3][0] - m_dCH[3][1]));


	s.AppendFormat(
		_T("			</ul>\n")
		_T("\n")
		_T("			<h2> View raw data </h2>\n")
		);


	CString filename[4];
	for (int i = 0; i < 4; i++)
	{

		filename[i] = m_TestList[m_nTestIdx].sCmd + _T("\\") + sImgFile;
		//filename[i] = sImgFile;
		filename[i].AppendFormat(_T("_"));
		filename[i].Append(m_sChName[i]);
		filename[i].AppendFormat(_T("_ch%d.csv"), i + 1);
	}

	for (int i = 0; i < 4; i++)
	{
		s += _T("			<a href=\"waveform\\") + filename[i];
		s.AppendFormat(_T("\" class=\"myButton_CH%d\">%s</a>\n")
			, i + 1
			, (LPCTSTR)m_sChName[i]
			);
	}

	s.AppendFormat(
		_T("		</td> \n")
		_T("		<td class=\"lastcol\"> \n")
		_T("			<h1> Electrical Load (IT8811)</h1>\n")
		_T("			<ul>\n")
		_T("				<li> CC mode</li>\n")
		);

	s.AppendFormat(
		_T("				<li> Iload setting = constant %.1fmA</li>\n"), m_dELoadI_set);


	s.AppendFormat(
		_T("				<li> Iload measurement = <span style=\"color:#d00\">%.1fmA</span></li>\n"), m_dELoadI);

	s.AppendFormat(
		_T("				<li> Vload measurement = <span style=\"color:green\">%.3fV</span></li>\n"), m_dELoadV);

	s.AppendFormat(
		_T("			</ul>\n")
		_T("			<h1> <br>SourceMeter(2400)</h1>\n")
		_T("			<ul>\n")
		);

	for (int i = 0; i < 3; i++)
	{
		s.AppendFormat(
			_T("				<li>%s = %.3fV, %.3fmA</li>\n")
			, (LPCTSTR)m_sSourceMeterName[i]
			, m_dSourceMeterV[i]
			, m_dSourceMeterI[i] * 1000.0
			);
	}

	s.AppendFormat(
		_T("			</ul>\n")
		_T("		</td>\n")
		_T("	</tr>\n")
		_T("	</table>\n")
		_T("</table>\n")
		_T("<!-------------------------------------------------------------------------------->\n\n")
		);

	return s;
}

CString CATEView::str_cast(CString s1, CString s2, CString s3)
{
	CString s;
	s = s1;
	s.Append(s2);
	s.Append(s3);
	return s;
}

CString CATEView::read_html_preset(CString sFile2)
{
	CStdioFile file;
	//CString sFile2;
	CString s2;

	CString s;

	int p1, p2;
	//sFile2.Format(_T("pre_define.pset"));
	//sFile2.Format(_T("pre_define2.pset"));
	if (file.Open(sFile2, CFile::modeRead | CFile::typeText) == 0)
	{
		MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
		s.Format(_T("ERROR"));
		return s;
	}

	char* char_buf;
	long length = file.GetLength();
	file.SeekToBegin();
	char_buf = new char[length + 1];
	memset(char_buf, '\0', length);
	file.Read(char_buf, length);
	char_buf[length] = '\0';
	s2 = char_buf;
	file.Close();
	delete char_buf;
	return s2;

}


CString CATEView::gen_html_mtp()
{

	return _T("");



	//bool is_mtp_def[256];
	//int m_nReg[256];
	//CString s;

	//CString sFile2(_T("test_setting.mtp"));
	//FILE *fptr;
	//unsigned char FileBuffer[16 * 16];
	//int length;
	//if((fptr=_wfopen(sFile2,_T("rb")))==NULL)
	//{
	//	ASSERT ( false );
	//}
	//else
	//{
	//	fseek(fptr, 0, SEEK_END);
	//	length = ftell(fptr);
	//	fseek(fptr, 0, SEEK_SET);
	//	fread(FileBuffer, length, 1, fptr);
	//	if(fclose(fptr))
	//	{
	//		ASSERT( false );
	//	}

	//}
	//_fcloseall( );
	//for (int i=0; i<length; i++)
	//	m_nReg[i] = FileBuffer[i];

	////s.Format(
	////	_T("<!DOCTYPE html>\n")
	////	_T("<html lang=\"en\">\n")
	////	_T("<head>\n")
	////	_T("  <title>RT1650 MTP Setting</title>\n")
	////	_T("  <meta charset=\"utf-8\">\n")
	////	_T("  <body>\n")
	////	);

	//s.Format(_T(""));
	//for(int i=0; i<256; i++)
	//	is_mtp_def[i] = false;
	//for(int i=0; i<2000; i++)
	//	if (mtp_def[i].RegName != _T("") && (mtp_def[i].addr <= 0xFF))
	//		is_mtp_def[mtp_def[i].addr] = true;
	//	else if (mtp_def[i].addr > 0xFF)
	//		break;



	//// gen MTP table
	//s.Format(_T("<br/><br/><br/><h1>MTP Setting</h1>\n"));
	//s.AppendFormat( 
	//	_T("<table id=\"box-table-a\" align=\"center\" width=\"500px\">\n")
	//	_T("		<colgroup> <col class=\"oce-second\" /> </colgroup>")
	//	);

	////<table  align="center" bgcolor="#aaaaa" style="color:#222">

	//s.AppendFormat( _T("\t<tr><th></th>\n"));
	//for(int i=0; i<16; i++)
	//	s.AppendFormat( _T("\t<th>%02X</th>\n"), i);
	//s.AppendFormat( _T("\t</tr>\n"));

	//for(int row=0; row<16; row++)
	//{
	//	s.AppendFormat( _T("\t<tr><td>%X0</td>\n"), row);
	//	int value;
	//	for(int i=0; i<16; i++)
	//	{
	//		value = m_nReg[row*16+i];
	//		if (!is_mtp_def[row*16+i])
	//			s.AppendFormat( _T("\t\t<td>%X</td>\n"), m_nReg[row*16+i]);
	//		else
	//			s.AppendFormat( _T("\t\t<td bgcolor=\"#ffaa33\">%X</td>\n"), m_nReg[row*16+i]);
	//	}
	//	s.AppendFormat( _T("\t</tr>\n"), row);
	//}
	//s.AppendFormat( _T("</table>\n<br><br>"));

	//return s;
}

CString CATEView::gen_html_title(CTime m_time)
{
	CString s;


	s.Format(
		_T("  <title>RT7800 VBUS_OVP Auto Test</title>\n")
		//_T("<div style=\"height:100px\"></div>\n")
		//_T("<table class=\"RICHTEK_HEAD_LINE\" style=\"background:#003666; width:100%%\">\n")
		_T("<table class=\"RICHTEK_HEAD_LINE\" style=\"background:#204686; width:100%%;height:100px\">\n")
		_T("	<tr>\n")
		_T("		<td width=\"20px\"></td>\n")
		_T("		<td style=\"color:#fff\">RICHTEK</td>\n")
		_T("		<td width=\"100px\"></td>\n")
		_T("		<td style=\"color:#fff\"> RT7800 test report</td>\n")
		_T("		<td style=\"color:#fff\" width=\"400px\" align=\"right\">")
		);

	s.AppendFormat(_T("%04d-%02d-%02d %02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), m_time.GetHour(), m_time.GetMinute());
	s.AppendFormat(
		_T("</td>\n")
		_T("	</tr>\n")
		_T("</table>\n")
		);

	//// generate Function Block & Application 
	////------------------------------------------------------------
	//s.AppendFormat(
	//	_T("<table width=\"100%%\">\n")
	//	_T("	<tr height=\"32px\" bgcolor=\"D95B43\" style=\"color:#fff\">\n")
	//	_T("		<th>Function Block</th>\n")
	//	_T("		<th>Typical Application</th>\n")
	//	_T("	</tr>\n")
	//	_T("	<tr>\n")
	//	_T("	<td height=\"700px\" class=\"test_connection\"> <img src=\"others\\function_block.png\" /></img>\n")
	//	_T("	<td class=\"test_connection\"> <img src=\"others\\typical_application.png\" /></img>\n")
	//	_T("	</tr>\n")
	//	_T("</table>\n")
	//	_T("<br/>\n")
	//	);

	return s;
}

CString CATEView::gen_html_test_list()
{
	CString s;

	s.Format(
		_T("<ul id=\"nestedlist\">\n")
		_T("    <li><a id=\"list_Test Group\" href=\"#\">Test Group</a>\n")
		_T("    <ul>\n")
		);

	int cur_lev = 1;
	int next_lev = 0;
	for (int i = 1; i < m_nTotalTestItemCnt; i++)
	{
		if (m_TestList[i].lev4 != 0)
			next_lev = 4;
		else if (m_TestList[i].lev3 != 0)
			next_lev = 3;
		else if (m_TestList[i].lev2 != 0)
			next_lev = 2;
		else
			next_lev = 1;

		if (cur_lev > next_lev)
			s += _T("		</ul></li>\n");
		cur_lev = next_lev;

		if (m_TestList[i].tm == TEST_MODE::FOLDER)
		{
			// folder
			if (cur_lev == 1)
				s.AppendFormat(_T("        <li class=\"connect\"><a id=\""));
			else
				s.AppendFormat(_T("        <br/><li><a id=\""));

			s += _T("list_") + m_TestList[i].sText + _T("\" href=\"#") + m_TestList[i].sText + _T("\"> ");
			s += m_TestList[i].sText + _T("</a>\n");
			s += _T("        <ul>\n");
		}
		else
		{
			// test item
			s.AppendFormat(
				_T("        <li> <a id=\""));
			s += _T("list_") + m_TestList[i].sText + _T("\" href=\"#") + m_TestList[i].sText + _T("\"> ");
			s += m_TestList[i].sText + _T("</a></li>\n");
		}

	}
	if (cur_lev == 1)
		s += _T("</ul>\n");
	else if (cur_lev == 2)
		s += _T("</ul>\n</ul>\n");
	else if (cur_lev == 3)
		s += _T("\t</ul>\n\t</ul>\n</ul>\n");


	return s;
}

bool CATEView::is_scope_triggered()
{
	do_query_number(":TER?");
	if (m_dResult == 0)
		return false;
	else
		return true;

}

bool CATEView::copy_my_files()
{

	if (m_bCopyFileDone)
		return false;

	CShellFileOp sfo;
	BOOL         bAPICalled;
	int          nAPIReturnVal;

	// This example copies a few files to the A: drive.

	// Pass the full paths to the files to be copied.

	//sfo.AddSourceFile ( _T("c:\\windows\\command\\format.com") );
	//sfo.AddSourceFile ( _T("c:\\windows\\command\\fdisk.exe") );
	//sfo.AddSourceFile ( _T("c:\\*.com") );
	sfo.AddSourceFile(_T("to_report_folder\\*"));

	// Pass the destination directory

	sfo.AddDestFile(m_sTestDir);

	// Set up a few flags that control the operation.

	sfo.SetOperationFlags
		(FO_COPY,         // the operation type (copy in this case)
		AfxGetMainWnd(), // pointer to parent window
		FALSE,           // flag - silent mode?
		FALSE,           // flag - allow undo?
		FALSE,           // flag - should wild cards affect files only?
		TRUE,            // flag - suppress confirmation messages?
		TRUE,            // flag - suppress confirmation messages 
		// when making directories?
		FALSE,           // flag - rename files when name collisions occur?
		FALSE);         // flag - simple progress dialog?

	// Start the operation.

	if (sfo.Go(&bAPICalled, &nAPIReturnVal))
	{
		// The operation succeeded!
	}
	else
	{
		if (!bAPICalled)
		{
			// SHFileOperation() wasn't called - check the info you passed
			// in to the CShellFileOp object.  The DEBUG version will
			// throw ASSERTs and/or show TRACE messages to help you out.
		}
		else
		{
			// SHFileOperation() returned nonzero (failure).  That return
			// value is now in nAPIReturnVal.
		}
	}

	//m_bCopyFileDone = true;
	return true;
}


void CATEView::gen_fod_chart()
{
	//float X_label[80];
	//float Vrect[80];
	//float Vout[80];
	//float Iout[80];
	//float RP[80];
	float m_dVrect[80] = {	// unit: V
		7.121569,
		7.105882,
		7.121569,
		7.105882,
		7.105882,
		7.372549,
		7.294117,
		7.262745,
		7.231372,
		7.215686,
		7.184314,
		7.152941,
		7.105882,
		7.137255,
		7.262745,
		7.247059,
		7.247059,
		7.215686,
		7.200000,
		7.184314,
		7.168627,
		7.152941,
		7.137255,
		7.105882,
		7.090196,
		7.105882,
		7.090196,
		7.090196,
		7.105882,
		7.105882,
		7.105882,
		7.105882,
		7.090196,
		7.090196,
		7.090196,
		7.090196,
		7.105882,
		7.121569,
		7.090196,
		7.105882,
		7.074510,
		7.105882,
		7.105882,
		7.074510,
		7.090196,
		7.105882,
		7.090196,
		7.105882,
		7.090196,
		5.788235,
		5.396079,
		5.349020,
		5.270588,
		5.270588,
		5.286274,
		5.270588,
		5.270588,
		5.286274,
		5.254902,
		5.286274,
		5.254902,
		5.270588,
		5.286274,
		5.270588,
		5.270588,
		5.301961,
		5.270588,
		5.286274,
		5.270588,
		5.270588,
		5.286274,
		5.270588,
		5.286274,
		5.254902,
		5.270588,
		5.270588,
		5.270588,
		5.270588,
		5.254902,
		5.270588,
	};

	float m_dVout[80] = {	// unit: mA
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.952941,
		4.952941,
		4.952941,
		4.941176,
		4.952941,
		4.952941,
		4.964706,
		4.952941,
		4.952941,
		4.964706,
		4.952941,
		4.964706,
		4.964706,
		4.952941,
		4.964706,
		4.964706,
		4.964706,
		4.976470,
		4.976470,
		4.988235,
		4.988235,
		4.988235,
		4.964706,
		4.952941,
		4.952941,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.952941,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
		4.941176,
		4.952941,
		4.941176,
	};

	float m_dIout[80] = {	// unit: mA
		0.000000,
		0.000000,
		15.686275,
		23.529411,
		39.215687,
		47.058823,
		62.745098,
		70.588234,
		86.274513,
		86.274513,
		101.960785,
		109.803925,
		125.490196,
		133.333328,
		149.019608,
		156.862747,
		164.705887,
		188.235291,
		188.235291,
		196.078430,
		203.921570,
		211.764709,
		227.450974,
		235.294113,
		243.137253,
		258.823517,
		266.666656,
		290.196075,
		290.196075,
		298.039215,
		305.882355,
		321.568634,
		329.411774,
		345.098053,
		345.098053,
		360.784302,
		368.627441,
		376.470581,
		400.000000,
		400.000000,
		407.843140,
		415.686279,
		431.372559,
		439.215698,
		447.058838,
		462.745087,
		470.588226,
		478.431366,
		494.117645,
		501.960785,
		517.647034,
		525.490173,
		533.333313,
		549.019592,
		556.862732,
		572.549011,
		580.392151,
		588.235291,
		603.921570,
		611.764709,
		619.607849,
		627.450989,
		643.137268,
		658.823547,
		658.823547,
		674.509827,
		682.352966,
		690.196106,
		705.882324,
		713.725464,
		729.411743,
		737.254883,
		745.098022,
		752.941162,
		760.784302,
		776.470581,
		784.313721,
		792.156860,
		800.000000,
		807.843140,
	};

	float m_dRP[80] = {	// unit: mW
		730.000000,
		730.000000,
		870.000000,
		870.000000,
		870.000000,
		1126.000000,
		1126.000000,
		1302.000000,
		1302.000000,
		1302.000000,
		1528.000000,
		1528.000000,
		1528.000000,
		1776.000000,
		1776.000000,
		1776.000000,
		2009.000000,
		2009.000000,
		2009.000000,
		2244.000000,
		2244.000000,
		2244.000000,
		2484.000000,
		2484.000000,
		2484.000000,
		2758.000000,
		2758.000000,
		2877.000000,
		2877.000000,
		2877.000000,
		3114.000000,
		3114.000000,
		3114.000000,
		3359.000000,
		3359.000000,
		3359.000000,
		3604.000000,
		3604.000000,
		3604.000000,
		3855.000000,
		3855.000000,
		3855.000000,
		4087.000000,
		4087.000000,
		4087.000000,
		4087.000000,
		4382.000000,
		4382.000000,
		4382.000000,
		4673.000000,
		4673.000000,
		3965.000000,
		3965.000000,
		3965.000000,
		4148.000000,
		4148.000000,
		4148.000000,
		4447.000000,
		4447.000000,
		4447.000000,
		4680.000000,
		4680.000000,
		4680.000000,
		4880.000000,
		4880.000000,
		4880.000000,
		5123.000000,
		5123.000000,
		5123.000000,
		5295.000000,
		5295.000000,
		5295.000000,
		5469.000000,
		5469.000000,
		5469.000000,
		5653.000000,
		5653.000000,
		5653.000000,
		5898.000000,
		5898.000000,
	};


	// From E-load
	float m_ELoadI[80] = {	// unit: mA
		0.000504,
		0.009476,
		0.019699,
		0.029434,
		0.039673,
		0.049393,
		0.059631,
		0.069366,
		0.079605,
		0.089325,
		0.099441,
		0.109161,
		0.119400,
		0.129135,
		0.139374,
		0.149094,
		0.159210,
		0.169449,
		0.179291,
		0.189529,
		0.199265,
		0.209503,
		0.219223,
		0.229462,
		0.239197,
		0.249435,
		0.259155,
		0.269394,
		0.279114,
		0.289352,
		0.299088,
		0.309326,
		0.319565,
		0.329285,
		0.339523,
		0.349258,
		0.359497,
		0.369217,
		0.379456,
		0.389053,
		0.399292,
		0.409027,
		0.419250,
		0.428986,
		0.439346,
		0.449585,
		0.459320,
		0.469559,
		0.479156,
		0.489395,
		0.499115,
		0.509491,
		0.519211,
		0.529327,
		0.539047,
		0.549408,
		0.559006,
		0.569382,
		0.579102,
		0.589340,
		0.599579,
		0.609314,
		0.619415,
		0.629272,
		0.639389,
		0.649231,
		0.659348,
		0.669067,
		0.679306,
		0.689041,
		0.699402,
		0.709137,
		0.719376,
		0.728973,
		0.739334,
		0.749451,
		0.759171,
		0.769531,
		0.779266,
		0.789368,
	};

	float m_ELoadV[80] = {	// unit: V
		4.946730,
		4.945820,
		4.944000,
		4.942260,
		4.940600,
		4.938780,
		4.937190,
		4.935680,
		4.933940,
		4.932430,
		4.930540,
		4.929090,
		4.927510,
		4.925920,
		4.924260,
		4.922440,
		4.920780,
		4.918880,
		4.917450,
		4.915710,
		4.914120,
		4.912310,
		4.910800,
		4.908980,
		4.907300,
		4.905640,
		4.904130,
		4.902540,
		4.901340,
		4.899440,
		4.898160,
		4.896710,
		4.895580,
		4.894070,
		4.892560,
		4.891270,
		4.890290,
		4.889160,
		4.887790,
		4.886960,
		4.885740,
		4.884920,
		4.883850,
		4.882950,
		4.882110,
		4.881210,
		4.880160,
		4.879170,
		4.878340,
		4.867370,
		4.856770,
		4.849520,
		4.844070,
		4.839680,
		4.836580,
		4.833540,
		4.830370,
		4.827120,
		4.824690,
		4.822200,
		4.820010,
		4.817580,
		4.815460,
		4.811390,
		4.809190,
		4.806990,
		4.804570,
		4.802380,
		4.799650,
		4.796780,
		4.794200,
		4.791720,
		4.788530,
		4.785200,
		4.783080,
		4.780670,
		4.777710,
		4.774690,
		4.772190,
		4.769010,
	};


	float X_label[80];
	for (int i = 0; i < 80; i++)
		X_label[i] = (float)i;

	HC_OUTLINE ol = {
		_T("FOD Accuracy"),
		_T("number(n)"),
		80,
		X_label,
		false
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T("{value}mA"), _T("Current") },
		{ _T("{value}mW"), _T("Power") },
		{ _T(""), _T("") }	// end marker
	};

	HC_X_AXIS Xaxis[] = {
		{ _T("VRECT"), 0, m_dVrect, _T("V") },
		{ _T("VRECT"), 0, m_arrVrect, _T("V") },

		{ _T("VOUT"), 0, m_dVout, _T("V") },
		{ _T("Iout"), 1, m_dIout, _T("mA") },
		{ _T("RP"), 2, m_dIout, _T("mW") },
		{ _T(""), 0, NULL, _T("") }
	};

	CString sCurve1 = gen_js_chart(_T("FOD_ACCuracy"), ol, Yaxis, Xaxis);
}

CString CATEView::gen_js_chart(CString sID, HC_OUTLINE ol, HC_Y_AXIS Yaxis[], HC_X_AXIS Xaxis[])
{

	CString s(_T(""));
	s.AppendFormat(
		//_T("<script type = \"text/javascript\">\n")
		_T("$(function () {\n")
		_T("$('#")
		);
	s.Append(sID);
	s.AppendFormat(
		_T("').highcharts({\n")
		_T("    chart: { zoomType: 'x' },\n")
		_T("    title: {\n")
		_T("        text: '")
		);
	s.Append(ol.sTitle_text);
	s.AppendFormat(
		_T("'\n")
		_T("    },\n")
		_T("    xAxis: [{\n")
		_T("        title: {\n")
		_T("            text: '")
		);
	s.Append(ol.sX_axis_text);
	s.AppendFormat(
		_T("'\n")
		_T("        },\n")
		_T("        categories: [\n")
		);
	for (int i = 0; i < ol.nX_data_length; i++)
	{
		s.AppendFormat(_T("'"));

		if (ol.bX_lable_is_int)
			s.AppendFormat(_T("0x%X"), (int)ol.pfX_label[i]);
		else
			s.AppendFormat(_T("%.3f"), ol.pfX_label[i]);
		s.AppendFormat(_T("'"));
		if (i < ol.nX_data_length - 1)
			s.AppendFormat(_T(","));
		if (i % 8 == 7 && i < ol.nX_data_length - 1)
			s.AppendFormat(_T("\n"));
	}
	s.AppendFormat(
		_T("],\n")
		_T("        crosshair: true\n")
		_T("    }],\n")
		_T("    plotOptions: {\n")
		_T("        series: {\n")
		_T("            fillColor: {\n")
		_T("                linearGradient: [0, 0, 0, 300],\n")
		_T("                stops: [\n")
		_T("                    [0, Highcharts.getOptions().colors[0]],\n")
		_T("                    [1, Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0).get('rgba')]\n")
		_T("                ]\n")
		_T("            }\n")
		_T("        }\n")
		_T("    },\n")
		);
	s.AppendFormat(
		_T("yAxis: [\n")
		);
	for (int i = 0; i < 10; i++)
	{
		if (Yaxis[i].sY_label == _T(""))
			break;
		s.AppendFormat(
			_T(" {\n")
			_T("     //gridLineWidth: 0,\n")
			_T("     labels: {\n")
			_T("         format: '")
			);
		s.Append(Yaxis[i].sY_label);
		s.AppendFormat(
			_T("',\n")
			_T("         style: {\n")
			_T("             color: Highcharts.getOptions().colors[1]\n")
			_T("         }\n")
			_T("     },\n")
			_T("     title: {\n")
			_T("         text: '")
			);
		s.Append(Yaxis[i].sY_title);
		s.AppendFormat(
			_T("	',\n")
			_T("         style: {\n")
			_T("             color: Highcharts.getOptions().colors[1]\n")
			_T("         }\n")
			_T("     },\n")
			_T("     opposite:")
			);
		if (i % 2 == 0)
			s.AppendFormat(_T("false\n"));
		if (i % 2 == 1)
			s.AppendFormat(_T("true\n"));
		s.AppendFormat(
			_T(" },\n")
			);
	}
	s.AppendFormat(
		_T("],\n")
		_T(" tooltip: {\n")
		_T("     shared: true\n")
		_T(" },\n")
		_T(" legend: {\n")
		_T("     align: 'center',\n")
		_T("     x: 0,\n")
		_T("     verticalAlign: 'top',\n")
		_T("     y: 0,\n")
		_T("     floating: false,\n")
		_T("     backgroundColor: (Highcharts.theme && Highcharts.theme.legendBackgroundColor) || '#FFFFFF'\n")
		_T(" },\n")
		_T(" series: [\n")
		);

	for (int i = 0; i < 20; i++)
	{
		if (Xaxis[i].sSeries_name == _T(""))
			break;
		s.AppendFormat(
			_T("	 {\n")
			_T("                name: '")
			);
		s.Append(Xaxis[i].sSeries_name);
		s.AppendFormat(
			_T("',\n")
			_T("                type: '")
			);
		if (i == 0)
			s.AppendFormat(_T("area"));
		else
			s.AppendFormat(_T("spline"));
		s.AppendFormat(
			_T("',\n")
			_T("                marker: {\n")
			_T("                    enabled: %s\n")
			_T("                },\n")
			_T("                yAxis: ")
			, (ol.nX_data_length > 50) ? _T("false") : _T("true")
			);
		s.AppendFormat(_T("%d"), Xaxis[i].nSeries_Y_sel);
		s.AppendFormat(
			_T(",\n")
			_T("                data: [\n")
			);
		for (int j = 0; j < ol.nX_data_length; j++)
		{
			s.AppendFormat(_T("%.2f"), Xaxis[i].pSeries_data[j]);
			if (j < ol.nX_data_length - 1)
				s.AppendFormat(_T(","));
			if (j % 8 == 7 && i < ol.nX_data_length - 1)
				s.AppendFormat(_T("\n"));
		}

		s.AppendFormat(
			_T("			], \n")
			_T("                tooltip: {\n")
			_T("                    valueSuffix: '")
			);
		s.Append(Xaxis[i].Series_suffix);
		s.AppendFormat(
			_T("'\n")
			_T("                }\n")
			_T("             },\n")
			);
	}
	s.AppendFormat(
		_T("            ]\n")
		_T("        });\n")
		_T("    });\n")
		//_T("</script>\n")
		);

	return s;
}

void CATEView::set_scope_trigger_edge(int nChannel, bool bPosEdge, double dLevel)
{
	/* Set trigger mode (EDGE, PULSe, PATTern, etc., and input source. */
	do_command(":TRIGger:MODE EDGE");
	//do_query_string(":TRIGger:MODE?");
	//printf("Trigger mode: %s\n", str_result);

	CString s;
	s.Format(_T(":TRIGger:EDGE:SOURCe CHANnel%d"), nChannel);
	do_command(s);
	//do_command(":TRIGger:EDGE:SOURCe CHANnel1");
	//do_query_string(":TRIGger:EDGE:SOURce?");
	//printf("Trigger edge source: %s\n", str_result);

	s.Format(_T(":TRIGger:EDGE:LEVel %.3f"), dLevel);
	do_command(s);
	//do_command(":TRIGger:EDGE:LEVel 1.5");
	//do_query_string(":TRIGger:EDGE:LEVel?");
	//printf("Trigger edge level: %s\n", str_result);

	if (bPosEdge)
		do_command(":TRIGger:EDGE:SLOPe POSitive");
	else
		do_command(":TRIGger:EDGE:SLOPe NEGative");

}

void CATEView::set_scope_persistence_display()
{
	do_command(":DISPlay:CLEar");
	do_command(":DISPlay:PERSistence INF");
}

void CATEView::set_scope_mode_roll()
{
	set_scope_mode(true, false);	// roll mode
}
void CATEView::set_scope_mode_auto()
{
	set_scope_mode(false, true);	// auto mode
}
void CATEView::set_scope_mode_normal()
{
	set_scope_mode(false, false);	// normal mode
}

void CATEView::set_eload_slew_rate(double dSlewRate)
{
	CString s;
	s.Format(_T("CURR:SLEW:STAT ON"));
	do_command(viLoad, s);

	// unit: A/us
	s.Format(_T("CURR:SLEW %f"), dSlewRate);
	do_command(viLoad, s);
}


void CATEView::find_visa_test()
{
	//int retCnt=0; 
	ViPUInt32 retCnt = 0;
	char buf[256] = { 0 };
	char instrDesc[500] = { 0 };
	ViSession defaultRM, vi;
	ViStatus status;
	ViFindList        find_list;
	viOpenDefaultRM(&defaultRM);
	err = viOpenDefaultRM(&defaultRM);
	ViUInt32	numInstrs;

	//status = viFindRsrc(defaultRM, "USB0::?*INSTR",
	status = viFindRsrc(defaultRM, "?*INSTR",
		&find_list, &numInstrs, instrDesc);

	viFindNext(find_list, instrDesc);

	status = viOpen(defaultRM, instrDesc,
		VI_NULL, VI_NULL, &vi);
	if (status != VI_SUCCESS){
		printf("Can not find USBTMC device!\n");
		viClose(vi);
		viClose(defaultRM);
		return;
	}
	printf("USBTMC device : %s\n", instrDesc);
	viPrintf(vi, "*idn?\n");
	viScanf(vi, "%t", &buf);
	printf("%s\n", buf);
	viClose(vi);
	viClose(defaultRM);
}
void CATEView::OnBnClickedButtonFind()
{
	find_visa_test();

#define VI_FIND_BUFLEN 800
	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	viOpenDefaultRM(&defaultRM);
	viFindRsrc(defaultRM, "?*INSTR", &list, &nmatches, matches);
	int i = 3;

}


CString CATEView::gen_zoom_png_html(CString sPath, CString sID, CString sTitle, int nBackgroundColor)
{
	CTempBuffer<TCHAR, MAX_PATH> strBuffer(MAX_PATH);
	strBuffer[0] = _T('\0');
	DWORD length = GetCurrentDirectory(MAX_PATH, strBuffer);

	if (length > MAX_PATH)
	{
		// We need to reallocate the string => MAX_PATH wasn't space enough.
		strBuffer.Reallocate(length);
		strBuffer[0] = _T('\0');
		GetCurrentDirectory(length, strBuffer);
	}

	//m_strCurrDir = strBuffer;
	m_strCurrDir = sPath;
	UpdateData(FALSE);

	//CString strTable;
	//strTable = _T("<TABLE border=1 cols=4 width=100%>\n")
	//	_T("<THEAD><TR><TD width=20>&nbsp;</TD><TD>File Name</TD><TD>Size</TD><TD>Modified</TD></TR></THEAD>");


	// recreate the filelist table
	//CComPtr<IHTMLElement> spfileList;
	//GetElementInterface(_T("fileList"), IID_IHTMLElement, (void **)&spfileList);

	CString strFilter = m_strCurrDir + _T("\\*.png");
	WIN32_FIND_DATA finddata;
	HANDLE hFind = FindFirstFile(strFilter, &finddata);
	if (hFind == INVALID_HANDLE_VALUE)
		return _T("");
	CWaitCursor cur;
	CString str, strDate;


	int nFileCnt = 0;
	CString sFileName[30];
	do
	{
		//str.Format(_T("<TR><TD width=20><IMG src=%s.gif></TD><TD><SPAN class=%s>%s</SPAN></TD><TD>%lu</TD><TD>%s</TD></TR>"),
		//	((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _T("folder") : _T("file")),
		//	((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _T("folder") : _T("file")),
		//	finddata.cFileName, finddata.nFileSizeLow, (LPCTSTR)strDate);
		//strTable += str;
		sFileName[nFileCnt++] = finddata.cFileName;
	} while (FindNextFile(hFind, &finddata));
	FindClose(hFind);

	CString shtml;
	shtml.Format(
		_T("<!-- show zoom picture -->\n")
		_T("<section class=\"mbr-gallery\"\n")
		_T("         id=\"%s\" style=\"background:#%06X;\">\n")
		_T("    <!-- Gallery -->\n")
		_T("    <h1 style=\"color:#ffffff;padding:20px;padding-bottom:0px;margin-bottom:0px\"> %s </h1>\n")
		_T("    <div class=\" container mbr-gallery-layout-default\">\n")
		_T("        <div>\n")
		_T("            <div class=\"row mbr-gallery-row\">\n")
		, (LPCTSTR)sID
		, nBackgroundColor
		, (LPCTSTR)sTitle
		);

	int pos1 = sPath.GetLength() - sPath.Find(_T('\\')) - 1;
	for (int i = 0; i < nFileCnt; i++)
	{
		sFileName[i] = sPath.Right(pos1) + _T("\\") + sFileName[i];
		shtml.AppendFormat(
			_T("                <div class=\"col-lg-3 col-md-4 col-sm-6 col-xs-12 mbr-gallery-item\">\n")
			_T("                    <a href=\"#lb-%s\" data-toggle=\"modal\" data-slide-to=\"%d\"> <img alt=\"\" src=\"%s\"> <span class=\"icon glyphicon glyphicon-zoom-in\"></span> </a>\n")
			_T("                </div>\n")
			, (LPCTSTR)sID
			, i, (LPCTSTR)sFileName[i]
			);
	}
	shtml.AppendFormat(
		_T("                <div class=\"clearfix\"></div>\n")
		_T("            </div><!-- Lightbox -->\n")
		_T("            <div tabindex=\"-1\" class=\"mbr-slider modal fade carousel slide\" id=\"lb-%s\"\n")
		_T("                 data-interval=\"false\" data-keyboard=\"true\" data-app-prevent-settings=\"\">\n")
		_T("                <div class=\"modal-dialog\">\n")
		_T("                    <div class=\"modal-content\">\n")
		_T("                        <div class=\"modal-body\">\n")
		_T("                            <ol class=\"carousel-indicators\">\n")
		, (LPCTSTR)sID
		);

	for (int i = 0; i < nFileCnt; i++)
	{
		shtml.AppendFormat(
			_T("                                <li data-slide-to=\"%d\" data-app-prevent-settings=\"\" data-target=\"#lb-%s\"></li>\n")
			, i, (LPCTSTR)sID);
	}
	shtml.AppendFormat(
		_T("                            </ol>\n")
		_T("                            <div class=\"carousel-inner\">\n")
		);

	for (int i = 0; i < nFileCnt; i++)
	{
		shtml.AppendFormat(
			_T("                                <div class=\"%s\"><img alt=\"\" src=\"%s\"></div>\n")
			, (i == nFileCnt - 1) ? _T("item active") : _T("item ")
			, sFileName[i]
			);
	}
	shtml.AppendFormat(
		_T("                            </div>\n")
		_T("                            <a class=\"left carousel-control\" role=\"button\" href=\"#lb-%s\" data-slide=\"prev\">\n")
		_T("                                <span class=\"glyphicon glyphicon-menu-left\" aria-hidden=\"true\"></span>                         <span class=\"sr-only\">Previous</span>\n")
		_T("                            </a>\n")
		_T("                            <a class=\"right carousel-control\" role=\"button\" href=\"#lb-%s\" data-slide=\"next\">\n")
		_T("                                <span class=\"glyphicon glyphicon-menu-right\" aria-hidden=\"true\"></span>\n")
		_T("                                <span class=\"sr-only\">Next</span>\n")
		_T("                            </a>\n")
		_T("                            <a class=\"close\" role=\"button\" href=\"#\" data-dismiss=\"modal\">\n")
		_T("                                <span class=\"glyphicon glyphicon-remove\" aria-hidden=\"true\"></span>\n")
		_T("                                <span class=\"sr-only\">Close</span>\n")
		_T("                            </a>\n")
		_T("                        </div>\n")
		_T("                    </div>\n")
		_T("                </div>\n")
		_T("            </div>\n")
		_T("</section>\n")
		_T("<!-- show zoom picture -->\n")
		, (LPCTSTR)sID
		, (LPCTSTR)sID
		);

	return shtml;
}

void CATEView::OnSize(UINT nType, int cx, int cy)
{
	CDHtmlViewSpec::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

CString CATEView::change_evb_connection_zoom(CString sOption)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;
	hResult = GetElement(L"id_evb_conn", &pElement2);
	save_current_html(_T("run_time_temp.html"));


	//m_sTestDir.Format(_T("RT7800_test\\"));
	m_sTestDir.Format(_T("RT7800_test_report\\"));

	CString sPath;
	CString sNewZoomPNG(_T(""));


	//for (int i = 0; i < m_nTotalTestItemCnt; i++)
	//{
	//	if (m_TestList[i].sCmd == sOption)
	//	{
	//		m_TM = TEST_MODE::PROTECT_VBUS_OVP;
	//		m_nTestIdx = i;
	//		//sPath = m_sTestDir + _T("evb\\vbus_ovp");
	//		sPath = m_sTestDir + m_TestList[i].sFolderPath;
	//		sNewZoomPNG = gen_zoom_png_html(sPath, _T("id_evb_conn"), _T("VBUS OVP Connection"), 0x808080);

	//	}
	//}

	for (int i = 0; i < m_nTotalTestItemCnt; i++)
	{
		if (sOption.MakeUpper() == m_TestList[i].sCmd.MakeUpper())
		{
			m_TM = m_TestList[i].tm;
			//sPath = m_sTestDir + m_TestList[i].sFolderPath;
			sPath = _T("to_report_folder\\") + m_TestList[i].sFolderPath;
			CString sTitle = m_TestList[i].sCmd + _T(" Connection");
			sNewZoomPNG = gen_zoom_png_html(sPath, _T("id_evb_conn"), sTitle, 0x808080);

			if (sNewZoomPNG.GetLength() < 100)
			{
				MessageBox(_T("ERROR"), _T("Can't find folder path !"), MB_ICONERROR);
			}
			else
			{
				bstr = sNewZoomPNG;
				pElement2->put_outerHTML(bstr);
			}

			break;
		}

	}

	int debug1 = 1;
	return sNewZoomPNG;

	//if (sOption == _T("VBUS OVP"))
	//{
	//	m_TM = TEST_MODE::PROTECT_VBUS_OVP;
	//	sPath = m_sTestDir + _T("evb\\vbus_ovp");
	//	sNewZoomPNG = gen_zoom_png_html(sPath, _T("id_evb_conn"), _T("VBUS OVP Connection"), 0x808080);
	//}
	//else if (sOption == _T("VBUS UVP"))
	//{
	//	sPath = m_sTestDir + _T("evb\\adc_cc1");
	//	sNewZoomPNG = gen_zoom_png_html(sPath, _T("id_evb_conn"), _T("ADC_CC1 Connection"), 0x8080f0);
	//}


	//bstr = sNewZoomPNG;
	//pElement2->put_outerHTML(bstr);
}

bool CATEView::save_text_file(CString sFileName, CString sContent)
{
	CStdioFile file;
	if (file.Open(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		CString s(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return false;
	}
	file.WriteString(sContent);
	file.Close();
	return true;
}

void CATEView::save_current_html(CString sFileName)
{
	CString strHtmlText;
	BOOL bState = FALSE;
	// get IDispatch interface of the active document object
	IDispatch *pDisp = this->GetHtmlDocument();

	if (pDisp != NULL)
	{   // get the IHTMLDocument3 interface
		IHTMLDocument3 *pDoc = NULL;
		HRESULT hr = pDisp->QueryInterface(IID_IHTMLDocument3, (void**)&pDoc);
		if (SUCCEEDED(hr))
		{   // get root element
			IHTMLElement *pRootElement = NULL;
			hr = pDoc->get_documentElement(&pRootElement);
			if (SUCCEEDED(hr))
			{   // get html text into bstr
				BSTR bstrHtmlText;
				hr = pRootElement->get_outerHTML(&bstrHtmlText);
				if (SUCCEEDED(hr))
				{   // convert bstr to CString
					strHtmlText = bstrHtmlText;
					bState = TRUE;
					SysFreeString(bstrHtmlText);
				}
				pRootElement->Release();
			}
			pDoc->Release();
		}
		pDisp->Release();
	}

	CStdioFile file;
	if (file.Open(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		CString s(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return;
	}
	file.WriteString(strHtmlText);
	file.Close();
}

void CATEView::OnButtonAdcTest1()
{
	// TODO: Add your command handler code here
}


void CATEView::OnButtonTest3()
{
	// TODO: Add your command handler code here
}

bool CATEView::save_html_file(CString sText, CString sFileName)
{
	CString sBottom = read_html_preset(_T("bottom_js.pset"));
	CString sTop = sText + sBottom;
	//sTop.AppendFormat(_T("</body>\n</html>"));

	CStdioFile file;
	CString sFile2 = m_sTestDir;
	//CString sFile2 = _T("");
	sFile2.AppendFormat(sFileName);
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		MessageBox(_T("Can't Open file ! "), _T("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	file.WriteString(sTop);
	file.Close();
	return true;
}


void CATEView::MyDelay(int nMs)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg);
	if (nMs < 5)
		Sleep(nMs);
	else
	{
		int temp = nMs;
		while (temp > 0)
		{
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				DispatchMessage(&msg);
			if (m_bStop)
				break;
			Sleep(4);
			temp -= 5;
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				DispatchMessage(&msg);
		}
	}
}

void CATEView::update_test_progress(int nProgress)
{
	CString sBusy(
		_T("<div class=\"windows8\" id = \"id_run_test_label\" style = \"height:80px;\">\n")
		_T("	<div class=\"wBall\" id=\"wBall_1\">\n")
		_T("		<div class=\"wInnerBall\"></div>\n")
		_T("	</div>\n")
		_T("	<div class=\"wBall\" id=\"wBall_2\">\n")
		_T("		<div class=\"wInnerBall\"></div>\n")
		_T("	</div>	<div class=\"wBall\" id=\"wBall_3\">\n")
		_T("		<div class=\"wInnerBall\"></div>\n")
		_T("	</div>\n")
		_T("	<div class=\"wBall\" id=\"wBall_4\">\n")
		_T("		<div class=\"wInnerBall\"></div>\n")
		_T("	</div>\n")
		_T("	<div class=\"wBall\" id=\"wBall_5\">\n")
		_T("		<div class=\"wInnerBall\"></div>\n")
		_T("	</div>\n")
		_T("</div>\n")
		);

	CString sIdle(
		_T("<div id = \"id_run_test_label\" style = \"height:80px;text-align:center;width:100%%\"></div>\n")
		);

	if (nProgress <= 1)
		update_html_div(_T("id_run_test_label"), sBusy);
	if (nProgress >= 100)
		update_html_div(_T("id_run_test_label"), sIdle);


	MSG msg;
	for (int i = 0; i < 5; i++)
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		DispatchMessage(&msg);
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult = GetElement(L"id_test_progress", &pElement2);
	//bstr = _T("instrument_status_linked");
	//pElement2->put_className(bstr);

	s.Format(
		_T("<div class=\"progress\" id=\"id_test_progress\" style=\"margin:20px\">\n")
		_T("  <div class=\"progress-bar progress-bar-%s progress-bar-striped\" role=\"progressbar\" aria-valuenow=\"%d\" aria-valuemin=\"0\" aria-valuemax=\"100\" style=\"width: %d%%\">\n")
		_T("    <span class=\"sr-only\">%d%% Complete</span>%d%%\n")
		_T("  </div>\n")
		_T("</div>\n")
		, (nProgress >= 100) ? _T("success") : _T("danger")
		, nProgress
		, nProgress
		, nProgress
		, nProgress
		);

	bstr = s;
	pElement2->put_outerHTML(bstr);

}

HRESULT CATEView::OnBitValueChange(IHTMLElement *phtmlElement)
{
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	phtmlElement->get_innerText(&bstr);

	phtmlElement->get_className(&bstr);
	CString sClass = bstr;


	phtmlElement->get_id(&bstr);
	s = bstr;


	int addr = 0;
	addr = _tcstoul(s.Mid(2, 8), 0, 16);
	int value = _tcstoul(s.Right(2), 0, 16);
	int p1 = s.Find(_T("]"));
	CString sbit = s.Mid(11, p1 - 11);
	p1 = sbit.Find(_T(":"));
	int msb, lsb, bitwidth;
	int bitmask[] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF };
	if (p1 == -1)
	{
		msb = _tcstoul(sbit, 0, 10);
		lsb = msb;
		bitwidth = 0;
	}
	else
	{
		msb = _tcstoul(sbit.Mid(0, p1), 0, 10);
		lsb = _tcstoul(sbit.Mid(p1 + 1), 0, 10);
		bitwidth = msb - lsb;
	}

	// toggle button

	IHTMLElement *pElement2;
	HRESULT hResult;
	bool bValueUpdateDone = false;
	for (int i = 0; i < 3000; i++)
	{
		if (g_reg_gui[i].addr == 0)
			break;
		if (addr == g_reg_gui[i].addr)
		{
			if (!bValueUpdateDone)
			{
				if (s.Find(_T("toggle")) != -1)
				{
					int ori_value = g_reg[i].value & (bitmask[bitwidth] << lsb);
					value = ori_value ? 0x00 : 0x01;
				}

				g_reg[i].value &= ~(bitmask[bitwidth] << lsb);
				g_reg[i].value |= value << lsb;
				bValueUpdateDone = true;
			}

			// update class 
			if ((g_reg_gui[i].sBitMap == sbit) && g_reg_gui[i].sGUI_type == _T("SEL"))
			{
				hResult = GetElement(g_reg_gui[i].sID, &pElement2);
				bstr = (value == g_reg_gui[i].value) ? _T("bit_value_sel") : _T("bit_value_not_sel");
				pElement2->put_className(bstr);
			}
			else if ((g_reg_gui[i].sBitMap == sbit) && g_reg_gui[i].sGUI_type == _T("BTN"))
			{
				hResult = GetElement(g_reg_gui[i].sID, &pElement2);
				if (s.Find(_T(":")) == -1)
				{
					if (value)
						pElement2->put_className(_T("btn btn-primary"));
					else
						pElement2->put_className(_T("btn btn-default"));
				}
			}

		}

	}
	return S_OK;
}

HRESULT CATEView::On_html_btn(IHTMLElement *phtmlElement)
{
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString sID;
	phtmlElement->get_innerText(&bstr);

	phtmlElement->get_className(&bstr);
	CString sClass = bstr;

	phtmlElement->get_id(&bstr);
	sID = bstr;
	int addr = _tcstoul(sID.Mid(2, 8), 0, 16);
	if (sID.Find(_T("_rd_reg")) != -1)
	{
		int reg_data = rt7800_rd4(addr);
		for (int i = 0; i < 1000; i++)
		{
			if (g_reg_data[i].addr = addr)
			{
				g_reg_data[i].value = reg_data;
				upate_html_reg_gui(addr, reg_data);
			}
		}
	}

	return S_OK;

}

void CATEView::upate_html_reg_gui(int addr, int value)
{
	CString s;
	IHTMLElement *pElement2;
	HRESULT hResult;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	int msb, lsb, bitwidth;
	int bitmask[] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF };
	int p1;
	CString sbit;

	for (int i = 0; i < 3000; i++)
	{
		if (g_reg_gui[i].addr == 0 || g_reg_gui[i].addr > addr)
			break;

		if (addr == g_reg_gui[i].addr)
		{
			sbit = g_reg_gui[i].sBitMap;
			p1 = sbit.Find(_T(":"));
			if (p1 == -1)
			{
				msb = _tcstoul(sbit, 0, 10);
				lsb = msb;
				bitwidth = 1;
			}
			else
			{
				msb = _tcstoul(sbit.Mid(0, p1), 0, 10);
				lsb = _tcstoul(sbit.Mid(p1 + 1), 0, 10);
				bitwidth = msb - lsb + 1;
			}

			int ori_value = g_reg[i].value;
			int new_bit_value = (value & (bitmask[bitwidth] << lsb)) >> lsb;
			if (new_bit_value == ori_value)
				continue;
			else
				g_reg[i].value = new_bit_value;

			// update class 
			if (g_reg_gui[i].sGUI_type == _T("SEL"))
			{
				hResult = GetElement(g_reg_gui[i].sID, &pElement2);
				bstr = (value == g_reg_gui[i].value) ? _T("bit_value_sel") : _T("bit_value_not_sel");
				pElement2->put_className(bstr);
			}
			else if ((g_reg_gui[i].sBitMap == sbit) && g_reg_gui[i].sGUI_type == _T("BTN"))
			{
				hResult = GetElement(g_reg_gui[i].sID, &pElement2);
				if (s.Find(_T(":")) == -1)
				{
					if (value)
						pElement2->put_className(_T("btn btn-primary"));
					else
						pElement2->put_className(_T("btn btn-default"));
				}
			}

		}

	}

}

int CATEView::get_reg_data32(int addr)
{
	int data = 0;
	for (int i = 0; i < 1000; i++)
	{
		if (g_reg_data[i].addr == 0)
			break;
		if (g_reg_data[i].addr == addr)
		{
			data = g_reg_data[i].value;
			break;
		}
	}
	return data;
}

int CATEView::get_reg_bit_data(int addr, CString sBit)
{
	int value = get_reg_data32(addr);

	CString sbit = sBit;
	int p1 = sbit.Find(_T(":"));
	int msb, lsb, bitwidth;
	int bitmask[] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF };
	if (p1 == -1)
	{
		msb = _tcstoul(sbit, 0, 10);
		lsb = msb;
		bitwidth = 0;
	}
	else
	{
		msb = _tcstoul(sbit.Mid(0, p1), 0, 10);
		lsb = _tcstoul(sbit.Mid(p1 + 1), 0, 10);
		bitwidth = msb - lsb;
	}

	int bit_value = (value & (bitmask[bitwidth] << lsb)) >> lsb;

	return bit_value;

}

void CATEView::update_all_gui_reg(CString sSearch)
{
	CString s(_T(""));
	//int addr = 0x40004024;
	//int value = 0x89;
	//int addr;
	s = _T("<div id=\"id_reg_setting\">\n");
	s += _T("<div class=\"row\" style=\"padding:10px;margin:8px;\">\n");

	int nGroupBorder = 0;
	int last_addr = 0;
	CString sBitName;
	CString sBit;
	CString sDesp[50];
	int nValueMap[50];
	CString sValueSelID;
	int id_sel_idx = 0;
	int display_reg_cnt = 0;
	for (int i = 0; i < 1000; i++)
	{
		if (g_reg[i].addr == 0)
			break;
		
		// check search text
		CString sBitName = g_reg[i].sBitName;
		CString sDesp2 = g_reg[i].sDescription;
		sSearch.MakeUpper();
		sBitName.MakeUpper();
		sDesp2.MakeUpper();
		if ((sBitName.Find(sSearch) == -1) && (sDesp2.Find(sSearch) == -1))
			continue;


		if (last_addr != g_reg[i].addr)
		//if (display_reg_cnt > 10)
		{
			display_reg_cnt = 0;
			if (last_addr != 0)
			{
				s += _T("</div></div></div>\n");
			}
			last_addr = g_reg[i].addr;
			s += _T("  <div class=\"col-sm-6 col-md-4\">\n");
			s.AppendFormat(_T("<div class=\"bs-callout bs-callout-%s\">\n")
				, (nGroupBorder == 0) ? _T("info") : _T("warning")
				);
			s += _T("    <div class=\"thumbnail\">\n");
			s.AppendFormat(_T("<h1>0x%04X_%04X <a href=\"#none\" id=\"0x%08X_rd_reg\" class=\"btn\"> Read</a></h1>\n"), (g_reg[i].addr >> 16), g_reg[i].addr & 0xFFFF, g_reg[i].addr);

		}
		sBitName = g_reg[i].sBitName;
		sBit = g_reg[i].sBitMap;

		int nLineCnt = FindStringLine(g_reg[i].sDescription, sDesp, nValueMap);

		bool bOneBit = (sBit.Find(_T(":")) == -1);
		g_reg[i].value = get_reg_bit_data(g_reg[i].addr, g_reg[i].sBitMap);

		if ( bOneBit)
		{
			s.AppendFormat(_T("\t\t<p><a href=\"#test1\" id=\"0x%08X[%s]_toggle\" class=\"%s\">%s</a> %s</p>\n")
				, g_reg[i].addr
				, g_reg[i].sBitMap
				, g_reg[i].value ? _T("btn btn-primary") : _T("btn btn-default")
				, g_reg[i].sBitName
				, sDesp[0]
				);

			g_reg_gui[id_sel_idx].addr = g_reg[i].addr;
			g_reg_gui[id_sel_idx].sBitMap = g_reg[i].sBitMap;
			g_reg_gui[id_sel_idx].sID.Format(_T("0x%08X[%s]_toggle"), g_reg[i].addr, (LPCTSTR)g_reg[i].sBitMap);
			g_reg_gui[id_sel_idx].value = -1;
			g_reg_gui[id_sel_idx].sGUI_type = _T("BTN");
			id_sel_idx++;

			for (int j = 1; j < nLineCnt; j++)
			{
				sValueSelID.Format(_T("0x%08X[%s]_set_to_0x%02X")
					, g_reg[i].addr
					, g_reg[i].sBitMap
					, j - 1
					);
				g_reg_gui[id_sel_idx].addr = g_reg[i].addr;
				g_reg_gui[id_sel_idx].sBitMap = g_reg[i].sBitMap;
				g_reg_gui[id_sel_idx].sID = sValueSelID;
				g_reg_gui[id_sel_idx].value = j - 1;
				g_reg_gui[id_sel_idx].sGUI_type = _T("SEL");
				id_sel_idx++;

				s.AppendFormat(_T("<p><a id=\"%s\" class=\"%s\" href=\"#%s\">%s</a></p>\n")
					, (LPCTSTR)sValueSelID
					, ((j - 1) == g_reg[i].value) ? _T("bit_value_sel") : _T("bit_value_not_sel")
					, (LPCTSTR)sDesp[j]
					, (LPCTSTR)sDesp[j]
					);
			}
		}
		else // multi-bit register
		{
			s.AppendFormat(
				_T("    <div class=\"input-group\">\n")
				_T(" 	  <span class=\"input-group-addon\" id=\"basic-addon3\" style=\"width:140px\">%s</span>\n")
				_T("      <input id=\"0x%08X[%s]_reg_edit\" value=\"0x%X\" type=\"text\" class=\"form-control\" placeholder=\"0x00\">\n")
				_T("      <span class=\"input-group-btn\">\n")

				//_T("      <button id=\"0x%08X[%s]_reg_wr\" class=\"btn btn-default \" type=\"button\">write</button>\n")
				_T("      <button id=\"0x%08X[%s]_reg_wr\" class=\"btn btn-info \" type=\"button\">write</button>\n")

				_T("      </span>\n")
				_T("	  <span class=\"input-group-addon\" id=\"basic-addon3\">_%04X[%s]</span>\n")
				_T("    </div>\n")
				//_T("	<p> %s</p>\n")
				, (LPCTSTR) g_reg[i].sBitName
				, g_reg[i].addr
				, (LPCTSTR) g_reg[i].sBitMap
				, g_reg[i].value
				, g_reg[i].addr
				, (LPCTSTR) g_reg[i].sBitMap
				//, g_reg[i].addr >> 16
				, g_reg[i].addr & 0xFFFF
				, (LPCTSTR) g_reg[i].sBitMap
				//, (LPCTSTR) g_reg[i].sDescription
				);
		}
		display_reg_cnt++;
		nGroupBorder++;
		nGroupBorder %= 2;
	}

	g_reg_gui[id_sel_idx].addr = 0;

	s += _T("</div></div>\n");

	update_html_div(_T("id_reg_setting"), s);


}

HRESULT CATEView::On_cancel_dac_test(IHTMLElement *pElement)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;
	CString sSearch = GetItemValue(L"id_gui_reg_search_text");

	m_bStop = TRUE;
	return S_OK;
}

HRESULT CATEView::On_start_dac_test(IHTMLElement *pElement)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;
	int sample_period;
	int sample_number;
	int sweep_number;
	CString sFileName;
	int low_temp;
	int high_temp;
	int temp_step;
	BOOL bTemp_enable;

	s = GetItemValue(L"id_dac_sample_period");
	sample_period = _tcstoul(s, 0, 10);

	s = GetItemValue(L"id_dac_sample_number");
	sample_number = _tcstoul(s, 0, 10);

	s = GetItemValue(L"id_dac_sweep_number");
	sweep_number = _tcstoul(s, 0, 10);

	sFileName = GetItemValue(L"id_dac_output_file");

	s = GetItemValue(L"id_dac_low_temperature");
	low_temp = _tcstoul(s, 0, 10);

	s = GetItemValue(L"id_dac_high_temperature");
	high_temp = _tcstoul(s, 0, 10);

	s = GetItemValue(L"id_dac_temperature_step");
	temp_step = _tcstoul(s, 0, 10);

	s = GetItemValue(L"id_dac_temp_enable");
	if (s == _T("0"))
		bTemp_enable = FALSE;
	else
		bTemp_enable = TRUE;



	//--------------------------------------------------------------------------------
	// Digital Meter initial
	//--------------------------------------------------------------------------------
#define METER_VISA_ADDRESS "USB0::0x0957::0xB318::MY55250092::0::INSTR"
	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) error_handler();
	err = viOpen(defaultRM, METER_VISA_ADDRESS, VI_NULL, VI_NULL, &vi);
	if (err != VI_SUCCESS) error_handler();
	err = viSetAttribute(vi, VI_ATTR_TMO_VALUE, 2500);
	if (err != VI_SUCCESS) error_handler();
	CString sCmd;
	sCmd = _T("MEAS:VOLT:DC?");
	do_query_string("*IDN?");
	CString sID = sResult;
	do_query_string(vi, sCmd);
	double result = m_dResult;
	m_dResult = 0;
	//--------------------------------------------------------------------------------
	rt7800_wr(0x40004070, 0xff, 0x00 );
	MyDelay(1);
	do_query_string(vi, sCmd);



	m_bStop = FALSE;
	int dac_code;
	update_test_progress(0);
	int progress = 0;
	s = _T("");
	double cv_dac = 0;
	if (!bTemp_enable)
	{
		low_temp = 25;
		high_temp = 25;
		temp_step = 10;
	}

	int total_temp_cnt = (high_temp - low_temp) / temp_step + 1;

	int now_progress = 0;
	CString sTop(_T(""));

	CStdioFile file;
	if (file.Open(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		CString s(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return S_OK;
	}
	for (int temp=low_temp; temp<=high_temp; temp+=temp_step)
	{

		if (bTemp_enable)
		{
			set_chamber(temp);
			//MyDelay(60000);
			MyDelay(5*60000);
		}

		int temp_idx = (temp - low_temp) / temp_step;
		for (int sweep = 0; sweep < sweep_number; sweep++)
		{
			for (int i = 0; i < 512*2; i++)
			{

				now_progress = 100 * (i 
								+ 1024 * sweep 
								+ 1024 * sweep_number*temp_idx);
				now_progress /= 1024 * sweep_number*total_temp_cnt;
				now_progress += 1;
				if (progress != now_progress)
				{
					progress = now_progress;
					update_test_progress(progress);
				}

				dac_code = (i < 512) ? i : (511 - (i-512));
				rt7800_wr(0x40004070, (dac_code & 0xFF), (dac_code >> 8) );

				for (int j = 0; j < sample_number; j++)
				{
					if (m_bStop)
						break;
					MyDelay(sample_period);
					do_query_string(vi, sCmd);
					cv_dac = m_dResult;
				}
				s.Format(_T("Temp=,%d,oC,DAC_CODE=,0x%03X,CV_DAV=,%.5f,V\n")
					, temp, dac_code, cv_dac);

				file.WriteString(s);
				if (m_bStop)
					break;
			}
			sTop += s;
			s = _T("");
			if (m_bStop)
				break;

			//save_text_file(sFileName, sTop);
		}
	}
	
	file.Close();

	if (bTemp_enable)
	{
		set_chamber(25);
		MyDelay(60000);
	}
	//save_text_file(sFileName, sTop);
	update_test_progress(100);


	return S_OK;
}


HRESULT CATEView::On_gui_reg_search_btn(IHTMLElement *pElement)
{
	IHTMLElement *pElement2;
	VARIANT ptrVal;
	VARIANT_BOOL pVar;
	CComBSTR bstr;
	CString s;
	HRESULT hResult;
	CString sSearch = GetItemValue(L"id_gui_reg_search_text");

	return S_OK;
}
