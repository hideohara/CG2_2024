#include <Windows.h>

// ver 1

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE, LPSTR, int) {
    
    //int a = 5;
    //float b = a;
// 出力ウィンドウへの文字出力
    OutputDebugStringA("Hello, DirectX!\n");

    return 0;
}


