
// QrCode1.0Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "QrCode1.0.h"
#include "QrCode1.0Dlg.h"
#include "afxdialogex.h"

#include <atlimage.h> // 处理图片

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CQrCode10Dlg 对话框



CQrCode10Dlg::CQrCode10Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QRCODE10_DIALOG, pParent)
	, m_dpi(0)
	, m_width(0)
	, m_dist(0)
	, m_radius(0)
	, m_text(_T(""))
	, m_tilt(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQrCode10Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DPI, m_dpi);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_width);
	DDX_Text(pDX, IDC_EDIT_DIST, m_dist);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_radius);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_text);
	DDX_Text(pDX, IDC_EDIT_TILT, m_tilt);
}

//BEGIN_MESSAGE_MAP(CQrCode10Dlg, CDialogEx)
	//ON_WM_SYSCOMMAND()
	//ON_WM_PAINT()
	//ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CQrCode10Dlg::OnBnClickedButton1)
	//ON_BN_CLICKED(IDOK, &CQrCode10Dlg::OnBnClickedOk)
	//ON_BN_CLICKED(IDC_BTN_GENERATE, &CQrCode10Dlg::OnBnClickedBtnGenerate)
//END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CQrCode10Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GENERATE, &CQrCode10Dlg::OnBnClickedBtnGenerate)
	ON_STN_CLICKED(IDC_STATIC_QR, &CQrCode10Dlg::OnStnClickedStaticQr)
END_MESSAGE_MAP()


// CQrCode10Dlg 消息处理程序

BOOL CQrCode10Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQrCode10Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQrCode10Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQrCode10Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQrCode10Dlg::OnBnClickedBtnGenerate()
{
	// 1. 【同步数据】界面 -> 变量
	UpdateData(TRUE);

	// 2. 简单检查
	if (m_text.IsEmpty()) {
		MessageBox(_T("请输入二维码内容！"), _T("提示"), MB_ICONWARNING);
		return;
	}

	// 3. 拼接命令参数
	CString cmdParams;
	// 注意：这里强制转换 (LPCTSTR)m_text 以避免乱码或格式错误
	cmdParams.Format(_T("\"%s\" %.2f %.2f %.2f %.2f %.2f"),
		(LPCTSTR)m_text, m_width, m_radius, m_dpi, m_tilt, m_dist);

	// 4. 获取当前路径
	TCHAR szDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szDir);

	// 5. 运行后台程序
	// 使用 SW_HIDE 隐藏黑框，让软件看起来更专业
	// 如果还在调试阶段，可以改回 SW_SHOW
	HINSTANCE hInst = ShellExecute(NULL, _T("open"), _T("backend.exe"), cmdParams, szDir, SW_HIDE);

	// 6. 检查启动是否成功
	if ((INT_PTR)hInst <= 32) {
		MessageBox(_T("启动失败！\n请确保 backend.exe 和 opencv_worldxxx.dll 都在当前目录下。"), _T("错误"), MB_ICONERROR);
		return;
	}

	// 7. 假装等待生成（给后台一点时间写文件）
	// 这里用一个“完成”弹窗来暂停流程是最好的简单办法
	MessageBox(_T("生成指令已发送！\n点击“确定”加载预览。"), _T("生成完毕"), MB_ICONINFORMATION);

	// 8. 加载图片并显示
	CString imgPath = _T("output.bmp");
	CImage image;

	// 尝试加载图片
	// 为了防止文件被占用，有时需要多试几次，这里简化处理直接加载
	HRESULT hr = image.Load(imgPath);

	if (SUCCEEDED(hr)) {
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_QR);
		if (pPic) {
			pPic->SetBitmap(NULL); // 清空旧图
			pPic->SetBitmap(image.Detach()); // 显示新图
		}
	}
	else {
		MessageBox(_T("预览加载失败。\n可能是生成出错，或者 output.bmp 文件不存在。"), _T("提示"), MB_ICONWARNING);
	}
}

void CQrCode10Dlg::OnStnClickedStaticQr()
{
	// TODO: 在此添加控件通知处理程序代码
}
