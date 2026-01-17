/* #include <iostream>
#include <string>
#include <limits>
#include <vector>     // 新增：虽然这里用数组也行，但 vector 习惯更好
#include <stdexcept>  // 新增：为了处理转换错误
#include "ImageUtils.h"
#include "Distortion.h"
#include "qrcodegen.hpp"

using namespace std;
using namespace qrcodegen;

// 配置结构体
struct Config {
    string text = "https://www.example.com";
    double widthMM = 20.0;
    double radiusMM = 15.0;
    double dpi = 600.0;
    double tiltAngle = 30.0;
    double distanceMM = 150.0;
};

// 工具函数：获取用户输入
template <typename T>
T getInput(const string& prompt, T defaultValue) {
    cout << prompt << " [" << defaultValue << "]: ";
    string input;
    getline(cin, input);
    if (input.empty()) return defaultValue;
    try {
        if constexpr (is_same_v<T, string>) return input;
        else if constexpr (is_same_v<T, double>) return stod(input);
        else if constexpr (is_same_v<T, int>) return stoi(input);
    } catch (...) {}
    return defaultValue;
}

void showMenu(const Config& cfg) {
    cout << "\n========================================\n";
    cout << "   圆柱二维码畸变生成器 v1.0\n";
    cout << "========================================\n";
    cout << "当前配置:\n";
    cout << "  1. 内容: " << cfg.text << "\n";
    cout << "  2. 目标宽度: " << cfg.widthMM << " mm\n";
    cout << "  3. 纸筒半径: " << cfg.radiusMM << " mm\n";
    cout << "  4. 打印 DPI: " << cfg.dpi << "\n";
    cout << "  5. 倾斜角度: " << cfg.tiltAngle << " 度\n";
    cout << "  6. 视距:     " << cfg.distanceMM << " mm\n";
    cout << "----------------------------------------\n";
    cout << "  G. 生成文件 (Generate)\n";
    cout << "  Q. 退出 (Quit)\n";
    cout << "========================================\n";
    cout << "请选择要修改的项 (1-6) 或操作 (G/Q): ";
}

void runGeneration(const Config& cfg) {
    cout << "\n>>> 开始处理...\n";

    double pxPerMM = cfg.dpi / 25.4;
    double radiusPx = cfg.radiusMM * pxPerMM;
    double distPx = cfg.distanceMM * pxPerMM;
    int targetPxWidth = (int)(cfg.widthMM * pxPerMM);

    // 1. 生成QR
    QrCode qr = QrCode::encodeText(cfg.text.c_str(), QrCode::Ecc::MEDIUM);
    int moduleScale = targetPxWidth / (qr.getSize() + 4);
    if (moduleScale < 8) moduleScale = 8;

    cout << ">>> 渲染基础二维码 (Scale: " << moduleScale << ")...\n";
    ImageGray stdImg = renderQrToImage(qr, moduleScale, 2);

    // 2. 畸变计算
    cout << ">>> 计算 3D 复合畸变...\n";
    ImageGray outImg = generateCompositeDistortion(stdImg, radiusPx, cfg.tiltAngle, distPx);

    if (outImg.data.empty()) {
        cout << "!!! 错误: 生成失败 (可能半径过小)\n";
        return;
    }

    // 3. 保存
    string bmpName = "output.bmp";
    string svgName = "output.svg";
    saveBMP(bmpName, outImg);
    saveSVG(svgName, outImg, 128);

    cout << ">>> 完成! 文件已保存:\n";
    cout << "    - " << bmpName << " (位图预览)\n";
    cout << "    - " << svgName << " (矢量打标)\n";
}

// ==========================================
//  核心修改区域：支持命令行参数
// ==========================================
int main(int argc, char* argv[]) {
    

    // 【新增 1】让它一上来就打印收到了什么参数
    std::cout << "=== 调试模式 ===" << std::endl;
    std::cout << "收到的参数数量: " << argc << std::endl;
    for(int i = 0; i < argc; i++) {
        std::cout << "参数 [" << i << "]: " << argv[i] << std::endl;
    }
    std::cout << "=================" << std::endl;

    system("pause");

    Config cfg;

    // --- 模式 A：静默命令行模式 (MFC调用) ---
    // 如果参数大于1，说明有外部命令传入
    if (argc > 1) {
        try {
            // 参数顺序约定:
            // exe路径 [1]内容 [2]宽度 [3]半径 [4]DPI [5]倾斜角 [6]视距
            
            if (argc >= 2) cfg.text = argv[1];
            
            // 使用 std::stod 将字符串转换为 double
            // 我们加了判断，防止参数没传够导致崩溃
            if (argc >= 3) cfg.widthMM = std::stod(argv[2]);
            if (argc >= 4) cfg.radiusMM = std::stod(argv[3]);
            if (argc >= 5) cfg.dpi = std::stod(argv[4]);
            if (argc >= 6) cfg.tiltAngle = std::stod(argv[5]);
            if (argc >= 7) cfg.distanceMM = std::stod(argv[6]);

            // 直接生成，不显示菜单
            runGeneration(cfg);
            
            return 0; // 完成任务直接退出
        }
        catch (const std::exception& e) {
            cerr << "参数解析错误: " << e.what() << endl;
            return 1; // 返回错误代码
        }
    }

    // --- 模式 B：原来的交互菜单模式 (直接双击运行) ---
    string choice;
    while (true) {
        showMenu(cfg);
        getline(cin, choice);

        if (choice == "1") cfg.text = getInput("请输入新内容", cfg.text);
        else if (choice == "2") cfg.widthMM = getInput("请输入宽度(mm)", cfg.widthMM);
        else if (choice == "3") cfg.radiusMM = getInput("请输入半径(mm)", cfg.radiusMM);
        else if (choice == "4") cfg.dpi = getInput("请输入DPI", cfg.dpi);
        else if (choice == "5") cfg.tiltAngle = getInput("请输入倾斜角(度)", cfg.tiltAngle);
        else if (choice == "6") cfg.distanceMM = getInput("请输入视距(mm)", cfg.distanceMM);
        else if (choice == "g" || choice == "G") {
            runGeneration(cfg);
            cout << "\n按回车键继续...";
            cin.get();
        }
        else if (choice == "q" || choice == "Q") break;
    }

    std::cout << "程序运行结束。请检查上方是否有报错。" << std::endl;
    system("pause"); // <--- 关键！让黑框停住

    return 0;
} */


