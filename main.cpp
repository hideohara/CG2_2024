#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <cassert>
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include <fstream>
#include <sstream>

#include <numbers>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


#include "Input.h"

#include "WinApp.h"


#include "DirectXCommon.h"

#include "Logger.h"
#include "StringUtility.h"



struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Vector2 {
    float x;
    float y;
};


struct Matrix4x4 {
    float m[4][4];
};

struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};

struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct MaterialData
{
    std::string textureFilePath;
};

struct ModelData {
    std::vector<VertexData> vertices;
    MaterialData material;
};

struct Material {
    Vector4 color;
    int32_t enableLighting;
};

struct TransformationMatrix {
    Matrix4x4 WVP;
    Matrix4x4 World;
};

struct DirectionalLight {
    Vector4 color; //!< ライトの色
    Vector3 direction; //!< ライトの向き
    float intensity; //!< 輝度
};

float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

float Length(const Vector3& v) { return std::sqrt(Dot(v, v)); }

Vector3 Normalize(const Vector3& v) {
    float length = Length(v);
    if (length == 0.0f) {
        return v;
    }
    return { v.x / length, v.y / length, v.z / length };
}


Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
    Matrix4x4 result;
    result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
    result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
    result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
    result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

    result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
    result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
    result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
    result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

    result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
    result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
    result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
    result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

    result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
    result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
    result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
    result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

    return result;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    return { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosTheta, sinTheta, 0.0f,
            0.0f, -sinTheta, cosTheta, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
}


Matrix4x4 MakeRotateYMatrix(float radian) {
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    return { cosTheta, 0.0f, -sinTheta, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sinTheta, 0.0f, cosTheta, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
}

Matrix4x4 MakeRotateZMatrix(float radian) {
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    return { cosTheta, sinTheta, 0.0f, 0.0f,
            -sinTheta, cosTheta, 0.0f , 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
}


Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
    Matrix4x4 result = Multiply(Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)), MakeRotateZMatrix(rotate.z));
    result.m[0][0] *= scale.x;
    result.m[0][1] *= scale.x;
    result.m[0][2] *= scale.x;

    result.m[1][0] *= scale.y;
    result.m[1][1] *= scale.y;
    result.m[1][2] *= scale.y;

    result.m[2][0] *= scale.z;
    result.m[2][1] *= scale.z;
    result.m[2][2] *= scale.z;

    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    return result;
}

Matrix4x4 MakeIdentity4x4() {
    // clang-format off
    Matrix4x4 identity;
    identity.m[0][0] = 1.0f;	identity.m[0][1] = 0.0f;	identity.m[0][2] = 0.0f;	identity.m[0][3] = 0.0f;
    identity.m[1][0] = 0.0f;	identity.m[1][1] = 1.0f;	identity.m[1][2] = 0.0f;	identity.m[1][3] = 0.0f;
    identity.m[2][0] = 0.0f;	identity.m[2][1] = 0.0f;	identity.m[2][2] = 1.0f;	identity.m[2][3] = 0.0f;
    identity.m[3][0] = 0.0f;	identity.m[3][1] = 0.0f;	identity.m[3][2] = 0.0f;	identity.m[3][3] = 1.0f;
    return identity;
    // clang-format on
}

// clang-format off
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
    float cotHalfFovV = 1.0f / std::tan(fovY / 2.0f);
    return {
        (cotHalfFovV / aspectRatio), 0.0f, 0.0f, 0.0f,
        0.0f, cotHalfFovV, 0.0f, 0.0f,
        0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
        0.0f, 0.0f, -(nearClip * farClip) / (farClip - nearClip), 0.0f
    };
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
    return {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (farClip - nearClip), 0.0f,
        (left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1.0f,
    };
}



Matrix4x4 Inverse(const Matrix4x4& m) {
    // clang-format off
    float determinant = +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
        + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
        + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

        - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
        - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
        - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

        - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
        - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
        - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

        + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
        + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
        + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

        + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
        + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
        + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

        - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
        - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
        - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

        - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
        - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
        - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

        + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
        + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
        + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

    Matrix4x4 result;
    float recpDeterminant = 1.0f / determinant;
    result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
        m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
        m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
    result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
        m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
        m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
    result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
        m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
        m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
    result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
        m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
        m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

    result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
        m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
        m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
    result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
        m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
        m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
    result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
        m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
        m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
    result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
        m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
        m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

    result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
        m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
        m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
    result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
        m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
        m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
    result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
        m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
        m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) * recpDeterminant;
    result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
        m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
        m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) * recpDeterminant;

    result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
        m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
        m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
    result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
        m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
        m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
    result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
        m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
        m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) * recpDeterminant;
    result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
        m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
        m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) * recpDeterminant;

    return result;
    // clang-format on
}

