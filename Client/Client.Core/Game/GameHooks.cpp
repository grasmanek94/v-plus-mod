#include "Main.h"
#include <winsock2.h>

static const char * (*g_origGetText) (void *theText, uint32_t hash);

static std::mutex g_textMutex;
static std::unordered_map<uint32_t, std::string> g_textMap;

int __stdcall HookedConnect(SOCKET s, sockaddr *name, int namelen) { return SOCKET_ERROR; }
int __stdcall HookedRecv(SOCKET s, char *buf, int len, int flags) { return SOCKET_ERROR; }
int __stdcall HookedRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen) { return SOCKET_ERROR; }
int __stdcall HookedSend(SOCKET s, char *buf, int len, int flags) { return SOCKET_ERROR; }
int __stdcall HookedSendTo(SOCKET s, char *buf, int len, int flags, sockaddr *to, int tolen) { return SOCKET_ERROR; }
hostent * __stdcall HookedGetHostByName(char *name) { return NULL; }
int __stdcall HookedGetHostName(char *name, int namelen) { return SOCKET_ERROR; }

static const char* GetText(void* theText, uint32_t hash)
{
	{
		std::unique_lock<std::mutex> lock(g_textMutex);

		auto it = g_textMap.find(hash);

		if (it != g_textMap.end())
		{
			return it->second.c_str();
		}
	}

	return g_origGetText(theText, hash);
}

void AddCustomText(const char *key, const char *value)
{
	std::unique_lock<std::mutex> lock(g_textMutex);
	g_textMap[SharedUtility::HashString(key)] = value;
}

bool GameHooks::InstallHooks()
{
	hook::iat("ws2_32.dll", HookedConnect, 4);
	hook::iat("ws2_32.dll", HookedRecv, 16);
	hook::iat("ws2_32.dll", HookedRecvFrom, 17);
	hook::iat("ws2_32.dll", HookedSend, 19);
	hook::iat("ws2_32.dll", HookedSendTo, 20);
	hook::iat("ws2_32.dll", HookedGetHostByName, 52);
	hook::iat("ws2_32.dll", HookedGetHostName, 57);

	g_textMap[0x748C6374] = "Loading V+";
	g_textMap[0xABB00DEB] = "V+";

	void* getTextPtr = hook::pattern("48 8B CB 8B D0 E8 ? ? ? ? 48 85 C0 0F 95 C0").count(1).get(0).get<void>(5);
	hook::set_call(&g_origGetText, getTextPtr);
	hook::call(getTextPtr, GetText);

	hook::call(hook::pattern("48 85 C0 75 34 8B 0D").count(1).get(0).get<void>(-5), GetText);
	return true;
}