/* #include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <stdexcept> // 处理 stod 转换错误
#include <algorithm> // 算法支持

// 引入你的自定义头文件
#include "ImageUtils.h"
#include "Distortion.h"
#include "qrcodegen.hpp"

using namespace std;
using namespace qrcodegen;

// ==========================================
// 1. 配置结构体
// ==========================================
struct Config {
    string text = "https://www.example.com";
    double widthMM = 20.0;
    double radiusMM = 15.0;
    double dpi = 600.0;
    double tiltAngle = 30.0;
    double distanceMM = 150.0;
};

// ==========================================
// 2. 辅助工具函数
// ==========================================
// 获取用户输入
template <typename T>
T getInput(const string& prompt, T defaultValue) {
    cout << prompt << " [" << defaultValue << "]: ";
    string input;
    getline(cin, input);
    if (input.empty()) return defaultValue;
    try {
        if constexpr (is_same_v<T, string>) return input;
        else if constexpr (is_same_v<T, double>) return stod(input);
        else if constexpr (is_same_v<T, int>) return stoi(input);
    } catch (...) {}
    return defaultValue;
}

// 显示菜单
void showMenu(const Config& cfg) {
    cout << "\n========================================\n";
    cout << "   圆柱二维码畸变生成器 v1.0\n";
    cout << "========================================\n";
    cout << "当前配置:\n";
    cout << "  1. 内容: " << cfg.text << "\n";
    cout << "  2. 目标宽度: " << cfg.widthMM << " mm\n";
    cout << "  3. 纸筒半径: " << cfg.radiusMM << " mm\n";
    cout << "  4. 打印 DPI: " << cfg.dpi << "\n";
    cout << "  5. 倾斜角度: " << cfg.tiltAngle << " 度\n";
    cout << "  6. 视距:     " << cfg.distanceMM << " mm\n";
    cout << "----------------------------------------\n";
    cout << "  G. 生成文件 (Generate)\n";
    cout << "  Q. 退出 (Quit)\n";
    cout << "========================================\n";
    cout << "请选择要修改的项 (1-6) 或操作 (G/Q): ";
}

// ==========================================
// 3. 核心生成逻辑
// ==========================================
void runGeneration(const Config& cfg) {
    cout << "\n>>> 开始处理...\n";

    double pxPerMM = cfg.dpi / 25.4;
    double radiusPx = cfg.radiusMM * pxPerMM;
    double distPx = cfg.distanceMM * pxPerMM;
    int targetPxWidth = (int)(cfg.widthMM * pxPerMM);

    // 1. 生成QR
    // 注意：这里使用的是 qrcodegen 库
    QrCode qr = QrCode::encodeText(cfg.text.c_str(), QrCode::Ecc::MEDIUM);
    
    // 计算缩放比例
    int moduleScale = targetPxWidth / (qr.getSize() + 4);
    if (moduleScale < 8) moduleScale = 8; // 保证最小清晰度

    cout << ">>> 渲染基础二维码 (Scale: " << moduleScale << ")...\n";
    // 假设 renderQrToImage 是在 ImageUtils.h 中定义的
    ImageGray stdImg = renderQrToImage(qr, moduleScale, 2);

    // 2. 畸变计算
    cout << ">>> 计算 3D 复合畸变...\n";
    // 假设 generateCompositeDistortion 是在 Distortion.h 中定义的
    ImageGray outImg = generateCompositeDistortion(stdImg, radiusPx, cfg.tiltAngle, distPx);

    if (outImg.data.empty()) {
        cout << "!!! 错误: 生成失败 (可能半径过小或参数错误)\n";
        return;
    }

    // 3. 保存文件
    string bmpName = "output.bmp";
    string svgName = "output.svg";
    
    // 假设这些保存函数在 ImageUtils.h 中
    saveBMP(bmpName, outImg);
    saveSVG(svgName, outImg, 128);

    cout << ">>> 完成! 文件已保存:\n";
    cout << "    - " << bmpName << " (位图预览)\n";
    cout << "    - " << svgName << " (矢量打标)\n";
}

// ==========================================
// 4. 主函数 (入口)
// ==========================================
int main(int argc, char* argv[]) {
    
    // ---------------------------------------------------------
    // 【调试区域】
    // 这一段是为了让你在黑框里看清楚 MFC 传了什么过来
    // 等以后软件发布时，可以把这几行删掉
    // ---------------------------------------------------------
    std::cout << "=== 调试模式 ===" << std::endl;
    std::cout << "收到的参数数量 (argc): " << argc << std::endl;
    for(int i = 0; i < argc; i++) {
        std::cout << "参数 [" << i << "]: " << argv[i] << std::endl;
    }
    std::cout << "=================" << std::endl;

    // 【注意】如果你希望 MFC 点击生成后不需要你手动按空格，
    // 请在调试完成后注释掉下面这行 system("pause");
    system("pause"); 
    // ---------------------------------------------------------


    Config cfg;

    // ==========================================
    // 模式 A：MFC 自动调用模式 (命令行模式)
    // ==========================================
    // 判定条件：参数大于1，说明有外部数据传入
    if (argc > 1) {
        try {
            // 参数顺序约定:
            // argv[0]: exe自身路径
            // argv[1]: 内容 (text)
            // argv[2]: 宽度 (width)
            // argv[3]: 半径 (radius)
            // argv[4]: DPI
            // argv[5]: 倾斜角 (tilt)
            // argv[6]: 视距 (dist)
            
            if (argc >= 2) cfg.text = argv[1];
            
            // 使用 std::stod 将字符串转换为 double
            // 加上简单的越界检查，防止参数缺失导致崩溃
            if (argc >= 3) cfg.widthMM = std::stod(argv[2]);
            if (argc >= 4) cfg.radiusMM = std::stod(argv[3]);
            if (argc >= 5) cfg.dpi = std::stod(argv[4]);
            if (argc >= 6) cfg.tiltAngle = std::stod(argv[5]);
            if (argc >= 7) cfg.distanceMM = std::stod(argv[6]);

            // 执行生成逻辑
            runGeneration(cfg);
            
            // 【关键】自动模式下，跑完就直接退出，不要显示菜单
            return 0; 
        }
        catch (const std::exception& e) {
            cerr << "参数解析错误: " << e.what() << endl;
            // 如果出错，暂停一下让用户看到错误
            system("pause"); 
            return 1; 
        }
    }

    // ==========================================
    // 模式 B：人工交互模式 (双击运行)
    // ==========================================
    // 只有当 argc == 1 时才会走到这里
    
    string choice;
    while (true) {
        showMenu(cfg);
        getline(cin, choice);

        if (choice == "1") cfg.text = getInput("请输入新内容", cfg.text);
        else if (choice == "2") cfg.widthMM = getInput("请输入宽度(mm)", cfg.widthMM);
        else if (choice == "3") cfg.radiusMM = getInput("请输入半径(mm)", cfg.radiusMM);
        else if (choice == "4") cfg.dpi = getInput("请输入DPI", cfg.dpi);
        else if (choice == "5") cfg.tiltAngle = getInput("请输入倾斜角(度)", cfg.tiltAngle);
        else if (choice == "6") cfg.distanceMM = getInput("请输入视距(mm)", cfg.distanceMM);
        else if (choice == "g" || choice == "G") {
            runGeneration(cfg);
            cout << "\n按回车键继续...";
            cin.get();
        }
        else if (choice == "q" || choice == "Q") {
            break;
        }
    }

    std::cout << "程序即将退出..." << std::endl;
    return 0;
} */


