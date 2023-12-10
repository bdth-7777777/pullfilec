#include <windows.h>
#include <stdio.h>
#include <string.h>

// 全局变量
HWND hList; // 列表框句柄
HWND hButtonClear, hButtonCancel, hButtonConfirm; // 按钮句柄
char txtFilePath[MAX_PATH] = "filepath.txt"; // 默认文本文件路径

// 窗口类名和标题
const char g_szClassName[] = "myWindowClass";
const char g_szWindowTitle[] = "将文件拖入此窗口";

// 窗口过程函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 解析命令行参数
void ParseCommandLine(LPSTR lpCmdLine) {
    // 解析命令行参数，格式为 "-o [txt文件路径]"
    char* token = strtok(lpCmdLine, " ");
    while (token != NULL) {
        if (strcmp(token, "-o") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                strncpy(txtFilePath, token, MAX_PATH);
            }
            break;
        }
        token = strtok(NULL, " ");
    }
}

// WinMain函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // 解析命令行参数
    ParseCommandLine(lpCmdLine);

    // 注册窗口类
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "窗口注册失败！", "错误",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建窗口
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        g_szWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "窗口创建失败！", "错误",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 创建列表框和按钮
    hList = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
        10, 10, 460, 200, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

    hButtonClear = CreateWindow("BUTTON", "重新选择",
        WS_VISIBLE | WS_CHILD, 10, 220, 100, 30, hwnd, (HMENU)2, NULL, NULL);
    hButtonCancel = CreateWindow("BUTTON", "取消",
        WS_VISIBLE | WS_CHILD, 120, 220, 100, 30, hwnd, (HMENU)3, NULL, NULL);
    hButtonConfirm = CreateWindow("BUTTON", "确认",
        WS_VISIBLE | WS_CHILD, 230, 220, 100, 30, hwnd, (HMENU)4, NULL, NULL);

    // 启用拖放功能
    DragAcceptFiles(hwnd, TRUE);

    // 消息循环
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            // 已在WinMain中创建控件
            break;

        case WM_DROPFILES:
        {
            HDROP hDrop = (HDROP)wParam;
            UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
            char szFileName[MAX_PATH];
            DWORD dwFileAttributes;

            for(UINT i = 0; i < nFiles; i++)
            {
                DragQueryFile(hDrop, i, szFileName, MAX_PATH);
                dwFileAttributes = GetFileAttributes(szFileName);

                // 检查是否为文件夹
                if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    MessageBox(hwnd, "无法拖入文件夹。", "错误", MB_ICONERROR);
                    continue; // 跳过文件夹
                }

                // 检查文件是否已经在列表中
                if (SendMessage(hList, LB_FINDSTRINGEXACT, -1, (LPARAM)szFileName) == LB_ERR)
                {
                    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szFileName);
                }
                else
                {
                    MessageBox(hwnd, "无法拖入已经拖入的文件。", "错误", MB_ICONERROR);
                }
            }
            DragFinish(hDrop);
        }
        break;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case 2: // "重新选择"按钮
                    SendMessage(hList, LB_RESETCONTENT, 0, 0);
                    break;
                case 3: // "取消"按钮
                    PostQuitMessage(0);
                    break;
                case 4: // "确认"按钮
                {
                    FILE* pFile = fopen(txtFilePath, "w");
                    if(pFile != NULL)
                    {
                        int count = SendMessage(hList, LB_GETCOUNT, 0, 0);
                        char szFileName[MAX_PATH];
                        for(int i = 0; i < count; i++)
                        {
                            SendMessage(hList, LB_GETTEXT, i, (LPARAM)szFileName);
                            fprintf(pFile, "%s\n", szFileName);
                        }
                        fclose(pFile);
                        // 文件路径已保存，无需弹出消息框，直接关闭程序
                        PostQuitMessage(0); // 关闭程序
                    }
                    else
                    {
                        MessageBox(hwnd, "无法打开指定的文本文件。", "错误", MB_ICONERROR);
                    }
                }
                break;
            }
        }
        break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}