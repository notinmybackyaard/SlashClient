// View_0.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCApplication5.h"
#include "View_0.h"

;
// CView_0

IMPLEMENT_DYNCREATE(CView_0, CView)

CView_0::CView_0()
	: m_MainApp(g_hInst)
{

}

CView_0::~CView_0()
{
}

BEGIN_MESSAGE_MAP(CView_0, CView)
END_MESSAGE_MAP()


// CView_0 그리기입니다.

void CView_0::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CView_0 진단입니다.

#ifdef _DEBUG
void CView_0::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView_0::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


void CView_0::Loop(const GameTimer & gt)
{
	m_MainApp.Run(gt);
	/*m_MainApp.Update(gt);
	m_MainApp.Draw(gt);*/
}

// CView_0 메시지 처리기입니다.


void CView_0::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_MainApp.Initialize();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


//BOOL CView_0::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
////	cs.x = 400;
//	cs.cx = 800;
//	cs.cy = 600;
//	return CView::PreCreateWindow(cs);
//}


void CView_0::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	int i = 0;
}
