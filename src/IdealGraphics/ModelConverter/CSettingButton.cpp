#include "pch.h"
#include "CSettingButton.h"

#include "pch.h"
#include "CConvertButton.h"
#include "ModelConverterDlg.h"
#include "stringUtil.h"
#include "Convert.h"

IMPLEMENT_DYNAMIC(CSettingButton, CButton)

BEGIN_MESSAGE_MAP(CSettingButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CSettingButton::OnClicked)
END_MESSAGE_MAP()

CSettingButton::CSettingButton() {}
CSettingButton::~CSettingButton() {}

void CSettingButton::OnClicked()
{
	// 부모 다이얼로그 가져오기
	CWnd* pParent = GetParent();
	if (nullptr == pParent)
	{
		return;
	}

	// 부모를 실제 타입으로 캐스팅
	CModelConverterDlg* pDlg = dynamic_cast<CModelConverterDlg*>(pParent);
	if (nullptr == pDlg)
	{
		return;
	}
}

