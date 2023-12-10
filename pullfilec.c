#include <windows.h>
#include <stdio.h>
#include <string.h>

// ȫ�ֱ���
HWND hList; // �б����
HWND hButtonClear, hButtonCancel, hButtonConfirm; // ��ť���
char txtFilePath[MAX_PATH] = "filepath.txt"; // Ĭ���ı��ļ�·��

// ���������ͱ���
const char g_szClassName[] = "myWindowClass";
const char g_szWindowTitle[] = "���ļ�����˴���";

// ���ڹ��̺�������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ���������в���
void ParseCommandLine(LPSTR lpCmdLine) {
    // ���������в�������ʽΪ "-o [txt�ļ�·��]"
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

// WinMain����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // ���������в���
    ParseCommandLine(lpCmdLine);

    // ע�ᴰ����
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
        MessageBox(NULL, "����ע��ʧ�ܣ�", "����",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ��������
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        g_szWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "���ڴ���ʧ�ܣ�", "����",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // �����б��Ͱ�ť
    hList = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
        10, 10, 460, 200, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

    hButtonClear = CreateWindow("BUTTON", "����ѡ��",
        WS_VISIBLE | WS_CHILD, 10, 220, 100, 30, hwnd, (HMENU)2, NULL, NULL);
    hButtonCancel = CreateWindow("BUTTON", "ȡ��",
        WS_VISIBLE | WS_CHILD, 120, 220, 100, 30, hwnd, (HMENU)3, NULL, NULL);
    hButtonConfirm = CreateWindow("BUTTON", "ȷ��",
        WS_VISIBLE | WS_CHILD, 230, 220, 100, 30, hwnd, (HMENU)4, NULL, NULL);

    // �����ϷŹ���
    DragAcceptFiles(hwnd, TRUE);

    // ��Ϣѭ��
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

// ���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            // ����WinMain�д����ؼ�
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

                // ����Ƿ�Ϊ�ļ���
                if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    MessageBox(hwnd, "�޷������ļ��С�", "����", MB_ICONERROR);
                    continue; // �����ļ���
                }

                // ����ļ��Ƿ��Ѿ����б���
                if (SendMessage(hList, LB_FINDSTRINGEXACT, -1, (LPARAM)szFileName) == LB_ERR)
                {
                    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szFileName);
                }
                else
                {
                    MessageBox(hwnd, "�޷������Ѿ�������ļ���", "����", MB_ICONERROR);
                }
            }
            DragFinish(hDrop);
        }
        break;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case 2: // "����ѡ��"��ť
                    SendMessage(hList, LB_RESETCONTENT, 0, 0);
                    break;
                case 3: // "ȡ��"��ť
                    PostQuitMessage(0);
                    break;
                case 4: // "ȷ��"��ť
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
                        // �ļ�·���ѱ��棬���赯����Ϣ��ֱ�ӹرճ���
                        PostQuitMessage(0); // �رճ���
                    }
                    else
                    {
                        MessageBox(hwnd, "�޷���ָ�����ı��ļ���", "����", MB_ICONERROR);
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