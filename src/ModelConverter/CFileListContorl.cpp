#include "pch.h"
#include "CFileListContorl.h"
#include <string>
#include <iostream>
#include <cmath>

IMPLEMENT_DYNAMIC(CFileListContorl, CListCtrl)

BEGIN_MESSAGE_MAP(CFileListContorl, CListCtrl)
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CFileListContorl::OnHeaderClick)
END_MESSAGE_MAP()

int CFileListContorl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

void CFileListContorl::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNM = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int col = pNM->iSubItem; 

	if (col == 0) 
	{
		static BOOL allChecked = FALSE;
		allChecked = !allChecked;

		for (int i = 0; i < GetItemCount(); ++i)
		{
			SetCheck(i, allChecked);
		}
	}

	*pResult = 0;
}

void CFileListContorl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	SetExtendedStyle(
		GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES
	);

	InsertColumn(0, _T("선택"), LVCFMT_CENTER, 40);
	InsertColumn(1, _T("파일명"), LVCFMT_LEFT, 100);
	InsertColumn(2, _T("크기"), LVCFMT_LEFT, 100);
	InsertColumn(3, _T("변환 여부"), LVCFMT_LEFT, 100);
	InsertColumn(4, _T("파일 경로"), LVCFMT_LEFT, 100);
	DragAcceptFiles(TRUE);
}

void CFileListContorl::OnDropFiles(HDROP hDropInfo)
{
	UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

	for (UINT i = 0; i < fileCount; ++i)
	{
		TCHAR path[MAX_PATH];
		DragQueryFile(hDropInfo, i, path, MAX_PATH);

		CString fullPath = path;
		CString fileName = fullPath.Mid(fullPath.ReverseFind('\\') + 1);


		int index = InsertItem(i, fullPath);
		SetItemText(index, 0, _T(""));
		SetItemText(index, 1, fileName);
		CFile file;
		if (file.Open(path, CFile::modeRead))
		{
			ULONGLONG size = file.GetLength();
			file.Close();

			long long siezKB = static_cast<long long>(std::ceil(static_cast<float>(size) / 1024.f));

			CString msg;
			msg.Format(_T("%llu KB"), siezKB);
			SetItemText(index, 2, msg);
		}
		SetItemText(index, 3, _T("대기"));
		SetItemText(index, 4, path);
	}

	DragFinish(hDropInfo);
}
