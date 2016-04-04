#pragma once

//#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <process.h>

#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

//http://stackoverflow.com/questions/16224273/warnings-using-vs2012-and-directx-june-2010
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <d3d11.h>
#include "FW1FontWrapper.h"
#pragma warning( pop )

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <set>

#include "SharedUtility.h"

#include "Utility/Hooking/Hooking.h"
#include "Utility/RAGE/pgCollection.h"

#include "Game/Input/InputHook.h"

#include "Game/CGameUtility.h"
#include "Game/CGameStateWatcher.h"
#include "Game/CGameAddresses.h"
#include "Game/CGamePatches.h"
#include "Game/CGameOverlay.h"
#include "Game/CGameStartupManager.h"
#include "Game/CGameVersion.h"

#include "Game/Scripting/NativeTranslationTables.h"
#include "Game/Scripting/ScriptThread.h"
#include "Game/Scripting/ScriptEngine.h"
#include "Game/Scripting/NativeInvoker.h"

#include "Game/Scripting/TestThread.h"

void TemporaryLogFunction(char *pszFormat, ...);

#define LOG_PRINT TemporaryLogFunction
#define LOG_DEBUG TemporaryLogFunction
#define LOG_WARNING TemporaryLogFunction
#define LOG_ERROR TemporaryLogFunction
