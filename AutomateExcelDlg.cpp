
// AutomateExcelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutomateExcel.h"
#include "AutomateExcelDlg.h"

#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CChart.h"
#include "CCharts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutomateExcelDlg dialog




CAutomateExcelDlg::CAutomateExcelDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutomateExcelDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nMeasCnt = 0;
}

void CAutomateExcelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutomateExcelDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDRUN, &CAutomateExcelDlg::OnBnClickedRun)
	ON_BN_CLICKED(IDC_ATE, &CAutomateExcelDlg::OnBnClickedAte)
	ON_BN_CLICKED(IDC_MULTI_AXES, &CAutomateExcelDlg::OnBnClickedMultiAxes)

END_MESSAGE_MAP()


// CAutomateExcelDlg message handlers

BOOL CAutomateExcelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutomateExcelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutomateExcelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutomateExcelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void FillSafeArray(OLECHAR FAR* sz, int iRow, int iCol,
	COleSafeArray* sa)
{

	VARIANT v;
	long index[2];

	index[0] = iRow;
	index[1] = iCol;

	VariantInit(&v);
	v.vt = VT_BSTR;
	v.bstrVal = SysAllocString(sz);
	sa->PutElement(index, v.bstrVal);
	SysFreeString(v.bstrVal);
	VariantClear(&v);

}