#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iomanip> // 用于控制输出精度

// Windows下控制台中文乱码修复
#ifdef _WIN32
#include <windows.h>
#endif

// 引入你的自定义头文件 (保持不变)
#include "ImageUtils.h"
#include "Distortion.h"
#include "qrcodegen.hpp"

using namespace std;
using namespace qrcodegen;

// ==========================================
// 1. 配置结构体 (增加输出文件名配置)
// ==========================================
struct Config {
    string text = "https://www.example.com";
    double widthMM = 20.0;
    double radiusMM = 15.0;
    double dpi = 600.0;
    double tiltAngle = 30.0;
    double distanceMM = 150.0;
    string outputBaseName = "output"; // 新增：输出文件基名（不带后缀）
};

// ==========================================
// 2. 辅助工具函数
// ==========================================

// 设置控制台编码为 UTF-8，防止中文乱码
void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    // 某些旧控制台可能还需要设置字体，这里暂略，通常 CP_UTF8 足够
#endif
    // 提高 float 输出精度
    cout << fixed << setprecision(2);
}

// 通用输入函数
template <typename T>
T getInput(const string& prompt, T defaultValue) {
    cout << prompt << " [" << defaultValue << "]: ";
    string input;
    getline(cin, input);
    if (input.empty()) return defaultValue;
    try {
        if constexpr (is_same_v<T, string>) return input;
        else if constexpr (is_same_v<T, double>) return stod(input);
        else if constexpr (is_same_v<T, int>) return stoi(input);
    } catch (...) {}
    return defaultValue;
}

