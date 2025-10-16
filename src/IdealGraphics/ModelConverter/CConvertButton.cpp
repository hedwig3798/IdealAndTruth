#include "pch.h"
#include "CConvertButton.h"
#include "ModelConverterDlg.h"

IMPLEMENT_DYNAMIC(CConvertButton, CButton)

BEGIN_MESSAGE_MAP(CConvertButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CConvertButton::OnClicked)
END_MESSAGE_MAP()

CConvertButton::CConvertButton() {}
CConvertButton::~CConvertButton() {}

void CConvertButton::OnClicked()
{
	// 부모 다이얼로그 가져오기
	CWnd* pParent = GetParent();
	if (!pParent) return;

	// 부모를 실제 타입으로 캐스팅
	CModelConverterDlg* pDlg = dynamic_cast<CModelConverterDlg*>(pParent);
	if (!pDlg) return;

	// 리스트 컨트롤 가져오기
	CFileListContorl& list = pDlg->m_fileListControl;

	int count = list.GetItemCount();

	for (int i = 0; i < count; i++)
	{
		bool checked = list.GetCheck(i);
		if (true == checked)
		{

		}
	}

}