//std::wstring ConvertString(const std::string& str) {
//    if (str.empty()) {
//        return std::wstring();
//    }
//
//    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
//    if (sizeNeeded == 0) {
//        return std::wstring();
//    }
//    std::wstring result(sizeNeeded, 0);
//    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
//    return result;
//}
//
//std::string ConvertString(const std::wstring& str) {
//    if (str.empty()) {
//        return std::string();
//    }
//
//    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
//    if (sizeNeeded == 0) {
//        return std::string();
//    }
//    std::string result(sizeNeeded, 0);
//    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
//    return result;
//}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    // 1. 中で必要となる変数の宣言
    // 2. ファイルを開く
    MaterialData materialData; // 構築するMaterialData
    std::string line; // ファイルから読んだ1行を格納するもの
    std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
    assert(file.is_open());  // とりあえず開けなかったら止める

    // 3. 実際にファイルを読み、MaterialDataを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            // 連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }

    // 4. MaterialData を返す
    return materialData;
}

// Obj 読み込み
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
    // 1. 中で必要となる変数の宣言
    ModelData modelData; // 構築するModelData
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals; // 法線
    std::vector<Vector2> texcoords; // テクスチャ座標
    std::string line; // ファイルから読んだ1行を格納するもの

    // 2. ファイルを開く
    std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
    assert(file.is_open());  // とりあえず開けなかったら止める

    // 3. 実際にファイルを読み、ModelDataを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;// 先頭の識別子を読む

        // identifierに応じた処理
        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.z *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (identifier == "f") {
            VertexData triangle[3];
            // 面は三角形限定。その他は未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                // 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');// /区切りでインデックスを読んでいく
                    elementIndices[element] = std::stoi(index);
                }
                // 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                //VertexData vertex = { position, texcoord };
                //modelData.vertices.push_back(vertex);
                triangle[faceVertex] = { position, texcoord };
            }
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }
        else if (identifier == "mtllib") {
            // materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }

    }

    // 4. ModelDataを返す
    return modelData;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
    // テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    // ミップマップ付きのデータを返す
    return mipImages;
}




ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
    // 1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width); // Textureの幅
    resourceDesc.Height = UINT(metadata.height); // Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; // TextureのFormat
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは2次元

    // 2. 利用するHeapの設定
    // 利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //　プロセッサの近くに配置

    // 3. Resourceを生成する
    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
        &resourceDesc,  // Resourceの設定
        D3D12_RESOURCE_STATE_GENERIC_READ,  // 初回のResourceState。Textureは基本読むだけ
        nullptr, // Clear最適値。使わないのでnullptr
        IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
    assert(SUCCEEDED(hr));
    return resource;

}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
    // Meta情報を取得
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    // 全MipMapについて
    for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
        // MipMapLevelを指定して各Imageを取得
        const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
        // Textureに転送
        HRESULT hr = texture->WriteToSubresource(
            UINT(mipLevel),
            nullptr,              // 全領域へコピー
            img->pixels,          // 元データアドレス
            UINT(img->rowPitch),  // 1ラインサイズ
            UINT(img->slicePitch) // 1枚サイズ
        );
        assert(SUCCEEDED(hr));
    }
}

//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
//{
//    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
//    handleCPU.ptr += (descriptorSize * index);
//    return handleCPU;
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
//{
//    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
//    handleGPU.ptr += (descriptorSize * index);
//    return handleGPU;
//}



//ID3D12DescriptorHeap* CreateDescriptorHeap(
//    ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
//{
//    ID3D12DescriptorHeap* descriptorHeap = nullptr;
//    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
//    descriptorHeapDesc.Type = heapType;
//    descriptorHeapDesc.NumDescriptors = numDescriptors;
//    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//    HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
//    assert(SUCCEEDED(hr));
//    return descriptorHeap;
//}


//ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
//{
//    // 生成するResourceの設定
//    D3D12_RESOURCE_DESC resourceDesc{};
//    resourceDesc.Width = width; // Textureの幅
//    resourceDesc.Height = height; // Textureの高さ
//    resourceDesc.MipLevels = 1; // mipmapの数
//    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
//    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
//    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
//    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
//    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知
//
//    // 利用するHeapの設定
//    D3D12_HEAP_PROPERTIES heapProperties{};
//    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る
//
//    // 深度値のクリア設定
//    D3D12_CLEAR_VALUE depthClearValue{};
//    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f（最大値）でクリア
//    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる
//
//    // Resourceの生成
//    ID3D12Resource* resource = nullptr;
//    HRESULT hr = device->CreateCommittedResource(
//        &heapProperties, // Heapの設定
//        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
//        &resourceDesc,  // Resourceの設定
//        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
//        &depthClearValue, // Clear最適値
//        IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
//    assert(SUCCEEDED(hr));
//
//    return resource;
//}


//void Log(const std::string& message) {
//    OutputDebugStringA(message.c_str());
//}
//
//void Log(const std::wstring& message) {
//    OutputDebugStringW(message.c_str());
//}

IDxcBlob* CompileShader(
    // CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    // Compilerに使用するProfile
    const wchar_t* profile,
    // 初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler)
{
    // ここの中身をこの後書いていく
    // 1. hlslファイルを読む
    // これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら止める
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

    // 2. Compileする
    LPCWSTR arguments[] = {
        filePath.c_str(), // コンパイル対象のhlslファイル名
        L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile, // ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",   // デバッグ用の情報を埋め込む
        L"-Od",     // 最適化を外しておく
        L"-Zpr",    // メモリレイアウトは行優先
    };
    // 実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,    // 読み込んだファイル
        arguments,              // コンパイルオプション
        _countof(arguments),    // コンパイルオプションの数
        includeHandler,         // includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );
    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));

    // 3. 警告・エラーがでていないか確認する
    // 警告・エラーが出てたらログに出して止める
    IDxcBlobUtf8* shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        // 警告・エラーダメゼッタイ
        assert(false);
    }

    // 4. Compile結果を受け取って返す
    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
    // 頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
    // 頂点リソースの設定
    D3D12_RESOURCE_DESC vertexResourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexResourceDesc.Width = sizeInBytes;
    // バッファの場合はこれらは1にする決まり
    vertexResourceDesc.Height = 1;
    vertexResourceDesc.DepthOrArraySize = 1;
    vertexResourceDesc.MipLevels = 1;
    vertexResourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // 実際に頂点リソースを作る
    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    return resource;
}

/*
// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された
    case WM_DESTROY:
        // OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
*/


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    HRESULT hr;

    // ポインタ
    WinApp* winApp = nullptr;

    // WindowsAPIの初期化
    winApp = new WinApp();
    winApp->Initialize();

    // ポインタ
    Input* input = nullptr;
    // 入力の初期化
    input = new Input();
    input->Initialize(winApp);


    // ***********************************