// 解析命令行参数
bool parseArguments(int argc, char* argv[], Config& cfg) {
    try {
        if (argc >= 2) cfg.text = argv[1];
        if (argc >= 3) cfg.widthMM = stod(argv[2]);
        if (argc >= 4) cfg.radiusMM = stod(argv[3]);
        if (argc >= 5) cfg.dpi = stod(argv[4]);
        if (argc >= 6) cfg.tiltAngle = stod(argv[5]);
        if (argc >= 7) cfg.distanceMM = stod(argv[6]);
        // 新增：支持自定义输出文件名 (例如: "C:/Data/qr001")
        if (argc >= 8) cfg.outputBaseName = argv[7]; 
        return true;
    } catch (const exception& e) {
        cerr << "!!! 参数解析异常: " << e.what() << endl;
        return false;
    }
}

void showMenu(const Config& cfg) {
    cout << "\n========================================\n";
    cout << "   圆柱二维码畸变生成器 v1.2 (Pro)\n";
    cout << "========================================\n";
    cout << "当前配置:\n";
    cout << "  1. 内容: " << cfg.text << "\n";
    cout << "  2. 宽度: " << cfg.widthMM << " mm\n";
    cout << "  3. 半径: " << cfg.radiusMM << " mm\n";
    cout << "  4. DPI:  " << cfg.dpi << "\n";
    cout << "  5. 倾斜: " << cfg.tiltAngle << " 度\n";
    cout << "  6. 视距: " << cfg.distanceMM << " mm\n";
    cout << "  7. 文件名: " << cfg.outputBaseName << ".*\n";
    cout << "----------------------------------------\n";
    cout << "  G. 生成 (Generate)\n";
    cout << "  Q. 退出 (Quit)\n";
    cout << "========================================\n";
    cout << "选择操作: ";
}

