
// ProcessViewerDlg.h : header file
//

#pragma once


// CProcessViewerDlg dialog
class CProcessViewerDlg : public CDialogEx
{
// Construction
public:
	CProcessViewerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESSVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRefresh();
	CListCtrl m_listCtrl;

	void AddData(CListCtrl& ctrl, int row, int col, CString str);
	bool GetProcessList();
	CString GetFullPath(DWORD dwPID);
	CString GetCommandLineFromPID(DWORD dwPID);

};

/*
typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	PPEB PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;*/

