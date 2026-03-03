#include "pch.h"
#include "ModelConverterDlg.h"
#include "CPrimitiveCreateButton.h"

IMPLEMENT_DYNAMIC(CPrimitiveCreateButton, CButton)

BEGIN_MESSAGE_MAP(CPrimitiveCreateButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CPrimitiveCreateButton::OnClicked)
END_MESSAGE_MAP()

CPrimitiveCreateButton::CPrimitiveCreateButton()
{

}

CPrimitiveCreateButton::~CPrimitiveCreateButton()
{

}

void CPrimitiveCreateButton::OnClicked()
{
	EnableWindow(false);

	// 부모 다이얼로그 가져오기
	CWnd* pParent = GetParent();
	if (nullptr == pParent)
	{
		EnableWindow(true);
		return;
	}

	// 부모를 실제 타입으로 캐스팅
	CModelConverterDlg* pDlg = dynamic_cast<CModelConverterDlg*>(pParent);
	if (nullptr == pDlg)
	{
		EnableWindow(true);
		return;
	}

	std::vector<VertexPUNData> vertexs;
	std::vector<unsigned int> indeies;
	PrimitiveGeomatry::CreateSphere(vertexs, indeies, false);

	MessageBox(_T("Convert Complete"), _T("Complete"), MB_OK | MB_ICONINFORMATION);

	EnableWindow(true);
}