void CAutomateExcelDlg::OnBnClickedRun()
{
	// Commonly used OLE variants.
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	CApplication app;

	// Start Excel and get an Application object.
	if (!app.CreateDispatch(TEXT("Excel.Application")))
	{
		AfxMessageBox(TEXT("Couldn't start Excel and get Application object."));
		return;
	}

	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;
	CRange range;
	CFont0 font;

	books = app.get_Workbooks();
	book = books.Add(covOptional);


	//Get the first sheet.
	sheets = book.get_Sheets();
	sheet = sheets.get_Item(COleVariant((short)1));
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("A1")));
	range.put_Value2(COleVariant(TEXT("Monthly Average Temperature")));
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("D1")));
	range.Merge(covOptional);
	range = sheet.get_Range(COleVariant(TEXT("B2")), COleVariant(TEXT("B2")));
	range.put_Value2(COleVariant(TEXT("Tokyo")));
	range = sheet.get_Range(COleVariant(TEXT("C2")), COleVariant(TEXT("C2")));
	range.put_Value2(COleVariant(TEXT("New York")));
	range = sheet.get_Range(COleVariant(TEXT("D2")), COleVariant(TEXT("D2")));
	range.put_Value2(COleVariant(TEXT("Berlin")));
	range = sheet.get_Range(COleVariant(TEXT("E2")), COleVariant(TEXT("E2")));
	range.put_Value2(COleVariant(TEXT("London")));


	//Fill A3:A6 with an array of values (Months).
	COleSafeArray saRet;
	DWORD numElements[] = { 12, 1 };   //4x1 element array
	saRet.Create(VT_BSTR, 2, numElements);

	FillSafeArray(L"Jan", 0, 0, &saRet);
	FillSafeArray(L"Feb", 1, 0, &saRet);
	FillSafeArray(L"Mar", 2, 0, &saRet);
	FillSafeArray(L"Apr", 3, 0, &saRet);
	FillSafeArray(L"May", 4, 0, &saRet);
	FillSafeArray(L"Jun", 5, 0, &saRet);
	FillSafeArray(L"Jul", 6, 0, &saRet);
	FillSafeArray(L"Aug", 7, 0, &saRet);
	FillSafeArray(L"Sep", 8, 0, &saRet);
	FillSafeArray(L"Oct", 9, 0, &saRet);
	FillSafeArray(L"Nov", 10, 0, &saRet);
	FillSafeArray(L"Dec", 11, 0, &saRet);

	range = sheet.get_Range(COleVariant(TEXT("A3")), COleVariant(TEXT("A14")));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	//Fill B3:E14 with values
	range = sheet.get_Range(COleVariant(TEXT("B3")), COleVariant(TEXT("B3")));
	range.put_Value2(COleVariant(double(7.0)));
	range = sheet.get_Range(COleVariant(TEXT("B4")), COleVariant(TEXT("B4")));
	range.put_Value2(COleVariant(double(6.9)));
	range = sheet.get_Range(COleVariant(TEXT("B5")), COleVariant(TEXT("B5")));
	range.put_Value2(COleVariant(double(9.5)));
	range = sheet.get_Range(COleVariant(TEXT("B6")), COleVariant(TEXT("B6")));
	range.put_Value2(COleVariant(double(14.5)));
	range = sheet.get_Range(COleVariant(TEXT("B7")), COleVariant(TEXT("B7")));
	range.put_Value2(COleVariant(double(18.2)));
	range = sheet.get_Range(COleVariant(TEXT("B8")), COleVariant(TEXT("B8")));
	range.put_Value2(COleVariant(double(21.5)));
	range = sheet.get_Range(COleVariant(TEXT("B9")), COleVariant(TEXT("B9")));
	range.put_Value2(COleVariant(double(25.2)));
	range = sheet.get_Range(COleVariant(TEXT("B10")), COleVariant(TEXT("B10")));
	range.put_Value2(COleVariant(double(26.5)));
	range = sheet.get_Range(COleVariant(TEXT("B11")), COleVariant(TEXT("B11")));
	range.put_Value2(COleVariant(double(23.3)));
	range = sheet.get_Range(COleVariant(TEXT("B12")), COleVariant(TEXT("B12")));
	range.put_Value2(COleVariant(double(18.3)));
	range = sheet.get_Range(COleVariant(TEXT("B13")), COleVariant(TEXT("B13")));
	range.put_Value2(COleVariant(double(13.9)));
	range = sheet.get_Range(COleVariant(TEXT("B14")), COleVariant(TEXT("B14")));
	range.put_Value2(COleVariant(double(9.6)));

	range = sheet.get_Range(COleVariant(TEXT("C3")), COleVariant(TEXT("C3")));
	range.put_Value2(COleVariant(double(-0.2)));
	range = sheet.get_Range(COleVariant(TEXT("C4")), COleVariant(TEXT("C4")));
	range.put_Value2(COleVariant(double(0.8)));
	range = sheet.get_Range(COleVariant(TEXT("C5")), COleVariant(TEXT("C5")));
	range.put_Value2(COleVariant(double(5.7)));
	range = sheet.get_Range(COleVariant(TEXT("C6")), COleVariant(TEXT("C6")));
	range.put_Value2(COleVariant(double(11.3)));
	range = sheet.get_Range(COleVariant(TEXT("C7")), COleVariant(TEXT("C7")));
	range.put_Value2(COleVariant(double(17.0)));
	range = sheet.get_Range(COleVariant(TEXT("C8")), COleVariant(TEXT("C8")));
	range.put_Value2(COleVariant(double(22.0)));
	range = sheet.get_Range(COleVariant(TEXT("C9")), COleVariant(TEXT("C9")));
	range.put_Value2(COleVariant(double(24.8)));
	range = sheet.get_Range(COleVariant(TEXT("C10")), COleVariant(TEXT("C10")));
	range.put_Value2(COleVariant(double(24.1)));
	range = sheet.get_Range(COleVariant(TEXT("C11")), COleVariant(TEXT("C11")));
	range.put_Value2(COleVariant(double(20.1)));
	range = sheet.get_Range(COleVariant(TEXT("C12")), COleVariant(TEXT("C12")));
	range.put_Value2(COleVariant(double(14.1)));
	range = sheet.get_Range(COleVariant(TEXT("C13")), COleVariant(TEXT("C13")));
	range.put_Value2(COleVariant(double(8.6)));
	range = sheet.get_Range(COleVariant(TEXT("C14")), COleVariant(TEXT("C14")));
	range.put_Value2(COleVariant(double(2.5)));

	range = sheet.get_Range(COleVariant(TEXT("D3")), COleVariant(TEXT("D3")));
	range.put_Value2(COleVariant(double(-0.9)));
	range = sheet.get_Range(COleVariant(TEXT("D4")), COleVariant(TEXT("D4")));
	range.put_Value2(COleVariant(double(0.6)));
	range = sheet.get_Range(COleVariant(TEXT("D5")), COleVariant(TEXT("D5")));
	range.put_Value2(COleVariant(double(3.5)));
	range = sheet.get_Range(COleVariant(TEXT("D6")), COleVariant(TEXT("D6")));
	range.put_Value2(COleVariant(double(8.4)));
	range = sheet.get_Range(COleVariant(TEXT("D7")), COleVariant(TEXT("D7")));
	range.put_Value2(COleVariant(double(13.5)));
	range = sheet.get_Range(COleVariant(TEXT("D8")), COleVariant(TEXT("D8")));
	range.put_Value2(COleVariant(double(17.0)));
	range = sheet.get_Range(COleVariant(TEXT("D9")), COleVariant(TEXT("D9")));
	range.put_Value2(COleVariant(double(18.6)));
	range = sheet.get_Range(COleVariant(TEXT("D10")), COleVariant(TEXT("D10")));
	range.put_Value2(COleVariant(double(17.9)));
	range = sheet.get_Range(COleVariant(TEXT("D11")), COleVariant(TEXT("D11")));
	range.put_Value2(COleVariant(double(14.3)));
	range = sheet.get_Range(COleVariant(TEXT("D12")), COleVariant(TEXT("D12")));
	range.put_Value2(COleVariant(double(9.0)));
	range = sheet.get_Range(COleVariant(TEXT("D13")), COleVariant(TEXT("D13")));
	range.put_Value2(COleVariant(double(3.9)));
	range = sheet.get_Range(COleVariant(TEXT("D14")), COleVariant(TEXT("D14")));
	range.put_Value2(COleVariant(double(1.0)));

	range = sheet.get_Range(COleVariant(TEXT("E3")), COleVariant(TEXT("E3")));
	range.put_Value2(COleVariant(double(3.9)));
	range = sheet.get_Range(COleVariant(TEXT("E4")), COleVariant(TEXT("E4")));
	range.put_Value2(COleVariant(double(4.2)));
	range = sheet.get_Range(COleVariant(TEXT("E5")), COleVariant(TEXT("E5")));
	range.put_Value2(COleVariant(double(5.7)));
	range = sheet.get_Range(COleVariant(TEXT("E6")), COleVariant(TEXT("E6")));
	range.put_Value2(COleVariant(double(8.5)));
	range = sheet.get_Range(COleVariant(TEXT("E7")), COleVariant(TEXT("E7")));
	range.put_Value2(COleVariant(double(11.9)));
	range = sheet.get_Range(COleVariant(TEXT("E8")), COleVariant(TEXT("E8")));
	range.put_Value2(COleVariant(double(15.2)));
	range = sheet.get_Range(COleVariant(TEXT("E9")), COleVariant(TEXT("E9")));
	range.put_Value2(COleVariant(double(17.0)));
	range = sheet.get_Range(COleVariant(TEXT("E10")), COleVariant(TEXT("E10")));
	range.put_Value2(COleVariant(double(16.6)));
	range = sheet.get_Range(COleVariant(TEXT("E11")), COleVariant(TEXT("E11")));
	range.put_Value2(COleVariant(double(14.2)));
	range = sheet.get_Range(COleVariant(TEXT("E12")), COleVariant(TEXT("E12")));
	range.put_Value2(COleVariant(double(10.3)));
	range = sheet.get_Range(COleVariant(TEXT("E13")), COleVariant(TEXT("E13")));
	range.put_Value2(COleVariant(double(6.6)));
	range = sheet.get_Range(COleVariant(TEXT("E14")), COleVariant(TEXT("E14")));
	range.put_Value2(COleVariant(double(4.8)));

	// Open Excel



	CString sTop;
	CString sData;
	CString sChart;
	VARIANT ret;
	CString str1;
	double buf[30][30];
	for (int i = 0; i < 30; i++)
	for (int j = 0; j < 30; j++)
		buf[i][j] = 0;
	CString buf_c[30];
	for (int i = 0; i < 30; i++)
		buf_c[i] = _T("");

	WCHAR *stop_c;
	// CString sRow;
	//	CString sIdx_l;
	CString sIdx;
	for (int row_idx = 0; row_idx < 30; row_idx++)
	{
		for (int col_idx = 0; col_idx < 30; col_idx++)
		{
			int first_char = col_idx / 26;
			int last_char = col_idx % 26;
			if (first_char > 0)
				sIdx.Format(_T("%c"), 'A' + first_char - 1);
			else
				sIdx.Format(_T(""));
			sIdx.AppendFormat(_T("%c"), 'A' + last_char);
			sIdx.AppendFormat(_T("%d"), row_idx + 1);
			range = sheet.get_Range(COleVariant(sIdx), COleVariant(sIdx));
			ret = range.get_Value(vtMissing);



			if (ret.vt != VT_NULL)
			{
				str1 = (LPCTSTR)(_bstr_t)ret;
				buf[row_idx][col_idx] = _tcstod(str1, &stop_c);
				if (row_idx == 1)
					buf_c[col_idx] = str1;
			}
		}
	}

	//CString "sData" for html table 
	for (int row = 0; row < 14; row++)
	{
		sData.AppendFormat(_T("<tr>\n"));
		for (int col = 0; col < 5; col++)
		{
			sData.AppendFormat(_T("<td>"));
			sData.AppendFormat(_T("%3.2f"), buf[row][col]);
			sData.AppendFormat(_T("</td>\n"));
		}
		sData.AppendFormat(_T("</tr>\n"));
	}

	//CString "sChart" for Highcharts
	for (int col = 0; col < 5; col++)
	{
		if (col > 0)
		for (int row = 0; row < 14; row++)
		{
			if (row == 0)
			{
				sChart.Append(_T("{\n"));
				sChart.AppendFormat(_T("name: '"));
				sChart.AppendFormat(_T("%s"), buf_c[col]);
				//sChart.AppendFormat(_T("test"));
				sChart.AppendFormat(
					_T("' ,\n")
					_T("data :[")
					);
			}
			else if (row > 1)
			{
				sChart.AppendFormat(_T("%2.1f"), buf[row][col]);
				sChart.AppendFormat(_T(","));
			}
		}
		if (col > 0){
			sChart.AppendFormat(
				_T("]\n")
				_T("}")
				);
			if (col < 4)
				sChart.AppendFormat(_T(", \n"));
		}
	}

	//CString sGTable_c (column name) for google Table
	CString sGTable_c;
	for (int col = 1; col < 5; col++)
	{
		sGTable_c.AppendFormat(_T("data.addColumn('number', '"));
		sGTable_c.AppendFormat(_T("%s"), buf_c[col]);
		sGTable_c.AppendFormat(_T("');\n"));

	}

	//CString sGTable (data) for google table
	CString sGTable;
	for (int row = 2; row < 14; row++)
	{
		sGTable.AppendFormat(_T("["));
		for (int col = 1; col < 5; col++)
		{
			sGTable.AppendFormat(_T(" %2.1f"), buf[row][col]);
			if (col < 4)
				sGTable.AppendFormat(_T(", "));
		}
		sGTable.AppendFormat(_T("]"));

		if (row < 13)
			sGTable.AppendFormat(_T(", \n"));
	}



	sTop.AppendFormat(
		_T("<!DOCTYPE html>\n")
		_T("</html>\n")
		_T("<body>\n")
		_T("<img src =\"AutomateExcel\\RICHTEK_LOGO.png\" alt = \"\" /> \n")
		_T("</body>\n")

		_T("<body>")
		_T("<p><a href= \"http://www.richtek.com\" target=\"_blanl\"> Visit RICHTEK </a></p> \n")
		_T("</body>\n")

		_T("<head>\n")
		_T("    <h1>Project Name</h1>\n")
		_T("</head>\n")
		);
	sTop.AppendFormat(
		_T("<body>\n")
		_T("    <p>Function Descripition<p>\n")
		_T("</body>\n")
		);
	sTop.AppendFormat(
		_T("<body>\n")
		_T("<table border= \"1\" style = \"width:50%%\">\n")
		);
	sTop.Append(sData);
	sTop.AppendFormat(
		_T("</table>\n")
		_T("</body>\n")
		);

	sTop.AppendFormat(
		_T("<head>\n")
		_T("	<meta http-equiv=\"Content-Type\" content= \"text / html; charset=utf-8\">\n")
		_T("	<title>Highcharts Example</title>\n")
		_T("	<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js\"></script>\n")
		_T("	<style type=\"text/css\">\n")
		_T("		${demo.css}\n")
		_T("	</style>\n")
		_T("	<script type = \"text/javascript\">\n")
		_T("$(function() {\n")
		_T("	$('#container').highcharts({\n")
		_T("		title: {\n")
		_T("			text: 'Monthly Average Temperature',\n")
		_T("			x : -20 //center\n")
		_T("		},")
		_T("		subtitle: {\n")
		_T("			text: 'Source: WorldClimate.com',\n")
		_T("			x : -20\n")
		_T("		},\n")
		_T("		xAxis : {\n")
		_T("			categories:['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',\n")
		_T("			'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']\n")
		_T("		},\n")
		_T("		yAxis : {\n")
		_T("			title: {\n")
		_T("				text: 'Temperature (°C)'\n")
		_T("			},")
		_T("			plotLines : [{\n")
		_T("			value: 0,\n")
		_T("			width : 1, \n")
		_T("			color : '#808080'\n")
		_T("			}]\n")
		_T("		},\n")
		_T("		tooltip: {\n")
		_T("			valueSuffix: '°C'\n")
		_T("		},\n")
		_T("		legend : {\n")
		_T("			layout: 'vertical',\n")
		_T("			align : 'right',\n")
		_T("			verticalAlign : 'middle',\n")
		_T("			borderWidth : 0\n")
		_T("		}, \n")
		_T("series:[\n")
		);
	sTop.Append(sChart);
	sTop.AppendFormat(
		_T("]\n")
		_T("	}); \n")
		_T(" }); \n")
		_T("</script>\n")
		_T("</head>\n")
		_T("<body>\n")
		_T("	<script src = \"js/highcharts.js\"></script>\n")
		_T("<script src = \"js/modules/exporting.js\"></script>\n")
		_T("<div id = \"container\" style = \"min-width: 310px; height: 400px; margin: 0 auto\"></div>\n")
		_T("</body>\n")
		);

	sTop.AppendFormat(
		_T("<head>\n")
		_T("<script type = \"text/javascript\" src = \"https://www.google.com/jsapi\"></script>\n")
		_T("<script type = \"text/javascript\">\n")
		_T("google.load(\"visualization\", \"1.1\", { packages:[\"table\"] });\n")
		_T("google.setOnLoadCallback(drawTable);\n")
		_T("function drawTable() {\n")
		_T("var data = new google.visualization.DataTable();\n")
		);
	sTop.AppendFormat(sGTable_c);
	sTop.AppendFormat(_T("data.addRows([\n"));
	sTop.Append(sGTable);
	sTop.AppendFormat(
		_T("]);\n")
		_T("   var table = new google.visualization.Table(document.getElementById('table_div')); \n")
		_T("   var table = new google.visualization.Table(document.getElementById('colorformat_div'));\n")
		_T("   var table = new google.visualization.Table(document.getElementById('numberformat_div'));\n")

		_T("   var formatter = new google.visualization.NumberFormat(\n")
		_T("   {prefix: '$', negativeColor: 'red', negativeParens: true});\n")
		_T("   formatter.format(data, 0); // Apply formatter to second column\n")

		/*	_T("   var formatter = new google.visualization.NumberFormat(\n")
			_T("   {prefix: '$', negativeColor: 'red', negativeParens: true});\n")
			_T("   formatter.format(data, 1); // Apply formatter to second column\n")

			_T("   var formatter = new google.visualization.NumberFormat(\n")
			_T("   {prefix: '$', negativeColor: 'red', negativeParens: true});\n")
			_T("   formatter.format(data, 2); // Apply formatter to second column\n")

			_T("   var formatter = new google.visualization.NumberFormat(\n")
			_T("   {prefix: '$', negativeColor: 'red', negativeParens: true});\n")
			_T("   formatter.format(data, 3); // Apply formatter to second column\n")  */

			//	_T("   var formatter = new google.visualization.ColorFormat();\n")
			//	_T("   formatter.addRange(0, 10000, 'white', 'orange');\n")
			//	_T("   formatter.addRange(10000, null, 'red', '#33ff33');\n")
			//	_T("   formatter.format(data, 1); // Apply formatter to second column\n")
			_T("   table.draw(data, {allowHtml: true, showRowNumber: true, width: '50%%', height: '50%%'});\n")
			_T("  }\n")
			_T(" </script>\n")
			_T("</head>\n")
			_T(" <body>\n")
			_T("<div id=\"table_div\"></div> \n")
			_T("<div id=\"colorformat_div\"></div> \n")
			_T("<div id=\"numberformat_div\"></div> \n")
			_T(" </body>\n")
			);


	sTop.AppendFormat(
		_T("</html>\n")
		);

	CString s;
	CString str(_T("Microsoft Office HTML Files (*.html)|*.html|All Files(*.*)|*.*||"));
	LPTSTR szFilters;
	szFilters = (LPTSTR)(LPCTSTR)str;
	s.Format(_T("report"));
	CFileDialog OpenfileDlg(FALSE, _T("html"), s,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
	if (OpenfileDlg.DoModal() != IDOK)
		return;
	CString sFile2 = OpenfileDlg.GetPathName();
	CStdioFile file;
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		s.Format(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return;
	}

	//file.WriteString(CT2A(sTop));
	file.Write(CT2A(sTop), sTop.GetLength());
	file.Close();


	//Format A1:C1 as bold, vertical alignment = center.
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("C1")));
	font = range.get_Font();
	font.put_Bold(covTrue);
	range.put_VerticalAlignment(COleVariant((short)-4108));   //xlVAlignCenter = -4108

	//AutoFit columns A:D.
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("D1")));
	CRange cols;
	cols = range.get_EntireColumn();
	cols.AutoFit();

	//Adding Chart
	CCharts charts;
	CChart chart;
	charts = book.get_Charts();
	chart = charts.Add(covOptional, covOptional, covOptional);

	app.put_Visible(TRUE);
	app.put_UserControl(TRUE);
}


