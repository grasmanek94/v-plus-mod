#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <list>

#ifdef _WIN32
	#define NOMINMAX

	#include <winsock2.h>
	#include <windows.h>
	#include <mmsystem.h>
	#include <direct.h>
	#include <WS2tcpip.h>
	#include <tlhelp32.h>
	#include <Shlwapi.h>
	#include <Psapi.h>

	#pragma comment(lib, "psapi.lib")
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <limits.h>
	#include <unistd.h>
	#include <stdarg.h>
	#include <sys/time.h>
#endif

#include "SharedUtility.h"

#ifndef _WIN32
	#define MAX_PATH PATH_MAX
#endif

namespace SharedUtility
{
#ifdef WIN32
	BOOL WINAPI ReadProcessMemoryEx(_In_ HANDLE hProcess,
		_In_ LPCVOID lpBaseAddress,
		_Out_writes_bytes_to_(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,
		_In_ SIZE_T nSize,
		_Out_opt_ SIZE_T * lpNumberOfBytesRead)
	{
		MEMORY_BASIC_INFORMATION mbi;
		DWORD flOldProtect;
 
		if (VirtualQueryEx(hProcess, lpBaseAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != sizeof(MEMORY_BASIC_INFORMATION))
		{
			//_tprintf(TEXT("VirtualQueryEx errorcode = %d\n"), GetLastError());
			return FALSE;
		}
 
		if (!mbi.Protect || (mbi.Protect & PAGE_GUARD))
			return FALSE;
 
		if (!(mbi.Protect & PAGE_READONLY))
		{
			if (!VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_READONLY, &flOldProtect))
				return FALSE;
 
			ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
 
			return VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, flOldProtect, &flOldProtect);
		}
 
		return ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
	}
 
	BOOL WINAPI InjectDllW(__in LPCWSTR lpcwszDll, __in DWORD dwProcessId)
	{
		INT nLength;
		DWORD dwTemp;
		HANDLE hProcess, hModuleSnap;
		LPVOID lpLoadLibraryW = NULL;
		LPVOID lpRemoteString;
		MODULEENTRY32 me32 = {0};
		BOOL Wow64Process;
 
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (hProcess == INVALID_HANDLE_VALUE)
			return FALSE;
 
		if (!IsWow64Process(hProcess, &Wow64Process))
		{
			CloseHandle(hProcess);
			return FALSE;
		}
 
		if (Wow64Process)
		{
			// process is x86 application running on x64 windows
			hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessId);
			if (hModuleSnap == INVALID_HANDLE_VALUE)
			{
				//_tprintf(TEXT("CreateToolhelp32Snapshot errorcode = %d\n"), GetLastError());
 
				// close process handle
				CloseHandle(hProcess);
 
				return FALSE;
			}
 
			// Set the size of the structure before using it.
			me32.dwSize = sizeof(MODULEENTRY32);
 
			// Retrieve information about the first module,
			// and exit if unsuccessful
			if (!Module32First(hModuleSnap, &me32))
			{
				//_tprintf(TEXT("Module32First errorcode = %d\n"), GetLastError());
				// clean the snapshot object
				CloseHandle(hModuleSnap);
 
				// close process handle
				CloseHandle(hProcess);
 
				return FALSE;
			}
 
			// Now walk the module list of the process,
			do
			{
				if (wcscmp(me32.szModule, L"KERNEL32.DLL") == 0)
				{
					IMAGE_NT_HEADERS32 nt;
					IMAGE_DOS_HEADER dos;
					IMAGE_EXPORT_DIRECTORY exports;
 
					// DOS HEADER
					if (!ReadProcessMemoryEx(hProcess, me32.hModule, &dos, sizeof(dos), NULL))
						break;
 
					// NT HEADER
					if (!ReadProcessMemoryEx(hProcess, reinterpret_cast<PBYTE>(me32.hModule) + dos.e_lfanew, &nt, sizeof(nt), NULL))
						break;
 
					// EXPORT TABLE
					if (!ReadProcessMemoryEx(hProcess, reinterpret_cast<PBYTE>(me32.hModule) + nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress, &exports, sizeof(exports), NULL))
						break;
 
					if (exports.NumberOfFunctions <= 0)
						break;
 
					DWORD* lpAddressOfNames = new DWORD[exports.NumberOfNames];
					CHAR szTemp[256];
 
					if (!ReadProcessMemoryEx(hProcess, reinterpret_cast<PBYTE>(me32.hModule) + exports.AddressOfNames, lpAddressOfNames, sizeof(DWORD) * exports.NumberOfNames, NULL))
						break;
 
					for (UINT uIndex = 0; uIndex < exports.NumberOfNames; uIndex++)
					{
						if (!ReadProcessMemoryEx(hProcess, reinterpret_cast<PBYTE>(me32.hModule) + lpAddressOfNames[uIndex], &szTemp, _countof(szTemp), NULL))
							break;
 
						if (strcmp(szTemp, "LoadLibraryW") == 0)
						{
							if (!ReadProcessMemoryEx(hProcess, reinterpret_cast<PBYTE>(me32.hModule) + exports.AddressOfFunctions + (uIndex * sizeof(DWORD)), &dwTemp, sizeof(dwTemp), NULL))
								break;
 
							lpLoadLibraryW = reinterpret_cast<PBYTE>(me32.hModule) + dwTemp;
						}
					}
 
					delete[] lpAddressOfNames;
 
					break;
				}
			} while (Module32Next(hModuleSnap, &me32));
 
			// clean the snapshot object
			CloseHandle(hModuleSnap);
		}
		else
			lpLoadLibraryW = GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryW");
 
