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
	// �θ� ���̾�α� ��������
	CWnd* pParent = GetParent();
	if (!pParent) return;

	// �θ� ���� Ÿ������ ĳ����
	CModelConverterDlg* pDlg = dynamic_cast<CModelConverterDlg*>(pParent);
	if (!pDlg) return;

	// ����Ʈ ��Ʈ�� ��������
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

