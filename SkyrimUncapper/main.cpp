#include <ShlObj.h>
#include "Relocation.h"
#include "BranchTrampoline.h"
#include "Hook_Skill.h"
#include "Settings.h"
#include "Utilities.h"

#define GAME_EXE_NAME		"SkyrimSE.exe"
#define GAME_EXE_VERSION	"1.4.2.0"

IDebugLog gLog;
void * g_moduleHandle = nullptr;

enum
{
	kPluginHandle_Invalid = 0xFFFFFFFF
};

UInt32	g_pluginHandle = kPluginHandle_Invalid;

struct PluginInfo
{
	enum
	{
		kInfoVersion = 1
	};

	UInt32			infoVersion;
	const char *	name;
	UInt32			version;
};

struct SKSEInterface
{
	UInt32	skseVersion;
	UInt32	runtimeVersion;
	UInt32	editorVersion;
	UInt32	isEditor;
	void *	(*QueryInterface)(UInt32 id);

	// call during your Query or Load functions to get a PluginHandle uniquely identifying your plugin
	// invalid if called at any other time, so call it once and save the result
	UInt32	(*GetPluginHandle)(void);

	// returns the SKSE build's release index
	UInt32	(*GetReleaseIndex)(void);
};

void SkyrimUncapper_Initialize(void)
{
	static bool isInit = false;
	if (isInit) return;
	isInit = true;

	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SkyrimUncapper\\SkyrimUncapper.log");

	_MESSAGE("imagebase = %016I64X", GetModuleHandle(NULL));
	_MESSAGE("reloc mgr imagebase = %016I64X", RelocationManager::s_baseAddr);

	std::string version;

	if (GetFileVersion(GAME_EXE_NAME, version))
	{
		char warningInfo[100];
		sprintf_s(warningInfo, "SkyrimSE.exe's current version is unsupported, required version is %s", GAME_EXE_VERSION);
		if (strcmp(version.c_str(), GAME_EXE_VERSION) != NULL)
		{
			MessageBox(NULL, warningInfo, "SkyrimUncapper", MB_OK);
			return;
		}
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
		g_moduleHandle = (void *)::GetModuleHandle("SkyrimUncapper.dll");
		SkyrimUncapper_Initialize();
		return true;
	}

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
	{

		g_moduleHandle = (void *)::GetModuleHandle("SkyrimUncapper.dll");

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
		settings.ReadConfig();
	}
};