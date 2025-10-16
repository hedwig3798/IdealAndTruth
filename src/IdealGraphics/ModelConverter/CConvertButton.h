#pragma once
#include <afxwin.h>
#include "AssimpType.h"

class CConvertButton :
    public CButton
{
	DECLARE_DYNAMIC(CConvertButton)

private:


public:
	CConvertButton();
	virtual ~CConvertButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClicked();
public:
};

