
// ProcessViewerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ProcessViewer.h"
#include "ProcessViewerDlg.h"
#include "afxdialogex.h"

#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"
#include "psapi.h"
#include "winternl.h"
#include "strsafe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CProcessViewerDlg dialog



CProcessViewerDlg::CProcessViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTPROCESS, m_listCtrl);

	m_listCtrl.InsertColumn(0, L"No.", LVCFMT_LEFT, 40);
	m_listCtrl.InsertColumn(1, L"Process ID", LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(2, L"Process Name", LVCFMT_LEFT, 170);
	m_listCtrl.InsertColumn(3, L"Command Line", LVCFMT_LEFT, 160);
	m_listCtrl.InsertColumn(4, L"Full Path", LVCFMT_LEFT, 200);

}

BEGIN_MESSAGE_MAP(CProcessViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_CANCEL, &CProcessViewerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_REFRESH, &CProcessViewerDlg::OnBnClickedRefresh)
END_MESSAGE_MAP()


// CProcessViewerDlg message handlers

BOOL CProcessViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

void CProcessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessViewerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessViewerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CProcessViewerDlg::OnBnClickedRefresh()
{
	// TODO: Add your control notification handler code here
	m_listCtrl.DeleteAllItems();

	GetProcessList();
}


void CProcessViewerDlg::AddData(CListCtrl& ctrl, int row, int col, CString str)
{
	LVITEM lv;
	lv.iItem = row;
	lv.iSubItem = col;
	lv.pszText = (LPWSTR)(LPCTSTR)str;
	lv.mask = LVIF_TEXT;
	if (col == 0)
		ctrl.InsertItem(&lv);
	else
		ctrl.SetItem(&lv);
}



bool CProcessViewerDlg::GetProcessList()
{
	int count = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)){
		CloseHandle(hProcessSnap);         
		return FALSE;
	}
	
	do
	{
		CString id_str;
		id_str.Format(L"%d", count);

		CString pid_str;
		pid_str.Format(L"%d", pe32.th32ProcessID);

		CString fullpath = GetFullPath(pe32.th32ProcessID);
		CString cmdln = GetCommandLineFromPID(pe32.th32ProcessID);

		AddData(m_listCtrl, count, 0, id_str);
		AddData(m_listCtrl, count, 1, pid_str);
		AddData(m_listCtrl, count, 2, pe32.szExeFile);
		AddData(m_listCtrl, count, 3, cmdln);
		AddData(m_listCtrl, count, 4, fullpath);

		count++;

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return TRUE;
}


CString CProcessViewerDlg::GetFullPath(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (!Module32First(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);           // clean the snapshot object
	}
	else {
		do
		{
			if (me32.szExePath != NULL) return me32.szExePath;
		} while (Module32Next(hModuleSnap, &me32));
		CloseHandle(hModuleSnap);
	}


	//try another
	HANDLE processHandle = NULL;
	TCHAR filename[MAX_PATH];

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (processHandle != NULL) {
		if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0) {
			return NULL;
		}
		else {
			return (CString) filename;
		}
		CloseHandle(processHandle);
	}

	return NULL;
}


typedef NTSTATUS(NTAPI* pfnNtQueryInformationProcess)(
	IN  HANDLE ProcessHandle,
	IN  PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN  ULONG ProcessInformationLength,
	OUT PULONG ReturnLength    OPTIONAL
	);


CString CProcessViewerDlg::GetCommandLineFromPID(DWORD dwPID) 
{
	pfnNtQueryInformationProcess gNtQueryInformationProcess;
	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	if (hNtDll == NULL) return NULL;

	gNtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (gNtQueryInformationProcess == NULL) {
		FreeLibrary(hNtDll);
		return NULL;
	}

	auto hHeap = GetProcessHeap();
	_PROCESS_BASIC_INFORMATION     pbi;
	RTL_USER_PROCESS_PARAMETERS upp;
	PEB   peb;
	DWORD len;

	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	gNtQueryInformationProcess(handle, ProcessBasicInformation, &pbi, sizeof(_PROCESS_BASIC_INFORMATION), &len);

	ReadProcessMemory(handle, pbi.PebBaseAddress, &peb, sizeof(PEB), &len);
	ReadProcessMemory(handle, peb.ProcessParameters, &upp, sizeof(RTL_USER_PROCESS_PARAMETERS), &len);

	if (upp.CommandLine.Length > 0) {
		WCHAR* pwszBuffer = (WCHAR*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, upp.CommandLine.Length);
		if (pwszBuffer) {
			ReadProcessMemory(handle, upp.CommandLine.Buffer, pwszBuffer, upp.CommandLine.Length, &len);

			TCHAR cmdln[1000];
			StringCbCopyN(cmdln, sizeof(cmdln), pwszBuffer, upp.CommandLine.Length);

			return (CString)cmdln;
		}
	}

	return NULL;
}
