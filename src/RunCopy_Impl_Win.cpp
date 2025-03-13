//
// Created by YubinKim on 25/03/13 목.
//
#ifdef _WIN32

#include <vector>

#include "RunCopKey.h"
#include <Windows.h>

void ReleaseAllKeys()
{
    // 눌러져 있는 복합키 key up
    constexpr int KeyCount = 255;
    INPUT inputs[KeyCount] = {};
    ZeroMemory(inputs, sizeof(inputs));

    for (int vkey = 0; vkey < KeyCount; ++vkey)
    {
        const bool pressed = (GetAsyncKeyState(vkey) & (1 << 15)) != 0;
        // if (vkey != VK_MENU && pressed)
        {
            inputs[vkey].type       = INPUT_KEYBOARD;
            inputs[vkey].ki.wVk     = vkey;
            inputs[vkey].ki.dwFlags = KEYEVENTF_KEYUP;
        }
    }
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void RunCopKey::DoCopy()
{
    ReleaseAllKeys();

    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type   = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type   = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_INSERT; //'C';

    inputs[2].type       = INPUT_KEYBOARD;
    inputs[2].ki.wVk     = VK_INSERT; //'C';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type       = INPUT_KEYBOARD;
    inputs[3].ki.wVk     = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}


#endif // _WIN32
