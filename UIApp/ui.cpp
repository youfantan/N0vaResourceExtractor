#ifndef UNICODE
#define UNICODE
#endif
#define BTN_0 3301
#define BTN_1 3302
#define BTN_2 3303
#define BTN_3 3304
#define BTN_4 3305
#define BTN_5 3306
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"ComCtl32.lib")
#pragma comment (lib,"Gdiplus.lib") 
#pragma comment(lib,"dwmapi.lib")
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "def.h"
#include <corecrt_io.h>
#include <Shlobj.h>
#include <direct.h>
#include <dwmapi.h>
#include <gdiplus.h>
using namespace Gdiplus;

struct ACCENTPOLICY
{
    int na;
    int nf;
    int nc;
    int nA;
};
struct WINCOMPATTRDATA
{
    int na;
    PVOID pd;
    ULONG ul;
};
typedef BOOL(WINAPI* SetWindowCompositionAttributeFunc)(IN HWND hwnd, IN WINCOMPATTRDATA* pwcad);
HWND Parent;
HWND ProgressBar;
std::string currentDir;
ULONG_PTR m_gdiplusToken;
int mode = -1;
wchar_t* param = new wchar_t[MAX_PATH];
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void replace(std::string filePath, int fileType);
void traverse(std::vector<std::string>& vct,const char* path);
int endsWith(const std::string&, const std::string& sub);
int getFileType(const char* filePath);
boolean compareFromHead(unsigned char* toBeCompared, size_t nToBeCompared, const unsigned char* pattern, size_t nPattern);
void doConvert(wchar_t* wFilePath);
DWORD WINAPI doVolumeConvert(LPVOID wDirPath);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    SetProcessDPIAware();
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif
    char cwd[MAX_PATH] = { 0 };
    _getcwd(cwd, MAX_PATH);
    currentDir = cwd;
    std::string outDir = currentDir + "\\out";
    if (0 != _access(outDir.c_str(), 0))
    {
        _mkdir(outDir.c_str());
    }
    const wchar_t CLASS_NAME[]  = L"N0vaDesktopResourceExtractor";
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
            WS_EX_LAYERED,
            CLASS_NAME,
            L"",
            WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT, 460, 250,
            nullptr,
            nullptr,
            hInstance,
            nullptr
    );
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) &~WS_CAPTION);
    SetLayeredWindowAttributes(hwnd, RGB(255,255,254),0,LWA_COLORKEY);
    HMODULE user32 = LoadLibrary(L"user32.dll");
    SetWindowCompositionAttributeFunc SetWindowCompositionAttribute = (SetWindowCompositionAttributeFunc)GetProcAddress(user32, "SetWindowCompositionAttribute");
    ACCENTPOLICY policy = { 3, 0, 0, 0 };
    WINCOMPATTRDATA data = { 19, &policy,sizeof(ACCENTPOLICY) };
    SetWindowCompositionAttribute(hwnd, &data);
    Parent = hwnd;
    if (hwnd == nullptr)
    {
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    Gdiplus::GdiplusStartupInput StartupInput;
    GdiplusStartup(&m_gdiplusToken, &StartupInput, nullptr);
    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            //Draw Components
            InitCommonControls();
            LOGFONT logFont;
            logFont.lfHeight = 24;
            logFont.lfWidth = 0;
            logFont.lfWeight = FW_REGULAR;
            logFont.lfOrientation = 0;
            logFont.lfEscapement = 0;
            logFont.lfStrikeOut = 0;
            logFont.lfUnderline = 0;
            logFont.lfItalic = 0;
            logFont.lfCharSet = GB2312_CHARSET;
            logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
            logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            logFont.lfQuality = DEFAULT_QUALITY;
            logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;
            wcscpy(logFont.lfFaceName, L"Microsoft JhengHei");
            HFONT hFont;
            hFont = CreateFontIndirect(&logFont);
            HWND btn_exit = CreateWindow(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                10, 7, 13, 13, hwnd, (HMENU)BTN_4, (HINSTANCE)hwnd, nullptr);
            HWND btn_min_size = CreateWindow(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                30, 7, 13, 13, hwnd, (HMENU)BTN_5, (HINSTANCE)hwnd, nullptr);
            HWND btn0=CreateWindow(L"Button", L"选择单个文件转换", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 40, 200, 60, hwnd, (HMENU)BTN_0, (HINSTANCE)hwnd, nullptr);
            HWND btn1=CreateWindow(L"Button", L"选择目录批量转换", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                240, 40, 200, 60, hwnd, (HMENU)BTN_1, (HINSTANCE)hwnd, nullptr);
            HWND progress = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE, 20, 130, 420, 30, hwnd, nullptr, nullptr, nullptr);
            ProgressBar = progress;
            HWND help = CreateWindow(L"Button", L"帮助", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 180, 100, 30, hwnd, (HMENU)BTN_2, (HINSTANCE)hwnd, nullptr);
            HWND start = CreateWindow(L"Button", L"开始", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                340, 180, 100, 30, hwnd, (HMENU)BTN_3, (HINSTANCE)hwnd, nullptr);
            SendMessage(btn0, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
            SendMessage(btn1, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
            SendMessage(help, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
            SendMessage(start, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
            return 0;
        }
        
        case WM_PAINT:
        {
            HDC hdc = GetDC(hwnd);
            Graphics graphics(hdc);
            RECT rect;
            GetClientRect(hwnd, &rect);
            Gdiplus::SolidBrush bkBrush(Gdiplus::Color(255, 255, 255, 254));
            Gdiplus::Pen* jadePen = new Pen(Gdiplus::Color(255,231,118,127));
            Gdiplus::SolidBrush jadeBrush(Gdiplus::Color(255,231,118,127));
            Gdiplus::RectF bRect(rect.left, rect.top, rect.right-1, rect.bottom-1);
            Gdiplus::RectF capRect(0, 0, 459, 30);
            graphics.FillRectangle(&bkBrush, bRect);
            graphics.DrawRectangle(jadePen, bRect);
            graphics.FillRectangle(&jadeBrush,capRect);
            DeleteDC(hdc);
            UpdateWindow(hwnd);
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam)) {
                case BTN_0:
                {
                    OPENFILENAME ofn;
                    TCHAR szFile[MAX_PATH] = { 0 };
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = Parent;
                    ofn.lpstrFile = szFile;
                    ofn.lpstrFile[0] = _T('\0');
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = _T("N0vaDesktopFile(*.ndf)\0*.ndf\0\0");
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
                    ofn.lpstrFileTitle = const_cast<wchar_t*>(_T("Open .NDF Files"));
                    BOOL bRet = GetOpenFileName(&ofn);
                    if (szFile[0]==0)
                    {
                        return 0;
                    }
                    mode = 0;
                    wcscpy(param, szFile);
                    break;
                }
                case BTN_1:
                {
                    TCHAR szFile[MAX_PATH] = { 0 };
                    BROWSEINFO bi;
                    ZeroMemory(&bi, sizeof(BROWSEINFO));
                    bi.hwndOwner = Parent;
                    bi.pszDisplayName = szFile;
                    bi.lpszTitle = _T("打开人工桌面的缓存文件目录");
                    bi.ulFlags = BIF_RETURNFSANCESTORS;
                    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
                    if (NULL == idl)
                    {
                        return 0;
                    }
                    SHGetPathFromIDList(idl, szFile);
                    mode = 1;
                    wcscpy(param, szFile);
                    break;
                }
                case BTN_2: 
                {
                    system("start Help.html");
                    break;
                }
                case BTN_3:
                {
                    if (mode == 0) {
                        mode = -1;
                        doConvert(param);
                    }else if(mode == 1){
                        HANDLE h = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)doVolumeConvert, param, 0, 0);
                        mode = -1;
                    }
                    else
                    {
                        MessageBox(Parent, L"请选择一个文件或目录", L"Error", MB_ICONERROR);
                    }
                    break;
                }
                case BTN_4: 
                {
                    Gdiplus::GdiplusShutdown(m_gdiplusToken);
                    PostQuitMessage(0);
                    break;
                }
                case BTN_5:
                {
                    PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                    break;
                }
            }
            return 0;
        }
        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
            int cx = pdis->rcItem.right - pdis->rcItem.bottom;
            int cy = pdis->rcItem.bottom - pdis->rcItem.top;

            switch (pdis->CtlID)
            {
                case BTN_4:
                {
                    HDC hdc = pdis->hDC;
                    Graphics graphics(hdc);
                    graphics.SetSmoothingMode(SmoothingModeHighQuality);
                    Gdiplus::RectF rect(pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right, pdis->rcItem.bottom);
                    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
                    graphics.FillEllipse(&brush, rect);
                    ReleaseDC(hwnd, hdc);
                    break;
                }
                case BTN_5:
                {
                    HDC hdc = pdis->hDC;
                    Graphics graphics(hdc);
                    graphics.SetSmoothingMode(SmoothingModeHighQuality);
                    Gdiplus::RectF rect(pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right, pdis->rcItem.bottom);
                 Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 255, 0));
                 graphics.FillEllipse(&brush, rect);
                 ReleaseDC(hwnd, hdc);
                    break;
                }
                return 0;
            }
        }
        case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (x < 459 && y < 30)
            {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::string WcharToChar(const wchar_t* wp, size_t m_encode = CP_ACP)
{
    std::string str;
    int len = WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
    char* m_char = new char[len + 1];
    WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
    m_char[len] = '\0';
    str = m_char;
    delete[] m_char;
    return str;
}

void doConvert(wchar_t* wFilePath) {
    SendMessage(ProgressBar, PBM_SETRANGE, (WPARAM)0, (LPARAM)(MAKELPARAM(0, 100)));
    SendMessage(ProgressBar, PBM_SETPOS, (WPARAM)(0), (LPARAM)0);
    std::string filePath = WcharToChar(wFilePath, CP_ACP);
    int fType = getFileType(filePath.c_str());
    replace(filePath, fType);
    SendMessage(ProgressBar, PBM_SETPOS, (WPARAM)(100), (LPARAM)0);
    MessageBox(nullptr, L"文件已成功转换", L"完成", MB_OK);
}
DWORD WINAPI doVolumeConvert(LPVOID wDirPath) {
    SendMessage(ProgressBar, PBM_SETRANGE,(WPARAM)0, (LPARAM)(MAKELPARAM(0, 100)));
    SendMessage(ProgressBar, PBM_SETPOS, (WPARAM)(0), (LPARAM)0);
    std::string dirPath = WcharToChar((wchar_t*)wDirPath, CP_ACP);
    std::vector<std::string> vct;
    traverse(vct, dirPath.c_str());
    double xProg;
    int Prog;
    for (size_t i = 0; i < vct.size(); i++)
    {
        xProg =(double) (i+1) / vct.size();
        Prog = (int)(xProg * 100);
        SendMessage(ProgressBar, PBM_SETPOS, (WPARAM)(Prog), (LPARAM)0);
        int fType = getFileType(vct[i].c_str());
        printf("%s \n", vct[i].c_str());
        replace(vct[i], fType);
    }
    MessageBox(nullptr, L"所有文件已成功转换", L"完成", MB_OK);
    return 0;
}
boolean compareFromHead(unsigned char* toBeCompared, size_t nToBeCompared, const unsigned char* pattern, size_t nPattern) {
    for (int i = 0; i < nPattern; ++i) {
        if (*toBeCompared != *pattern) {
            return false;
        }
        *toBeCompared++;
        *pattern++;
    }
    return true;
}
int getFileType(const char* filePath) {
    FILE* file = fopen(filePath, "rb+");
    //get file first 10 bytes
    char* magic = new char[10];
    fread(magic, 1, 10, file);
    if (compareFromHead(reinterpret_cast<unsigned char*>(magic), 10, EXIF_IMAGE_MAGIC, 4)) {
        delete[] magic;
        return EXIF_FILE;
    }
    else if (compareFromHead(reinterpret_cast<unsigned char*>(magic), 10, PNG_IMAGE_MGIC, 8)) {
        delete[] magic;
        return PNG_FILE;
    }
    else if (compareFromHead(reinterpret_cast<unsigned char*>(magic), 10, JPEG_IMAGE_MAGIC, 4)) {
        delete[] magic;
        return JPEG_FILE;
    }
    else if (compareFromHead(reinterpret_cast<unsigned char*>(magic), 10, FTYPMP42_VIDEO_MAGIC, 10) || compareFromHead(reinterpret_cast<unsigned char*>(magic), 10, FTYPISOM_VIDEO_MAGIC, 10)) {
        delete[] magic;
        return FTYP_VIDEO_FILE;
    }
    else {
        delete[] magic;
        return UNRECOGNIZED_FILE;
    }
}
int endsWith(const std::string& s, const std::string& sub) {
    return s.rfind(sub) == (s.length() - sub.length()) ? 1 : 0;
}
std::string getFileName(std::string path) {
    std::string::size_type iPos = (path.find_last_of('\\') + 1) == 0 ? path.find_last_of('/') + 1 : path.find_last_of('\\') + 1;
    return path.substr(iPos, path.length() - iPos);
}
void replace(std::string filePath, int fileType) {
    FILE* file = fopen(filePath.c_str(), "rb+");
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    FILE* modified;
    std::string fn = getFileName(filePath);
    filePath = currentDir;
    filePath = filePath.append("\\out\\").append(fn);
    if (fileType == FTYP_VIDEO_FILE) {
        modified = fopen(filePath.append(".mp4").c_str(), "wb+");
        fseek(file, 2, SEEK_SET);
        len -= 2;
    }
    else if (fileType == EXIF_FILE || fileType == JPEG_FILE) {
        modified = fopen(filePath.append(".jpg").c_str(), "wb+");
    }
    else if (fileType == PNG_FILE) {
        modified = fopen(filePath.append(".png").c_str(), "wb+");
    }
    else {
        printf("Unrecognized File :%s\n", filePath.c_str());
        return;
    }
    char* buffer = new char[len + 1];
    fread(buffer, 1, len, file);
    fwrite(buffer, 1, len, modified);
    delete[] buffer;
    fclose(file);
    fclose(modified);
}
void traverse(std::vector<std::string>& vct,const char* path) {
    _int64 handle;
    _finddata_t fi{};
    char _path[1024] = { 0 };
    memcpy(_path, path, strlen(path));
    strcat(_path, "\\*");
    _int64 ret = handle = _findfirst(_path, &fi);
    printf("%s\n", path);
    if (handle == -1) {
        printf("Error: No files exists in the directory %s", path);
    }
    while (ret != -1) {
        if (strcmp(fi.name, ".") != 0 && strcmp(fi.name, "..") != 0) {
            std::string s(path);
            s.append("\\");
            s.append(fi.name);
            if ((fi.attrib & _A_NORMAL) == 0) {
                vct.push_back(s);
            }
        }
        ret = _findnext(handle, &fi);
    }
    _findclose(handle);
}