#ifdef _DEBUG
    ID3D12Debug1* debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif


    // ***********************************

    // ポインタ
    DirectXCommon* dxCommon = nullptr;

    // DirectXの初期化
    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);


    // --------------------------------------


    // RootSignature作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;  // 0から始まる
    descriptorRange[0].NumDescriptors = 1;  // 数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


    // RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;    // レジスタ番号0とバインド
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;   // VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0;    // レジスタ番号0を使う
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;  // Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShdaderで使う
    rootParameters[3].Descriptor.ShaderRegister = 1;    // レジスタ番号1を使う

    descriptionRootSignature.pParameters = rootParameters;  // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;   // 0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;   // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0;   // レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);





    // シリアライズしてバイナリにする
    ID3DBlob* signatureBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    // バイナリを元に生成
    ID3D12RootSignature* rootSignature = nullptr;
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    // InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;



    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);



    // BlendStateの設定
    D3D12_BLEND_DESC blendDesc{};
    // すべての色要素を書き込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // RasiterzerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    // 裏面（時計回り）を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    //rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    // 三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    // Depthの機能を有効化する
    depthStencilDesc.DepthEnable = true;
    // 書き込みします
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    // 比較関数はLessEqual。つまり、近ければ描画される
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;




    // Shaderをコンパイルする
    IDxcBlob* vertexShaderBlob = CompileShader(L"resources/shaders/Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
    assert(vertexShaderBlob != nullptr);

    IDxcBlob* pixelShaderBlob = CompileShader(L"resources/shaders/Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
    assert(pixelShaderBlob != nullptr);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature;// RootSignature
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };// VertexShader
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };// PixelShader
    graphicsPipelineStateDesc.BlendState = blendDesc;// BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;// RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ（形状）のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // どのように画面に色を打ち込むかの設定（気にしなくて良い）
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // DepthStencilの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 実際に生成
    ID3D12PipelineState* graphicsPipelineState = nullptr;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
    assert(SUCCEEDED(hr));

    // ------------------------------------------------

    const uint32_t kSubdivision = 16; //分割数
    const uint32_t kVertexCount = kSubdivision * kSubdivision * 6;//球体頂点数

    ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * kVertexCount);
    // モデル読み込み
    //ModelData modelData = LoadObjFile("resources", "plane.obj");
    //ModelData modelData = LoadObjFile("resources", "axis.obj");
    //ModelData modelData = LoadObjFile("resources", "cube.obj");
    // 頂点リソースを作る
    //ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());

     //頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * kVertexCount;
    // 1頂点あたりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // 頂点バッファビューを作成する
    //D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();// リソースの先頭のアドレスから使う
    //vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());// 使用するリソースのサイズは頂点のサイズ
    //vertexBufferView.StrideInBytes = sizeof(VertexData);// 1頂点あたりのサイズ


    // 頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    // 書き込むためのアドレスを取得
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    /*
        // 左下
        vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
        vertexData[0].texcoord = { 0.0f, 1.0f };
        // 上
        vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
        vertexData[1].texcoord = { 0.5f, 0.0f };
        // 右下
        vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
        vertexData[2].texcoord = { 1.0f, 1.0f };

        // 左下2
        vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
        vertexData[3].texcoord = { 0.0f, 1.0f };
        // 上2
        vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
        vertexData[4].texcoord = { 0.5f, 0.0f };
        // 右下2
        vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
        vertexData[5].texcoord = { 1.0f, 1.0f };
    */

    //球体用頂点
    const float kPi = std::numbers::pi_v<float>;
    const float kLonEvery = (2 * kPi) / float(kSubdivision); //経度分割1つ分の角度
    const float kLatEvery = kPi / float(kSubdivision); //緯度分割1つ分の角度
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
        float lat = -kPi / 2.0f + kLatEvery * latIndex;
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
            uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
            float lon = lonIndex * kLonEvery;
            //a
            vertexData[start].position.x = cos(lat) * cos(lon);
            vertexData[start].position.y = sin(lat);
            vertexData[start].position.z = cos(lat) * sin(lon);
            vertexData[start].position.w = 1.0f;
            vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
            vertexData[start].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
            vertexData[start].normal.x = vertexData[start].position.x;
            vertexData[start].normal.y = vertexData[start].position.y;
            vertexData[start].normal.z = vertexData[start].position.z;

            //b
            vertexData[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
            vertexData[start + 1].position.y = sin(lat + kLatEvery);
            vertexData[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
            vertexData[start + 1].position.w = 1.0f;
            vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
            vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
            vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
            vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
            vertexData[start + 1].normal.z = vertexData[start + 1].position.z;

            //c
            vertexData[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
            vertexData[start + 2].position.y = sin(lat);
            vertexData[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
            vertexData[start + 2].position.w = 1.0f;
            vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
            vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
            vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
            vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
            vertexData[start + 2].normal.z = vertexData[start + 2].position.z;

            //c
            vertexData[start + 3] = vertexData[start + 2];
            //b
            vertexData[start + 4] = vertexData[start + 1];
            //d
            vertexData[start + 5].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
            vertexData[start + 5].position.y = sin(lat + kLatEvery);
            vertexData[start + 5].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
            vertexData[start + 5].position.w = 1.0f;
            vertexData[start + 5].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
            vertexData[start + 5].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
            vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
            vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
            vertexData[start + 5].normal.z = vertexData[start + 5].position.z;
        }
    }



    // 頂点リソースにデータを書き込む
    //VertexData* vertexData = nullptr;
    //vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
    //std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());// 頂点データをリソースにコピー

    // マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Material));
    // マテリアルにデータを書き込む
    Material* materialData = nullptr;
    // 書き込むためのアドレスを取得
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    // マテリアルの内容
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = true;

    // Sprite用のマテリアルリソースを作る
    ID3D12Resource* materialResourceSprite = CreateBufferResource(device, sizeof(Material));
    // マテリアルにデータを書き込む
    Material* materialDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
    // マテリアルの内容
    materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialDataSprite->enableLighting = false;

    // WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
    // データを書き込む
    TransformationMatrix* wvpData = nullptr;
    // 書き込むためのアドレスを取得
    wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    // 単位行列を書きこんでおく
    wvpData->WVP = MakeIdentity4x4();
    wvpData->World = MakeIdentity4x4();



    // Transform変数を作る
    Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
    
    //Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };
    Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f, 0.0f}, {0.0f, 4.0f, -10.0f} };

    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);

    // IMGUI


    // Textureを読んで転送する
    DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
    //DirectX::ScratchImage mipImages = LoadTexture(modelData.material.textureFilePath);
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
    UploadTextureData(textureResource, mipImages);

    // 2枚目のTextureを読んで転送する
    DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
    const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
    ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
    UploadTextureData(textureResource2, mipImages2);



    // 実際にShaderResourceViewを作る
    // metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める
    //D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    //// 先頭はImGuiが使っているのでその次を使う
    //textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 1);
    //D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 1);


    // SRVの生成
    device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

    // meataDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
    srvDesc2.Format = metadata2.format;
    srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
    // SRVの生成
    device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);


    // --------------------------------------

    
    // Sprite用の頂点リソースを作る
    ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
    // リソースの先頭のアドレスから使う
    vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点6つ分のサイズ
    vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
    // 1頂点あたりのサイズ
    vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

    VertexData* vertexDataSprite = nullptr;
    vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

    /*
    // 1枚目の三角形
    vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };// 左下
    vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
    vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 左上
    vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
    vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };// 右下
    vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
    // 2枚目の三角形
    vertexDataSprite[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 左上
    vertexDataSprite[3].texcoord = { 0.0f, 0.0f };
    vertexDataSprite[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };// 右上
    vertexDataSprite[4].texcoord = { 1.0f, 0.0f };
    vertexDataSprite[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };// 右下
    vertexDataSprite[5].texcoord = { 1.0f, 1.0f };
    */

    // 左下
    vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
    vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };
    // 左上
    vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
    vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };
    // 右下
    vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
    vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };
    // 右上
    vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
    vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };


    // Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
    // データを書き込む
    TransformationMatrix* transformationMatrixDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
    // 単位行列を書きこんでおく
    // *transformationMatrixDataSprite = MakeIdentity4x4();
    transformationMatrixDataSprite->WVP = MakeIdentity4x4();
    transformationMatrixDataSprite->World = MakeIdentity4x4();

    // CPUで動かす用のTransformを作る
    Transform transformSprite{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };


    // --------------------------------------

    // 頂点インデックス
    ID3D12Resource* indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

    D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
    // リソースの先頭のアドレスから使う
    indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
    // 使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_tとする
    indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

    // インデックスリソースにデータを書き込む
    uint32_t* indexDataSprite = nullptr;
    indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
    indexDataSprite[0] = 0;	indexDataSprite[1] = 1;	indexDataSprite[2] = 2;
    indexDataSprite[3] = 1;	indexDataSprite[4] = 3;	indexDataSprite[5] = 2;


    // --------------------------------------

    bool useMonsterBall = true;


    // --------------------------------------

    // ライト用のリソースを作る。
    ID3D12Resource* directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
    // マテリアルにデータを書き込む
    DirectionalLight* directionalLightData = nullptr;
    // 書き込むためのアドレスを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

    // デフォルト値はとりあえず以下のようにしておく
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData->intensity = 1.0f;


    // -----------------------------------------------



    // ------------------------------------------------


    MSG msg{};
    // ウィンドウの×ボタンが押されるまでループ
    while (msg.message != WM_QUIT) {
        //// Windowにメッセージが来てたら最優先で処理させる
        //if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        //    TranslateMessage(&msg);
        //    DispatchMessage(&msg);
        //}
        //else {
        //}


        // Windowsのメッセージ処理
        if (winApp->ProcessMessage()) {
            // ゲームループを抜ける
            break;
        }

        // 入力の更新
        input->Update();


        //// 数字の0キーが押されていたら
        if (input->TriggerKey(DIK_0))
        {
            OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
        }

/*

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
        //ImGui::ShowDemoWindow();
        ImGui::Begin("Settings");
        ImGui::ColorEdit4("material", &materialData->color.x, ImGuiColorEditFlags_AlphaPreview);
        ImGui::SliderAngle("rotate.y", &transform.rotate.y);
        ImGui::Checkbox("useMonsterBall", &useMonsterBall);
        ImGui::DragFloat3("light", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
        ImGui::End();

        // 方向は正規化
        directionalLightData->direction = Normalize(directionalLightData->direction);


        //transform.rotate.y += 0.03f;
        //Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
        //*wvpData = worldMatrix;

        Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
        Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
        Matrix4x4 viewMatrix = Inverse(cameraMatrix);
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
        Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
        wvpData->WVP = worldViewProjectionMatrix;
        wvpData->World = worldMatrix;

        // Sprite用のWorldViewProjectionMatrixを作る
        Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
        Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
        Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
        Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
        transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
        transformationMatrixDataSprite->World = worldMatrixSprite;

        // ImGuiの内部コマンドを生成する
        ImGui::Render();
*/
        // 描画前処理
        dxCommon->PreDraw();

/*
        // RootSignatureを設定。PSOに設定しているけど別途設定が必要
        commandList->SetGraphicsRootSignature(rootSignature);
        commandList->SetPipelineState(graphicsPipelineState);   // PSOを設定
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);   // VBVを設定
        // 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // マテリアルCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
        // wvp用のCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
        // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
        // commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
        commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
        // DirectionalLightのCBufferの場所を設定(ドローコールの前に挿入)
        commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
        // 描画！（DrawCall/ドローコール）。3頂点で1つのインスタンス。インスタンスについては今後
        //commandList->DrawInstanced(6, 1, 0, 0);
        commandList->DrawInstanced(kVertexCount, 1, 0, 0);
        //commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

        
        // Spriteの描画。変更が必要なものだけ変更する
        commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);   // VBVを設定
        // マテリアルCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
        // インデックス
        commandList->IASetIndexBuffer(&indexBufferViewSprite);// IBVを設定
        // TransformationMatrixCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
        // texture
        commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
        // 描画！（DrawCall/ドローコール）
        //commandList->DrawInstanced(6, 1, 0, 0);
        // 描画！（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い
        commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
        



        // 実際のcommandListのImGuiの描画コマンドを積む
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
*/
        // 描画後処理
        dxCommon->PostDraw();
    }

    // 文字列を格納する
    std::string str0{ "STRING!!!＊＊＊＊＊＊＊＊＊" };

    // 整数を文字列にする
    std::string str1{ std::to_string(10) };

    // string->wstring
    //std::wstring ConvertString(const std::string & str);
    // wstring->string
    //std::string ConvertString(const std::wstring & str);
