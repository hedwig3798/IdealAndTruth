#pragma once
#include <afxwin.h>
#include <string>
#include "Geometry.h"

class CPrimitiveCreateButton :
    public CButton
{
	DECLARE_DYNAMIC(CPrimitiveCreateButton)

private:


public:
	CPrimitiveCreateButton();
	virtual ~CPrimitiveCreateButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClicked();
};

