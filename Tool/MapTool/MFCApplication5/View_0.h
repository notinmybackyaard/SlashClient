#pragma once


// CView_0 뷰입니다.

#include "BoxApp.h"

class CView_0 : public CView
{
	DECLARE_DYNCREATE(CView_0)

protected:
	CView_0();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CView_0();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	void Loop(const GameTimer& gt);
private:
	InstancingAndCullingApp			m_MainApp;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


