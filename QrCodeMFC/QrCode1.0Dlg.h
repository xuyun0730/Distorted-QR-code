
// QrCode1.0Dlg.h: 头文件
//

#pragma once


// CQrCode10Dlg 对话框
class CQrCode10Dlg : public CDialogEx
{
// 构造
public:
	CQrCode10Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QRCODE10_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	double m_dpi;
	double m_width;
	double m_dist;
	double m_radius;
	CString m_text;
	double m_tilt;
	afx_msg void OnBnClickedBtnGenerate();
	afx_msg void OnStnClickedStaticQr();
};
