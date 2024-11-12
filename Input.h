#pragma once


#include <windows.h>
//#include <wrl.h>

//using namespace Microsoft::WRL;

#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


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

    IDirectInput8* directInput = nullptr;
    IDirectInputDevice8* keyboard = nullptr;

};

