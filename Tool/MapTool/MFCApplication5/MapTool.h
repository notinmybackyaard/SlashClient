#pragma once
#include "afxwin.h"
#include "Define.h"

// CMapTool 대화 상자입니다.
class CView_0;
class CGameObject;

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

public:
	inline CView_0* GetView() { return m_pView; }

private:
	CCreateContext				m_Context;
	CView_0*					m_pView;


	bool						m_IsReplace = true;
	CGameObject*				m_pCurObject = nullptr;
	CGameObject*				m_pPrevObject = nullptr;
	vector<CGameObject*>		m_vecObject;

	unordered_map<string, vector<CGameObject*>>		m_mapObject;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ModelListBox;
	afx_msg void OnLbnSelchangeListModel();
	afx_msg void OnBnClickedButtonAdd();
	CListBox m_ObjectListBox;
	float m_fPosX;
	afx_msg void OnBnClickedButtonApply();
	float m_fPosY;
	float m_fPosZ;
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	float m_fRotX;
	float m_fRotY;
	float m_fRotZ;
	afx_msg void OnLbnSelchangeListObjects();
	CListBox m_TextureListBox;
	afx_msg void OnLbnSelchangeListTexture();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnNMThemeChangedScrollbarPosx(NMHDR *pNMHDR, LRESULT *pResult);
	CScrollBar m_PosXVSBar;
	afx_msg void OnBnClickedButtonDelete();
	CButton m_RadioDir[3];
	int m_iInstCnt;
	float m_fInstInterval;
	afx_msg void OnBnClickedButtonInstMake();
	afx_msg void OnBnClickedButtonInstPreview();
	CButton m_OneMinusButton;
	CButton m_IsAlphaCheckButton;
};