// ==========================================
// 3. 核心生成逻辑
// ==========================================
bool runGeneration(const Config& cfg) {
    cout << "\n>>> [Step 1] 初始化参数...\n";

    double pxPerMM = cfg.dpi / 25.4;
    double radiusPx = cfg.radiusMM * pxPerMM;
    double distPx = cfg.distanceMM * pxPerMM;
    int targetPxWidth = (int)(cfg.widthMM * pxPerMM);

    // 1. 生成 QR Data
    QrCode qr = QrCode::encodeText(cfg.text.c_str(), QrCode::Ecc::MEDIUM);
    int qrRawSize = qr.getSize();
    int padding = 2; // 白边模块数 (双侧共4)
    int totalModules = qrRawSize + (padding * 2);

    // 计算缩放比例 (整数运算)
    int moduleScale = targetPxWidth / totalModules;
    if (moduleScale < 4) {
        cout << "!!! 警告: DPI过低或尺寸太小，二维码可能无法识别 (Scale=" << moduleScale << ")\n";
        moduleScale = 4; // 强制最小倍率
    }

    // 计算实际生成的物理尺寸 (用于反馈给用户)
    double actualWidthMM = (moduleScale * totalModules) / pxPerMM;
    double errorMM = actualWidthMM - cfg.widthMM;

    cout << ">>> [Step 2] 渲染基础二维码...\n";
    cout << "    - 目标宽度: " << cfg.widthMM << " mm\n";
    cout << "    - 实际宽度: " << actualWidthMM << " mm (误差: " << errorMM << " mm)\n";
    cout << "    - 模块尺寸: " << moduleScale << " px/block\n";

    ImageGray stdImg = renderQrToImage(qr, moduleScale, padding);

    // 2. 畸变计算
    cout << ">>> [Step 3] 应用 3D 柱面畸变...\n";
    ImageGray outImg = generateCompositeDistortion(stdImg, radiusPx, cfg.tiltAngle, distPx);

    if (outImg.data.empty()) {
        cerr << "!!! 错误: 畸变生成失败 (图像为空)\n";
        return false;
    }

    // 3. 保存文件
    string bmpName = cfg.outputBaseName + ".bmp";
    string svgName = cfg.outputBaseName + ".svg";

    cout << ">>> [Step 4] 保存文件...\n";
    if (!saveBMP(bmpName, outImg)) {
        cerr << "!!! 错误: BMP 保存失败 (路径不存在或权限不足)\n";
        return false;
    }
    saveSVG(svgName, outImg, 128); // 阈值128

    cout << ">>> 成功! 输出文件:\n";
    cout << "    [BMP] " << bmpName << "\n";
    cout << "    [SVG] " << svgName << "\n";
    
    return true;
}

// ==========================================
// 4. 主函数
// ==========================================
int main(int argc, char* argv[]) {
    initConsole(); // 修复中文乱码
    Config cfg;

    // --- 模式 A: 自动模式 (命令行调用) ---
    if (argc > 1) {
        // 调试信息：正式发布时可保留，用于日志追踪
        cout << "[Auto Mode] 参数个数: " << argc << endl;

        if (!parseArguments(argc, argv, cfg)) {
            return 1; // 参数解析失败返回非0
        }

        bool success = runGeneration(cfg);

        if (success) {
            // 成功时不暂停，直接退出，方便 MFC 快速响应
            return 0; 
        } else {
            // 失败时暂停，让开发者能看到报错信息
            system("pause");
            return 2; // 业务逻辑失败返回非0
        }
    }

    // --- 模式 B: 交互模式 (双击运行) ---
    string choice;
    while (true) {
        showMenu(cfg);
        getline(cin, choice);

        if (choice == "1") cfg.text = getInput("内容", cfg.text);
        else if (choice == "2") cfg.widthMM = getInput("宽度(mm)", cfg.widthMM);
        else if (choice == "3") cfg.radiusMM = getInput("半径(mm)", cfg.radiusMM);
        else if (choice == "4") cfg.dpi = getInput("DPI", cfg.dpi);
        else if (choice == "5") cfg.tiltAngle = getInput("倾斜角", cfg.tiltAngle);
        else if (choice == "6") cfg.distanceMM = getInput("视距(mm)", cfg.distanceMM);
        else if (choice == "7") cfg.outputBaseName = getInput("输出文件名(无后缀)", cfg.outputBaseName);
        else if (choice == "g" || choice == "G") {
            runGeneration(cfg);
            cout << "\n按回车键继续...";
            cin.get();
        }
        else if (choice == "q" || choice == "Q") {
            break;
        }
    }

    return 0;
}