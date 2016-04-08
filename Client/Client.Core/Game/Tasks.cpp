#include "Main.h"

std::wstring getTaskNameById(uint16_t taskId)
{
	std::wstring taskName = L"Unknown";

	auto pair = taskMap.find(taskId);

	if(pair != taskMap.end())
	{
		taskName = pair->second;
	}

	return taskName;
}

void addTaskListEntry(wchar_t *pwszTaskListString, size_t taskListStringLength, uint64_t pTask)
{
	uint16_t taskId = *(uint16_t *)(pTask + 0x34);
	std::wstring taskName = getTaskNameById(taskId);

	wchar_t wszTaskListEntry[128];
	swprintf_s(wszTaskListEntry, 128, L"   %ls (%d)\n", taskName.c_str(), taskId);
	wcscat_s(pwszTaskListString, taskListStringLength, wszTaskListEntry);

	uint64_t pChildTask = *(uint64_t *)(pTask + 0x20);
	uint32_t itemCount = 0;

	while(pChildTask != NULL)
	{
		uint64_t pCurrentChildTask = pChildTask;

		taskId = *(uint16_t *)(pChildTask + 0x34);
		std::wstring taskName = getTaskNameById(taskId);

		itemCount++;

		for(uint32_t i = 0; i < itemCount; i++)
		{
			wcscat_s(pwszTaskListString, taskListStringLength, L"   ");
		}

		swprintf_s(wszTaskListEntry, 128, L"   %ls (%d)\n", taskName.c_str(), taskId);
		wcscat_s(pwszTaskListString, taskListStringLength, wszTaskListEntry);

		pChildTask = *(uint64_t *)(pCurrentChildTask + 0x20);
	}
}

void getTaskTreeInfo(wchar_t *pwszTaskListString, size_t taskListStringLength, uint64_t pTaskTree)
{
	int32_t taskTreeCapacity = *(int32_t *)(pTaskTree + 0x48);

	if(taskTreeCapacity > 0)
	{
		for(int32_t i = 0; i < taskTreeCapacity; i++)
		{
			uint64_t pTask = *(uint64_t *)(pTaskTree + 0x20 + i * 0x08);

			if(pTask != NULL)
			{
				addTaskListEntry(pwszTaskListString, taskListStringLength, pTask);
			}
		}
	}
}

extern uint64_t g_pClonePed;

void drawTaskList(ID3D11DeviceContext *pD3D11DeviceContext, IFW1FontWrapper *pFW1FontWrapper)
{
	if(GameStateWatcher::GetGameState() == GameStatePlaying && GameAddresses::ppPedFactory != NULL && GameAddresses::pedIntelligenceOffset != 0)
	{
		uint64_t pPedFactory = *(uint64_t *)(GameAddresses::ppPedFactory);

		if(pPedFactory != NULL)
		{
			uint64_t pLocalPlayerPed = *(uint64_t *)(pPedFactory + 8);

			if(pLocalPlayerPed != NULL)
			{
				uint64_t pPedIntelligence = *(uint64_t *)(pLocalPlayerPed + GameAddresses::pedIntelligenceOffset);
				
				if(pPedIntelligence != NULL)
				{
					uint64_t pTaskTreePed = *(uint64_t *)(pPedIntelligence + 0x360);
					uint64_t pTaskTreeMovement = *(uint64_t *)(pPedIntelligence + 0x370);
					uint64_t pTaskTreeMotion = *(uint64_t *)(pPedIntelligence + 0x378);

					wchar_t wszTaskListString[4096];
					swprintf_s(wszTaskListString, 4096, L"\nTasks:\n");

					if(pTaskTreePed != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreePed); }
					if(pTaskTreeMovement != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreeMovement); }
					if(pTaskTreeMotion != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreeMotion); }

					if(g_pClonePed != NULL)
					{
						wcscat_s(wszTaskListString, 4096, L"\nClone Ped Tasks:\n");

						pPedIntelligence = *(uint64_t *)(g_pClonePed + GameAddresses::pedIntelligenceOffset);

						pTaskTreePed = *(uint64_t *)(pPedIntelligence + 0x360);
						pTaskTreeMovement = *(uint64_t *)(pPedIntelligence + 0x370);
						pTaskTreeMotion = *(uint64_t *)(pPedIntelligence + 0x378);

						if(pTaskTreePed != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreePed); }
						if(pTaskTreeMovement != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreeMovement); }
						if(pTaskTreeMotion != NULL) { getTaskTreeInfo(wszTaskListString, 4096, pTaskTreeMotion); }
					}

					pFW1FontWrapper->DrawString(pD3D11DeviceContext, wszTaskListString, 18.0f, 22.0f, 64.0f, 0xff000000, FW1_RESTORESTATE);
					pFW1FontWrapper->DrawString(pD3D11DeviceContext, wszTaskListString, 18.0f, 20.0f, 62.0f, 0xffffffff, FW1_RESTORESTATE);
				}
			}
		}
	}
}
