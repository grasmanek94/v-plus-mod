#include "..\OpenVHook\inc\main.h"
#include "Script.hxx"

void KeyboardHandlerFunction(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

void External_Init(HINSTANCE hInstance)
{
	scriptRegister(hInstance, ScriptMain);
	keyboardHandlerRegister(KeyboardHandlerFunction);
}

void External_Cleanup(HINSTANCE hInstance)
{
	scriptUnregister(hInstance);
	keyboardHandlerUnregister(KeyboardHandlerFunction);
}
