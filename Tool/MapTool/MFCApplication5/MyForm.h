#pragma once



#include "afxwin.h"
#include "MapTool.h"
#include "EffectTool.h"

// CMyForm 폼 뷰입니다.

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	enum TOOLTYPE {TOOL_MAP, TOOL_EFFECT};

public:
	//CMapTool* Get_Dialog();
	CView_0* Get_View();// { if (m_eCurTool == TOOL_MAP) m_MapTool->Get_View(); }
	//inline CDialog* Get_Dialog() { return &m_CurTool; }

private:
	CMapTool			m_MapTool;
	CEffectTool			m_EffectTool;
	TOOLTYPE			m_eCurTool;
	

	void FindPath(const std::wstring& wstrPath	, const std::wstring& wstrExt);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedMaptool();
	CListBox m_LoadObjectListBox;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonEffectTool();
	CListBox m_LoadTextureListBox;
	afx_msg void OnLbnSelchangeListLoadTex();
	CButton m_ToolRadio[2];
};


