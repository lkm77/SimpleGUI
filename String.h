#pragma once
#ifndef SimpleGUI_String
#define SimpleGUI_String

#include <string>
namespace SimpleGUI
{
#ifdef UNICODE
    typedef std::wstring String;
    typedef wchar_t Char;
#else
    typedef std::string String;
    typedef char Char;
#endif 
}

#endif