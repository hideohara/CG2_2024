#include <Windows.h>

// ver 1

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE, LPSTR, int) {

// 出力ウィンドウへの文字出力
    OutputDebugStringA("aaa Hello,DirectX!\n");

    return 0;
}


