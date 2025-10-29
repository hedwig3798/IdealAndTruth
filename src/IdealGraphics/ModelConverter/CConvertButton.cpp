#include "pch.h"
#include "CConvertButton.h"
#include "ModelConverterDlg.h"
#include "stringUtil.h"
#include "Convert.h"

IMPLEMENT_DYNAMIC(CConvertButton, CButton)

BEGIN_MESSAGE_MAP(CConvertButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CConvertButton::OnClicked)
END_MESSAGE_MAP()

CConvertButton::CConvertButton() {}
CConvertButton::~CConvertButton() {}

void CConvertButton::OnClicked()
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

	// 리스트 컨트롤 가져오기
	CFileListContorl& list = pDlg->m_fileListControl;

	int count = list.GetItemCount();
	std::vector<Converter::ConvertJob> jobs;
	for (int i = 0; i < count; i++)
	{
		bool checked = list.GetCheck(i);
		if (true == checked)
		{
			Converter::ConvertJob job;
			std::string fileName = ::WstrToStr(list.GetItemText(i, 4).GetString());
			job.m_path = fileName;
			job.m_isStatic = true;
			job.m_hasBone = false;
			jobs.push_back(job);
		}
	}
	Converter c;
	c.SetOutputPath(pDlg->m_savePath);
	c.SetExtension(".iver");
	c.ReadAssetFile(jobs);

	MessageBox(_T("Convert Complete"), _T("Complete"), MB_OK | MB_ICONINFORMATION);

	EnableWindow(true);
}