void CAutomateExcelDlg::OnBnClickedAte()
{
	Excel_to_buffer();
	gen_html_head();
	int x = 4;
	for (int nMeas = 0; nMeas < m_nMeasCnt; nMeas++)
	{
		for (int y = 0; y < m_nMeasCol; y++)
		{
			gen_curve(m_sCellTitle[y], m_sCellTitle[x], m_sCellTitle[y], nMeas, x, y);
		}
	}

	/*for (int nMeas = 0; nMeas < m_nMeasCnt; nMeas++)
	{
	for (int y = 0; y < m_nMeasCol; y++)
	{

	}
	}*/



	gen_html_bottom();
	save_file();

}


void CAutomateExcelDlg::Excel_to_buffer()
{
	// Commonly used OLE variants.
	//--------------------------------------------------------------------------------
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	CApplication app;
	//--------------------------------------------------------------------------------

	// Start Excel and get an Application object.
	//--------------------------------------------------------------------------------
	if (!app.CreateDispatch(TEXT("Excel.Application")))
	{
		AfxMessageBox(TEXT("Couldn't start Excel and get Application object."));
		return;
	}

	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;
	CRange range;
	CFont0 font;
	// Open Excel file
	CString       strExcelPath = _T("D:\\2. My_Design\\AutomateExcel\\RT4832_1024_11Bit_Exponential.xlsx");
	books.AttachDispatch(app.get_Workbooks(), true);
	LPDISPATCH lpDisp = NULL;
	lpDisp = books.Add(COleVariant(strExcelPath));
	book.AttachDispatch(lpDisp);
	//Open 
	sheets.AttachDispatch(book.get_Worksheets(), true);
	CString strSheetName = _T("ATE Measurement Result");
	lpDisp = sheets.get_Item(COleVariant(strSheetName));
	sheet.AttachDispatch(lpDisp);
	//--------------------------------------------------------------------------------

	// get row & column number
	//--------------------------------------------------------------------------------
	CRange range_row;
	CRange range_col;
	CRange usedRange;
	usedRange.AttachDispatch(sheet.get_UsedRange(), true);
	range_row.AttachDispatch(usedRange.get_Rows(), true);
	range_col.AttachDispatch(usedRange.get_Columns(), true);
	int nrow_idx = range_row.get_Count();
	int ncol_idx = range_col.get_Count();
	usedRange.ReleaseDispatch();
	range_row.ReleaseDispatch();
	range_col.ReleaseDispatch();
	//--------------------------------------------------------------------------------

	for (int i = 0; i < MAX_MEASURE_CNT; i++)
		m_nDataLength[i] = 0;
	for (int i = 0; i < MAX_MEASURE_CNT; i++)
	for (int j = 0; j < MAX_COL; j++)
	for (int k = 0; k < MAX_BUFFER_LENGTH; k++)
		m_fData[i][j][k] = 0;
	for (int i = 0; i < MAX_COL; i++)
		m_sCellTitle[i] = _T("");




	VARIANT ret;
	CString str1;
	//int row_temp;

	double buf[256][10];
	for (int i = 0; i < 256; i++)
	for (int j = 0; j < 10; j++)
		buf[i][j] = 0.0;

	float buf_gain[10];
	for (int i = 0; i < 10; i++)
		buf_gain[i] = 0.0;

	WCHAR *stop_c;
	CString sIdx;
	CString sFirstTitle(_T(""));
	int nIdx = 0;
	int nMeasCnt = 0;
	bool bDone = false;
	double unit_gain = 0.0;
	for (int col_idx = 0; col_idx < 15; col_idx++)
	{
		for (int row_idx = 0; row_idx < 3000; row_idx++)
		{
			// get cell position
			int first_char = col_idx / 26;
			int last_char = col_idx % 26;
			if (first_char > 0)
				sIdx.Format(_T("%c"), 'A' + first_char - 1);
			else
				sIdx.Format(_T(""));
			sIdx.AppendFormat(_T("%c"), 'A' + last_char);
			sIdx.AppendFormat(_T("%d"), row_idx + 1);
			range = sheet.get_Range(COleVariant(sIdx), COleVariant(sIdx));
			ret = range.get_Value(vtMissing);

			if (ret.vt != VT_NULL)
			{
				str1 = (LPCTSTR)(_bstr_t)ret;

				if ((str1 == _T("")) && (row_idx == 0))
				{
					m_nMeasCol = col_idx;
					col_idx = 0;
					bDone = true;
					break;
					//	return;
				}

				// check last row
				if (str1 == _T(""))
				{
					if (col_idx == 0)
						m_nDataLength[nMeasCnt] = nIdx - 1;  // save last measure title
					m_nMeasCnt = nMeasCnt + 1;
					nMeasCnt = 0;
					nIdx = 0;
					break;
				}

				if (row_idx == 0)
				{
					m_sCellTitle[col_idx] = str1;
					CString s;
					s.Format(_T("%s"), str1);
					int pos1 = s.Find(_T("("));
					int pos2 = s.Find(_T(")"));
					CString s2 = s.Mid(pos1 + 1, pos2 - pos1 - 1);
					if (s2 == _T("A"))
						unit_gain = 1000.0; // default (A) ==> (mA)
					else
						unit_gain = 1.0;  // others keep
					if (s2.Left(2) == _T("mA"))
						unit_gain = 1.0;
					else if (s2.Left(2) == _T("uA"))
						unit_gain = 0.001;
					buf_gain[col_idx] = unit_gain; // buf[] unit fixed to mA
				}
				// check cell is title
				if ((row_idx != 0) && m_sCellTitle[col_idx] == str1)
				{
					if (col_idx == 0)
						m_nDataLength[nMeasCnt] = nIdx - 1;
					nMeasCnt++;
					nIdx = 1;
					continue;
				}

				if (nIdx != 0)	// not title string
				{
					double fCellData = _tcstod(str1, &stop_c);
					if (unit_gain != 1)
						m_fData[nMeasCnt][col_idx][nIdx - 1] = fCellData * buf_gain[col_idx];
					else
						m_fData[nMeasCnt][col_idx][nIdx - 1] = fCellData;
				}
				nIdx++;
			}
		} // end of row_idx

		if (bDone)
			break;

	} // end of col_idx

	//if (bDone) .... 

}
void CAutomateExcelDlg::gen_html_head()
{

	m_sHtml.AppendFormat(
		_T("<!DOCTYPE html>\n")
		_T("<html>\n")
		_T("	<meta http-equiv=\"Content-Type\" content= \"text / html; charset=utf-8\">\n")
		_T("	<title>ATE Report</title>\n")
		_T("	<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js\"></script>\n")
		_T("	<style type=\"text/css\">\n")
		_T("		${demo.css}\n")
		_T("	</style>\n")
		_T("	<script src = \"js/highcharts.js\"></script>\n")
		_T("	<script src = \"js/modules/exporting.js\"></script>\n")

		_T("	<body>\n")
		_T("	<img src =\"AutomateExcel\\RICHTEK_LOGO.png\" alt = \"\" /> \n")
		_T("	<p><a href= \"http://www.richtek.com\" target=\"_blanl\"> Visit RICHTEK </a></p> \n")
		_T("</body>\n")

		_T("<head>\n")
		_T("    <h1>Project Name</h1>\n")
		_T("<body>\n")
		_T("    <p>Function Descripition<p>\n")
		_T("</body>\n")
		_T("</head>\n")
		);
}

