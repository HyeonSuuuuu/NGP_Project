#include "pch.h"
#include "debugUtil.h"

void DebugPrint(const char* message) {
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

void DebugPrintWithVar(const char* message, int value) {
    char buffer[256];
    sprintf(buffer, "%s: %d ", message, value);
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
}