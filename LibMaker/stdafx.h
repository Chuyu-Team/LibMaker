// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#include <Windows.h>

// TODO: 在此处引用程序需要的其他头文件
#include <BaseFunction.h>

#define YY_APPLY_ENUM_CALSS_BIT_OPERATOR(_ENUM)                      \
    inline constexpr _ENUM& operator|=(_ENUM& _eLeft, _ENUM _eRight) \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        (_Type&)_eLeft |= (_Type)_eRight;                            \
        return _eLeft;                                               \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator|(_ENUM _eLeft, _ENUM _eRight)    \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        auto _Result = (_Type)_eLeft | (_Type)_eRight;               \
        return _ENUM(_Result);                                       \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator&(_ENUM _eLeft, _ENUM _eRight)    \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return _ENUM((_Type)_eLeft & (_Type)_eRight);                \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator~(_ENUM _eLeft)                   \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return _ENUM(~(_Type)_eLeft);                                \
    }                                                                \
                                                                     \
    inline constexpr bool HasFlags(_ENUM _eLeft, _ENUM _eRight)      \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return (_Type)_eLeft & (_Type)_eRight;                       \
    }
