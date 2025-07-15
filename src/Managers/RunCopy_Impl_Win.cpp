// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifdef _WIN32

#include <QThread>
#include <vector>

#include "RunCopKey.h"
#include <Windows.h>

/**
 * INPUT 구조체를 0으로 초기화하고, 대상 멤버 변수에 값을 입력합니다.
 * @param in_type type 변수값
 * @param in_wVK ki.wVk 변수값
 * @param in_dwFlags ki.dwFlags 변수값. 기본값(0)이면  KeyDown.
 * @return 
 */
INPUT make_INPUT(DWORD in_type, WORD in_wVK, DWORD in_dwFlags = 0)
{
    INPUT res;
    ZeroMemory(&res, sizeof(res));
    res.type       = in_type;
    res.ki.wVk     = in_wVK;
    res.ki.dwFlags = in_dwFlags;
    return res;
}

/**
 * 컨트롤 키가 눌린 상태에서 다른 모든키를 릴리즈 합니다.
 * 
 */
void ReleaseAllKeysOnPressedControlKey()
{
    constexpr int KeyCount = 255;

    std::vector<INPUT> inputs;
    inputs.reserve(KeyCount);

    inputs.push_back(make_INPUT(INPUT_KEYBOARD, VK_CONTROL));

    for (int vkey = 0; vkey < KeyCount; ++vkey)
    {
        const bool pressed = (GetAsyncKeyState(vkey) & (1 << 15)) != 0;
        if (vkey != VK_CONTROL && pressed)
        {
            inputs.push_back(make_INPUT(INPUT_KEYBOARD, vkey, KEYEVENTF_KEYUP));
        }
    }

    SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
}

void RunCopKey::DoCopy()
{
    ReleaseAllKeysOnPressedControlKey();

    std::array<INPUT, 4> inputs;

    inputs[0] = make_INPUT(INPUT_KEYBOARD, VK_CONTROL);
    inputs[1] = make_INPUT(INPUT_KEYBOARD, VK_INSERT);
    inputs[2] = make_INPUT(INPUT_KEYBOARD, VK_INSERT, KEYEVENTF_KEYUP);
    inputs[3] = make_INPUT(INPUT_KEYBOARD, VK_CONTROL, KEYEVENTF_KEYUP);

    SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
}


#endif // _WIN32
