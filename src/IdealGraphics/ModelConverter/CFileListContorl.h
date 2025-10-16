#pragma once
#include <afxcmn.h>
class CFileListContorl :
    public CListCtrl
{
	DECLARE_DYNAMIC(CFileListContorl)

public:
	CFileListContorl() {}
	virtual ~CFileListContorl() {}

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
};