/*
    // 変数から型を推論してくれる
    Log(std::format("enemyHp:{}, texturePath:{}\n", 2, "aaaa"));

    CloseHandle(fenceEvent);

    // ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
    // こういうもんである。初期化と逆順に行う
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    directionalLightResource->Release();
    indexResourceSprite->Release();
    vertexResourceSprite->Release();
    transformationMatrixResourceSprite->Release();
    textureResource2->Release();
    textureResource->Release();
    materialResourceSprite->Release();
    materialResource->Release();
    wvpResource->Release();
    vertexResource->Release();
    graphicsPipelineState->Release();
    signatureBlob->Release();
    if (errorBlob) {
        errorBlob->Release();
    }
    rootSignature->Release();
    pixelShaderBlob->Release();
    vertexShaderBlob->Release();

    fence->Release();
    dsvDescriptorHeap->Release();
    rtvDescriptorHeap->Release();
    srvDescriptorHeap->Release();
    depthStencilResource->Release();
    swapChainResources[0]->Release();
    swapChainResources[1]->Release();
    swapChain->Release();
    commandList->Release();
    commandAllocator->Release();
    commandQueue->Release();
    device->Release();
    useAdapter->Release();
    dxgiFactory->Release();
#ifdef _DEBUG
    debugController->Release();
#endif
    //CloseWindow(winApp->GetHwnd());

    */

    // WindowsAPIの終了処理
    winApp->Finalize();

    // DirectX解放
    delete dxCommon;

    // 入力解放
    delete input;

    // WindowsAPI解放
    delete winApp;



    // リソースリークチェック
    IDXGIDebug1* debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }

    //CoUninitialize();


    return 0;
}


