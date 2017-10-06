#include <ShlObj.h>
#include "skse64_common/Relocation.h"
#include "skse64_common/BranchTrampoline.h"
#include "Hook_Skill.h"
#include "Settings.h"
#include "Utilities.h"
#include "PluginAPI.h"

#ifdef _DEBUG
#include "ScanMemory.h"
#endif

#define GAME_EXE_NAME		"SkyrimSE.exe"

IDebugLog gLog;
void * g_moduleHandle = nullptr;


UInt32	g_pluginHandle = kPluginHandle_Invalid;


void SkyrimUncapper_Initialize(void)
{
	static bool isInit = false;
	if (isInit) return;
	isInit = true;

	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SkyrimUncapper\\SkyrimUncapper.log");

	_MESSAGE("imagebase = %016I64X ===============", GetModuleHandle(NULL));
	_MESSAGE("reloc mgr imagebase = %016I64X ==============", RelocationManager::s_baseAddr);

	std::string version;

	if (GetFileVersion(GAME_EXE_NAME, version))
	{

	}
	else
	{
		MessageBox(NULL, "Can't access SkyrimSE.exe's version info, SkyrimUncapper won't be initialized", "SkyrimUncapper", MB_OK);
		return;
	}

	if (!g_branchTrampoline.Create(1024 * 64))
	{
		_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
		return;
	}

	if (!g_localTrampoline.Create(1024 * 64, g_moduleHandle))
	{
		_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
		return;
	}
	settings.ReadConfig();

	Hook_Skill_Commit();

	_MESSAGE("Init complete");
}


extern "C" {

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
	{
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_moduleHandle = (void *)hDllHandle;
			SkyrimUncapper_Initialize();
			break;

		case DLL_PROCESS_DETACH:
			break;
		};

		return TRUE;
	}

	bool StartSkyrimUncapper(void)
	{
		g_moduleHandle = reinterpret_cast<void *>(GetModuleHandleA("SkyrimUncapper.dll"));
		SkyrimUncapper_Initialize();
		return true;
	}

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
	{

		g_moduleHandle = reinterpret_cast<void *>(GetModuleHandleA("SkyrimUncapper.dll"));

		SkyrimUncapper_Initialize();

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "SkyrimUncapper";
		info->version = 1;

		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor)
		{
			_ERROR("loaded in editor, marking as incompatible");

			return false;
		}
		return true;
	}

	bool SKSEPlugin_Load(const void * skse)
	{
		return true;
	}

	void LoadINIConfig()
	{
#ifdef _DEBUG
		settings.ReadConfig();
#endif
	}
};