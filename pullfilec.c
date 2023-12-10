#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND hwndButtonConfirm, hwndButtonCancel, hwndButtonReset, hwndTextBox;
char filePaths[1024] = "";
char outputFile[1024] = "";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 检查命令行参数，如果为空，则直接退出
    if (lpCmdLine == NULL || strlen(lpCmdLine) == 0)
    {
        return 0;
    }

    const char *className = "MyWindowClass";
    const char *windowTitle = "文件路径选择";

    // 获取命令行参数
    strncpy(outputFile, lpCmdLine, sizeof(outputFile) - 1);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        className,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL,
        NULL,
        hInstance,
        NULL);

    hwndButtonConfirm = CreateWindow(
        "BUTTON",
        "确认",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 200, 80, 25,
        hwnd,
        (HMENU)1,
        (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
        NULL);

    hwndButtonCancel = CreateWindow(
        "BUTTON",
        "取消",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        260, 200, 80, 25,
        hwnd,
        (HMENU)2,
        (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
        NULL);

    hwndButtonReset = CreateWindow(
        "BUTTON",
        "重新选择",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        170, 200, 80, 25,
        hwnd,
        (HMENU)3,
        (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
        NULL);

    hwndTextBox = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 10, 460, 170,
        hwnd,
        (HMENU)4,
        (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
        NULL);

    SetWindowText(hwndTextBox, "请拖入一个或多个文件");

    HFONT hFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "黑体");
    SendMessage(hwndTextBox, WM_SETFONT, (WPARAM)hFont, TRUE);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
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
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case 1: // 确认按钮
                {
                    FILE *fp = fopen(outputFile, "w");
                    if (fp != NULL)
                    {
                        fputs(filePaths, fp);
                        fclose(fp);
                    }
                    DestroyWindow(hwnd);
                    break;
                }
                case 2: // 取消按钮
                    DestroyWindow(hwnd);
                    break;
                case 3: // 重新选择按钮
                    SetWindowText(hwndTextBox, "请拖入一个或多个文件");
                    filePaths[0] = '\0';
                    break;
            }
            break;
        case WM_DROPFILES:
        {
            HDROP hDrop = (HDROP)wParam;
            UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

            for (UINT i = 0; i < fileCount; i++)
            {
                UINT filePathLength = DragQueryFile(hDrop, i, NULL, 0);
                char *filePath = (char *)malloc((filePathLength + 3) * sizeof(char));
                DragQueryFile(hDrop, i, filePath, filePathLength + 1);

                // 检查是否为文件夹
                DWORD attributes = GetFileAttributes(filePath);
                if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    free(filePath);
                    continue;
                }

                if (strlen(filePaths) > 0)
                {
                    strcat(filePaths, "\r\n");
                }
                strcat(filePaths, filePath);

                free(filePath);
            }

            SetWindowText(hwndTextBox, filePaths);

            DragFinish(hDrop);
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}