#pragma once


#include <windows.h>



class Input
{

public: // メンバ関数
    // 初期化
    void Initialize(HINSTANCE hInstance, HWND hwnd);
    // 更新
    void Update();

private:
    BYTE key[256] = {};
    BYTE preKey[256] = {};

};

