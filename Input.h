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

    /// <summary>
    /// キーの押下をチェック
    /// </summary>
    /// <param name="keyNumber">キー番号( DIK_0 等)</param>
    /// <returns>押されているか</returns>
    bool PushKey(BYTE keyNumber);

    /// <summary>
    /// キーのトリガーをチェック
    /// </summary>
    /// <param name="keyNumber">キー番号( DIK_0 等)</param>
    /// <returns>トリガーか</returns>
    bool TriggerKey(BYTE keyNumber);

private:
    BYTE key[256] = {};
    BYTE preKey[256] = {};

    IDirectInput8* directInput = nullptr;
    IDirectInputDevice8* keyboard = nullptr;

};

