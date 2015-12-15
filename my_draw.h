// Wireless_RxDoc.h : interface of the CWireless_RxDoc class
//


#pragma once

#define DATA_LENGTH 1000
	typedef struct _DATA_LOG_DEF_ {
		CString sName;
		CString sLastValue;
		CString sUnit;
		float*  fData;
		float*	fToggleLog;
		float	fMax;
		float	fMin;
		CPoint	ptBase;
		double	dShowScale;
		COLORREF	clr;
		CPen	pen;
		BOOL	bShow;
		int		nDrawType;
	};


class CWireless_RxDoc : public CDocument
{
protected: // create from serialization only
	CWireless_RxDoc();
	DECLARE_DYNCREATE(CWireless_RxDoc)

// Attributes
public:
	_DATA_LOG_DEF_ new_vout;
	_DATA_LOG_DEF_ new_vrect;
	_DATA_LOG_DEF_ new_iout;
	_DATA_LOG_DEF_ new_ts;
	_DATA_LOG_DEF_ new_rp;

	_DATA_LOG_DEF_ ibat;
	_DATA_LOG_DEF_ ibat_set;
	_DATA_LOG_DEF_ ibus;
	_DATA_LOG_DEF_ iin_ta;
	_DATA_LOG_DEF_ imid;
	_DATA_LOG_DEF_ isys;

	_DATA_LOG_DEF_ vbat;
	_DATA_LOG_DEF_ vbus;
	_DATA_LOG_DEF_ vin_ta;
	_DATA_LOG_DEF_ vmid;
	_DATA_LOG_DEF_ vsys;

	double m_fVrect[1000];
	double m_fIout[1000];
	double m_fVout[1000];
	int	m_nLastIdx;
	double m_fVrect_max;
	double m_fIout_max;
	double m_fVout_max;
	double m_fVrect_min;
	double m_fIout_min;
	double m_fVout_min;

	int m_nLogPt[1000];
	double m_fbuf[4][1000];
	unsigned int	m_status[1000];
	double m_fSec[1000];


	BOOL		m_bBattery;
	double		m_fvout_min;
	double		m_fvout_max;
	double		m_fvout_ripple;
	int			m_fvout_delay;

	double		m_fiout_min;
	double		m_fiout_max;
	double		m_fiout_ripple;
	int			m_fiout_delay;

	double		m_fts_min;
	double		m_fts_max;
	double		m_fts_ripple;
	int			m_fts_delay;
	BOOL		m_bExternalADC;



// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CWireless_RxDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



	BOOL CWireless_RxDoc::data_curve_init(CString sName, CString sUnit, _DATA_LOG_DEF_* curve, COLORREF clr, int nPenStyle, int nPenWidth, double dShowScale, int y_offset);
	//BOOL CWireless_RxDoc::data_curve_init(CString sName, CString sUnit, _DATA_LOG_DEF_* curve, COLORREF clr, int nPenStyle, int nPenWidth);
	BOOL CWireless_RxDoc::check_boundary(int source, int idx, double fVout_diff);

	//void CWireless_RxDoc::update_curve_point(_DATA_LOG_DEF_* curve);
	void CWireless_RxDoc::update_curve_point(_DATA_LOG_DEF_* curve, double input_value, int status);
protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