		if (lpLoadLibraryW)
		{
			nLength = (INT)(wcslen(lpcwszDll) * sizeof(WCHAR));
 
			// allocate mem for dll name
			lpRemoteString = VirtualAllocEx(hProcess, NULL, nLength + 1, MEM_COMMIT, PAGE_READWRITE);
			if (!lpRemoteString)
			{
				//_tprintf(TEXT("VirtualAllocEx errorcode = %d\n"), GetLastError());
				// clean the snapshot object
				CloseHandle(hModuleSnap);
 
				// close process handle
				CloseHandle(hProcess);
 
				return FALSE;
			}
 
			// write dll name
			WriteProcessMemory(hProcess, lpRemoteString, lpcwszDll, nLength, NULL);
 
			// call loadlibraryw
			HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibraryW, lpRemoteString, NULL, NULL);
 
			WaitForSingleObject(hThread, 4000);
 
			// free mem
			VirtualFreeEx(hProcess, lpRemoteString, 0, MEM_RELEASE);
 
			return TRUE;
		}
 
		// close process handle
		CloseHandle(hProcess);
 
		return FALSE;
	}

	bool GetProcessIdFromProcessName(const wchar_t *pwszProcessName, DWORD *pdwProcessId, DWORD dwExceptionProcId)
	{
		bool bResult = false;
		HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if(!hProcessSnapShot)
			return false;

		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(ProcessEntry);

		if(Process32First(hProcessSnapShot, &ProcessEntry))
		{
			while(Process32Next(hProcessSnapShot, &ProcessEntry))
			{
				if(!wcscmp(ProcessEntry.szExeFile, pwszProcessName) && (dwExceptionProcId != 0xFFFFFFFF ? (dwExceptionProcId != ProcessEntry.th32ProcessID) : 1))
				{
					if(pdwProcessId)
					{
						*pdwProcessId = ProcessEntry.th32ProcessID;
					}

					bResult = true;
					break;
				}
			}
		}

		CloseHandle(hProcessSnapShot); 
		return bResult;
	}

	bool IsProcessRunning(const wchar_t *pwszProcessName)
	{
		return GetProcessIdFromProcessName(pwszProcessName, NULL);
	}

	bool KillProcess(const wchar_t *pwszProcessName)
	{
		// Check if the process is running
		DWORD dwProcessId = 0;

		if(GetProcessIdFromProcessName(pwszProcessName, &dwProcessId))
		{
			// Attempt to open a handle to the process
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);

			// Did the process handle open successfully?
			if(hProcess)
			{
				// Attempt to terminate the process
				if(TerminateProcess(hProcess, 0))
				{
					// Process terminated
					return true;
				}
			}
		}

		return false;
	}

	bool StripPath1(wchar_t *pwszString)
	{
		for(size_t i = wcslen(pwszString); i > 0; --i)
		{
			if(pwszString[i] == '\\')
			{
				pwszString[i + 1] = '\0';
				return true;
			}
		}

		return false;
	}

	wchar_t * StripPath2(wchar_t *pwszString)
	{
		wchar_t *pwszPath = pwszString;
		wchar_t *pszStrippedPath = (pwszString + wcslen(pwszString));

		while(pszStrippedPath != pwszPath)
		{
			if(*pszStrippedPath == '\\')
				return (pszStrippedPath + 1);

			pszStrippedPath--;
		}

		return pwszPath;
	}

	bool IsFileExists(const wchar_t *pwszFileName)
	{
		return (GetFileAttributes(pwszFileName) != 0xFFFFFFFF && GetLastError() != ERROR_FILE_NOT_FOUND);
	}

	const wchar_t * GetExecutablePath()
	{
		static wchar_t wszExecutablePath[MAX_PATH];

		GetModuleFileName(GetModuleHandle(NULL), wszExecutablePath, MAX_PATH);

		StripPath1(wszExecutablePath);
		return wszExecutablePath;
	}
#endif
};
