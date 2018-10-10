
// MFCApplication5.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCApplication5.h"
#include "MainFrm.h"

#include "MFCApplication5Doc.h"
#include "MFCApplication5View.h"
#include "View_0.h"


#include "GameTimer_Manager.h"
#include "Frame_Manager.h"
#include "GameTimer.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication5App

BEGIN_MESSAGE_MAP(CMFCApplication5App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMFCApplication5App::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CMFCApplication5App 생성

CMFCApplication5App::CMFCApplication5App()
{
	m_bHiColorIcons = TRUE;

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MFCApplication5.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CMFCApplication5App 개체입니다.

CMFCApplication5App theApp;


// CMFCApplication5App 초기화

BOOL CMFCApplication5App::InitInstance()
{
	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCApplication5Doc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CMFCApplication5View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	RECT m_Rect = { 0, 0, 600, 600 };
	AdjustWindowRect(&m_Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width = m_Rect.right - m_Rect.left;
	int height = m_Rect.bottom - m_Rect.top;
	m_pMainWnd->SetWindowPos(NULL, 300, 100, width, height, 0);

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->SetWindowTextW(L"Map Tool");

	CGameTimer_Manager::GetInstance()->Ready_Timers(L"Time_Default");
	CGameTimer_Manager::GetInstance()->Ready_Timers(L"Timer_FPS");

	CFrame_Manager::GetInstance()->Ready_Frames(L"Frame 30", 30.f);
	CFrame_Manager::GetInstance()->Ready_Frames(L"Frame 60", 60.f);
	CFrame_Manager::GetInstance()->Ready_Frames(L"Frame 100", 100.f);
	CFrame_Manager::GetInstance()->Ready_Frames(L"Frame 3000", 3000.f);

	const wchar_t* wstrFrame[FPS_END] = { L"Frame 30", L"Frame 60", L"Frame 100" , L"Frame 3000" };

	//mTimer.Reset();
	CGameTimer_Manager::GetInstance()->Get_GameTimer(L"Timer_FPS")->Reset();
	CGameTimer_Manager::GetInstance()->Get_GameTimer(L"Time_Default")->Reset();

	return TRUE;
}

// CMFCApplication5App 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMFCApplication5App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCApplication5App 사용자 지정 로드/저장 메서드

void CMFCApplication5App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CMFCApplication5App::LoadCustomState()
{
}

void CMFCApplication5App::SaveCustomState()
{
}

// CMFCApplication5App 메시지 처리기





BOOL CMFCApplication5App::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (this->m_pMainWnd->IsIconic())
		return FALSE;
	else
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame != NULL)
		{
			CMyForm* pMainForm = pMainFrame->GetMainForm();
			if (pMainForm != NULL)
			{
				CView_0* pView = pMainForm->Get_View();//->GetView();
				if (pView != NULL)
				{
					GameTimer gt;
					if (pView->IsWindowVisible())
					{
						GameTimer pFPSTimer = *CGameTimer_Manager::GetInstance()->Get_GameTimer(L"Timer_FPS");
						pView->Loop(pFPSTimer);

						//CGameTimer_Manager::GetInstance()->Compute_TimeDelta(L"Time_Default");
						//wchar_t* wstrFrame[FPS_END] = { L"Frame 30", L"Frame 60", L"Frame 100" , L"Frame 3000" };
						//GameTimer pTimer = *(CGameTimer_Manager::GetInstance()->Get_GameTimer(L"Time_Default"));
						//if (CFrame_Manager::GetInstance()->Permit_Call(wstrFrame[1], const_cast<GameTimer&>(pTimer)))
						//{
						//	CGameTimer_Manager::GetInstance()->Compute_TimeDelta(L"Timer_FPS");
						//	GameTimer pFPSTimer = *CGameTimer_Manager::GetInstance()->Get_GameTimer(L"Timer_FPS");
						//	//CalculateFrameStats();
						//	pView->Loop(pFPSTimer);
						//}

						
					}
				}
			}
		}
	}

	return CWinAppEx::OnIdle(lCount);
}
