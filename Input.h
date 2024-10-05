#pragma once

#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <wrl.h>
//using namespace Microsoft::WRL;


class Input
{

public: // メンバ関数
    // namespace省略
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
    // DirectInputの初期化
    //IDirectInput8* directInput = nullptr;

    // DirectInputのインスタンス
    ComPtr<IDirectInput8> directInput;


    // キーボードデバイス生成
    ComPtr<IDirectInputDevice8> keyboard;

    BYTE key[256] = {};
    BYTE keyPre[256] = {};


};