void CAutomateExcelDlg::gen_curve(CString sTitleTop, CString sTitleX, CString sTitleY, int nIndexMeas, int nIndexX, int nIndexY)
{
	CString sxasis;   // setting X-Axis format for highcharts basic line 
	int nrow_x = m_nDataLength[nIndexMeas] / 8;
	int ncol_x = m_nDataLength[nIndexMeas] % 8;
	bool bxDone1 = false;
	bool bxDone2 = false;
	for (int i = 0; i < nrow_x + 1; i++)
	{
		for (int j = 0; j < ncol_x + 1; j++)
		{
			if (i == nrow_x && j == ncol_x)
			{
				bxDone1 = true;
				break;
			}
			sxasis.AppendFormat(_T("'"));
			sxasis.AppendFormat(_T("%3.0f"), m_fData[nIndexMeas][nIndexX][i * 8 + j]);
			sxasis.AppendFormat(_T("'"));
			if (i == nrow_x && j == ncol_x - 1)
			{
				bxDone2 = true;
				break;
			}
			sxasis.AppendFormat(_T(","));

		}
		if (bxDone1 || bxDone2)
			break;
		sxasis.AppendFormat(_T("\n"));
	}

	CString syasis;   // setting X-Axis format for highcharts basic line 
	int nrow_y = m_nDataLength[nIndexMeas] / 8;
	int ncol_y = m_nDataLength[nIndexMeas] % 8;
	bool byDone1 = false;
	bool byDone2 = false;
	for (int i = 0; i < nrow_y + 1; i++)
	{
		for (int j = 0; j < ncol_y + 1; j++)
		{
			if (i == nrow_y && j == ncol_y)
			{
				byDone1 = true;
				break;
			}
			//syasis.AppendFormat(_T("'"));
			syasis.AppendFormat(_T("%3.8f"), m_fData[nIndexMeas][nIndexY][i * 8 + j]);
			//syasis.AppendFormat(_T("'"));
			if (i == nrow_y && j == ncol_y - 1)
			{
				byDone2 = true;
				break;
			}
			syasis.AppendFormat(_T(","));

		}
		if (byDone1 || byDone2)
			break;
		syasis.AppendFormat(_T("\n"));
	}

	m_sHtml.AppendFormat(
		_T("	<script type = \"text/javascript\">\n")
		_T("$(function() {\n")
		_T("	$('#")
		);
	m_sHtml.AppendFormat(_T("%d"), nIndexMeas);		//define id
	m_sHtml.AppendFormat(_T("%d"), nIndexY);
	m_sHtml.AppendFormat(
		_T("').highcharts({\n")
		_T("		title: {\n")
		_T("			text: '")
		);
	m_sHtml.AppendFormat(_T("%s"), sTitleTop);		//define chart name	
	m_sHtml.AppendFormat(
		_T("',\n")
		_T("			x : -20 //center\n")
		_T("		},")
		_T("		xAxis : {\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sHtml.AppendFormat(_T("%s"), sTitleX);		//define X-Axis title
	m_sHtml.AppendFormat(
		_T("'\n")
		_T("			},")
		_T("			categories:[")
		);
	m_sHtml.Append(sxasis);							//define X-Axis 
	m_sHtml.AppendFormat(
		_T("]\n")
		_T("		},\n")
		_T("		yAxis : {\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sHtml.AppendFormat(_T("%s"), sTitleY);		//define Y-Axis title
	m_sHtml.AppendFormat(
		_T("'\n")
		_T("			},")
		_T("			plotLines : [{\n")
		_T("			value: 0,\n")
		_T("			width : 1, \n")
		_T("			color : '#808080'\n")
		_T("			}]\n")
		_T("		},\n")
		//	_T("		tooltip: {\n")
		//	_T("			valueSuffix: '°C'\n")
		//	_T("		},\n")
		_T("		legend : {\n")
		//_T("			layout: 'vertical',\n")
		_T("			align : 'center',\n")
		_T("			verticalAlign : 'bottom',\n")
		//	_T("			borderWidth : 1,\n")
		//	_T(" backgroundColor : ((Highcharts.theme && Highcharts.theme.legendBackgroundColor) || '#FFFFFF'), \n")
		//	_T("shadow :　true")
		_T("		}, \n")
		_T("series:[\n")
		_T("{\n")
		_T("name: '")
		);
	m_sHtml.AppendFormat(_T("%s"), sTitleY);		//define Y-Axis title
	m_sHtml.AppendFormat(
		_T("',\n")
		_T("data :[")
		);
	m_sHtml.Append(syasis);							//define Y-Axis 
	m_sHtml.AppendFormat(
		_T("]\n")
		_T("	}] \n")
		_T("	}); \n")
		_T(" }); \n")
		_T("</script>\n")
		);
}

void CAutomateExcelDlg::gen_html_bottom()
{
	for (int nMeas = 0; nMeas < m_nMeasCnt; nMeas++)
	{
		for (int y = 0; y < m_nMeasCol; y++)
		{
			m_sHtml.AppendFormat(_T("<div id = \""));
			m_sHtml.AppendFormat(_T("%d"), nMeas);		//define id
			m_sHtml.AppendFormat(_T("%d"), y);
			m_sHtml.AppendFormat(
				_T("\" style = \"min-width: 310px; height: 400px; margin: 0 auto\"></div>\n")
				_T("<br>\n")
				);
		}
	}
	m_sHtml.AppendFormat(
		_T("</html>\n"));
}


void CAutomateExcelDlg::save_file()
{
	CString s;
	CString str(_T("Microsoft Office HTML Files (*.html)|*.html|All Files(*.*)|*.*||"));
	LPTSTR szFilters;
	szFilters = (LPTSTR)(LPCTSTR)str;
	s.Format(_T("report_new"));
	CFileDialog OpenfileDlg(FALSE, _T("html"), s,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
	if (OpenfileDlg.DoModal() != IDOK)
		return;
	CString sFile2 = OpenfileDlg.GetPathName();
	CStdioFile file;
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		s.Format(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return;
	}

	//file.WriteString(CT2A(sTop));
	file.Write(CT2A(m_sHtml), m_sHtml.GetLength());
	file.Close();
}


void CAutomateExcelDlg::OnBnClickedMultiAxes()
{

	//gen_html_head_macurve();
	//	gen_multi_axes();
	gen_fod_chart();
//	gen_html_bottom_macurve();
	save_file_macurve();

}
CString CAutomateExcelDlg::gen_html_head_macurve()
{
	CString sHead(_T(""));
	sHead.AppendFormat(
		_T("<!DOCTYPE html>\n")
		_T("<html>\n")
		_T("	<meta http-equiv=\"Content-Type\" content= \"text / html; charset=utf-8\">\n")
		_T("	<title>ATE Report</title>\n")
		_T("	<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js\"></script>\n")
		_T("	<style type=\"text/css\">\n")
		_T("		${demo.css}\n")
		_T("	</style>\n")
		_T("	<script src = \"js/highcharts.js\"></script>\n")
		_T("	<script src = \"js/modules/exporting.js\"></script>\n")
		_T("	<script type = \"text/javascript\">\n")
		);
	return sHead;
}

void CAutomateExcelDlg::gen_multi_axes()
{
	double m_dVrect[80] = {	// unit: V
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

	double m_dVout[80] = {	// unit: mA
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

	double m_dIout[80] = {	// unit: mA
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

	double m_dRP[80] = {	// unit: mW
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
	double m_ELoadI[80] = {	// unit: mA
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

	double m_ELoadV[80] = {	// unit: V
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
	CString sxasis;   // setting X-Axis format for highcharts basic line 
	int xasislength = 80;
	for (int i = 0; i < xasislength; i++)
	{
		sxasis.AppendFormat(_T("'"));
		sxasis.AppendFormat(_T("%i"), i);
		sxasis.AppendFormat(_T("'"));
		if (i < xasislength - 1)
			sxasis.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			sxasis.AppendFormat(_T("\n"));
	}

	int yasislength = 80;
	CString syasis1;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis1.AppendFormat(_T("%4.1f"), m_dRP[i]);
		if (i < yasislength - 1)
			syasis1.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis1.AppendFormat(_T("\n"));
	}
	CString syasis2;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis2.AppendFormat(_T("%1.4f"), m_dVrect[i]);
		if (i < yasislength - 1)
			syasis2.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis2.AppendFormat(_T("\n"));
	}
	CString syasis3;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis3.AppendFormat(_T("%3.2f"), m_dIout[i]);
		if (i < yasislength - 1)
			syasis3.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis3.AppendFormat(_T("\n"));
	}
	CString syasis4;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis4.AppendFormat(_T("%1.4f"), m_dVout[i]);
		if (i < yasislength - 1)
			syasis4.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis4.AppendFormat(_T("\n"));
	}
	CString syasis5;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis5.AppendFormat(_T("%3.2f"), 1000 * m_ELoadI[i]);
		if (i < yasislength - 1)
			syasis5.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis5.AppendFormat(_T("\n"));
	}
	CString syasis6;   // setting 1st data format for highcharts basic line 
	for (int i = 0; i < yasislength; i++)
	{
		syasis6.AppendFormat(_T("%1.4f"), m_ELoadV[i]);
		if (i < yasislength - 1)
			syasis6.AppendFormat(_T(","));
		if (i % 8 == 7 && i < xasislength - 1)
			syasis6.AppendFormat(_T("\n"));
	}



	m_sCurve.AppendFormat(
		_T("	<script type = \"text/javascript\">\n")
		_T("$(function() {\n")
		_T("	$('#")
		);
	m_sCurve.AppendFormat(_T("0"));		//define id
	m_sCurve.AppendFormat(_T("0"));
	m_sCurve.AppendFormat(
		_T("').highcharts({\n")
		_T("		chart: {")
		_T("			zoomType: 'xy'")
		_T("		},")
		_T("		title: {\n")
		_T("			text: '")
		);
	m_sCurve.AppendFormat(_T("FOD Accuracy"));		//define chart name	
	m_sCurve.AppendFormat(
		_T("'\n")
		_T("		},\n")
		_T("		xAxis : [{\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sCurve.AppendFormat(_T("number(n)"));		//define X-Axis title
	m_sCurve.AppendFormat(
		_T("'\n")
		_T("			},\n")
		_T("			categories:[")
		);
	m_sCurve.Append(sxasis);							//define X-Axis 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("crosshair: true\n")
		_T("		}],\n")
		_T("plotOptions: {\n")
		_T("	series: {\n")
		_T("		fillColor: {\n")
		_T("			linearGradient:[0, 0, 0, 300],\n")
		_T("			stops : [\n")
		_T("				[0, Highcharts.getOptions().colors[0]],\n")
		_T("				[1, Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0).get('rgba')]\n")
		_T("			]\n")
		_T("		}\n")
		_T("	}\n")
		_T("},\n")

		_T("		yAxis : [{ // Primart yAxis \n")
		_T("			labels: {\n")
		_T("				format: '{value} V',\n")	//define 1st Y-Axis unit
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sCurve.AppendFormat(_T("Voltage"));		//define 1st Y-Axis title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			opposite: false\n")
		_T("		},\n")
		_T("			{ // Secondary yAxis \n")
		_T("			gridLineWidth: 0,\n")
		_T("			labels: {\n")
		_T("				format: '{value} mA',\n")	//define 2nd Y-Axis unit
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sCurve.AppendFormat(_T("Current"));		//define 2nd Y-Axis title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			opposite: true\n")
		_T("		},\n")
		_T("			{ // Tertiary yAxis \n")
		_T("			gridLineWidth: 0,\n")
		_T("			labels: {\n")
		_T("				format: '{value} mW',\n")	//define 3rd Y-Axis unit
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			title: {\n")
		_T("				text: '")
		);
	m_sCurve.AppendFormat(_T("Power"));		//define 3rd Y-Axis title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T("				style: {\n")
		_T("					color: Highcharts.getOptions().colors[1]\n")
		_T("				}\n")
		_T("			},\n")
		_T("			opposite: true\n")
		_T("		}],\n")

		_T("tooltip: {\n")
		_T("shared: true\n")
		_T("},\n")
		_T("		legend : {\n")
		//_T("			layout: 'vertical',\n")
		_T("			align : 'center',\n")
		_T("			x : 0,\n")
		_T("			verticalAlign : 'bottom',\n")
		_T("			y : 0,\n")
		_T("			floating: false,\n")
		_T("			backgroundColor : (Highcharts.theme && Highcharts.theme.legendBackgroundColor) || '#FFFFFF'\n")
		_T("		}, \n")
		_T("series:[\n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("RP"));		//define 1st Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'area',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 2,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis1);							//define 1st Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'mW'\n")
		_T("	} \n")
		_T("	}, \n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("VRECT"));		//define 2nd Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'spline',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 0,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis2);							//define 2nd Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'V'\n")
		_T("	} \n")
		_T("	}, \n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("IOUT"));		//define 3rd Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'spline',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 1,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis3);							//define 3rd Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'mA'\n")
		_T("	} \n")
		_T("	}, \n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("VOUT"));		//define 4th Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'spline',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 0,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis4);							//define 4th Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'mW'\n")
		_T("	} \n")
		_T("	}, \n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("ELoad_Iout"));		//define 5th Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'spline',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 1,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis5);							//define 5th Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'mA'\n")
		_T("	} \n")
		_T("	}, \n")
		_T("{\n")
		_T("name: '")
		);
	m_sCurve.AppendFormat(_T("ELoad_Vout"));		//define 6th Data title
	m_sCurve.AppendFormat(
		_T("',\n")
		_T(" type: 'spline',\n")
		_T(" marker: {\n")
		_T("	enabled: false\n")
		_T(" },\n")
		_T("yAxis: 0,\n")
		_T("data :[")
		);
	m_sCurve.Append(syasis6);							//define 6th Data 
	m_sCurve.AppendFormat(
		_T("],\n")
		_T("tooltip: {\n")
		_T("	valueSuffix: 'V'\n")
		_T("	} \n")
		//_T("	}, \n")
		_T("	}] \n")
		_T("	}); \n")
		_T(" }); \n")
		_T("</script>\n")
		);
}

CString CAutomateExcelDlg::gen_js_chart(CString sID, HC_OUTLINE ol, HC_Y_AXIS Yaxis[], HC_X_AXIS Xaxis[])
{
	CString s(_T(""));
	s.AppendFormat(
		_T("$(function () {\n")
		_T("$('#")
		);
	s.Append(sID);
	s.AppendFormat(
		_T("').highcharts({\n")
		_T("    chart: { zoomType: 'xy' },\n")
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
		s.AppendFormat(_T("%1.0f"), ol.pfX_label[i]);
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
			_T("     gridLineWidth: 0,\n")
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
		_T("     verticalAlign: 'bottom',\n")
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
			_T("                    enabled: true\n")
			_T("                },\n")
			_T("                yAxis: ")
			);
		s.AppendFormat(_T("%d"), Xaxis[i].nSeries_Y_sel);
		s.AppendFormat(
			_T(",\n")
			_T("                data: [\n")
			);
		for (int j = 0; j < ol.nX_data_length; j++)
		{
			s.AppendFormat(_T("%.1f"), Xaxis[i].pSeries_data[j]);
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
			_T("</script>\n")
			);
			return s;
	

}
void CAutomateExcelDlg::gen_fod_chart()
{
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
		X_label
	};

	HC_Y_AXIS Yaxis[] = {
		{ _T("{value}V"), _T("Voltage") },
		{ _T("{value}mA"), _T("Current") },
		{ _T("{value}mW"), _T("Power") },
		{ _T(""), _T("") }	// end marker
	};

	HC_X_AXIS Xaxis[] = {
		{ _T("RP"), 2, m_dIout, _T("mW") },
		{ _T("VRECT"), 0, m_dVrect, _T("V") },
		{ _T("VOUT"), 0, m_dVout, _T("V") },
		{ _T("Iout"), 1, m_dIout, _T("mA") },
		{ _T(""), 0, NULL, _T("") }  // end marker
	};

	CString sHead = gen_html_head_macurve();
	CString sCurve = gen_js_chart(_T("FOD_ACCuracy"), ol, Yaxis, Xaxis);
	CString sBottom = gen_html_bottom_macurve(_T("FOD_ACCuracy"));

	m_sCurve = sHead + sCurve + sBottom;


}



CString CAutomateExcelDlg::gen_html_bottom_macurve(CString sID)
{
	CString sBottom(_T(""));
	sBottom.AppendFormat(
		_T("<div id = \"")
		);
	sBottom.Append(sID);
	sBottom.AppendFormat(
		_T("\" style = \"min-width: 310px; height: 400px; margin: 0 auto\"></div>\n")
		_T("</html>\n"));
	return sBottom;
}


void CAutomateExcelDlg::save_file_macurve()
{
	CString s;
	CString str(_T("Microsoft Office HTML Files (*.html)|*.html|All Files(*.*)|*.*||"));
	LPTSTR szFilters;
	szFilters = (LPTSTR)(LPCTSTR)str;
	s.Format(_T("report_macurve_20151001"));
	CFileDialog OpenfileDlg(FALSE, _T("html"), s,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
	if (OpenfileDlg.DoModal() != IDOK)
		return;
	CString sFile2 = OpenfileDlg.GetPathName();
	CStdioFile file;
	if (file.Open(sFile2, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0)
	{
		s.Format(_T("Can't Open file !!"));
		AfxMessageBox(s);
		return;
	}

	//file.WriteString(CT2A(sTop));
	file.Write(CT2A(m_sCurve), m_sCurve.GetLength());
	file.Close();
}

