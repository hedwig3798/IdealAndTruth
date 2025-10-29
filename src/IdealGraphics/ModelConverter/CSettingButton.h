#pragma once
#include <afxwin.h>

class CSettingButton : 
	public CButton
{
	DECLARE_DYNAMIC(CSettingButton)

private:


public:
	CSettingButton();
	virtual ~CSettingButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClicked();
public:
};

