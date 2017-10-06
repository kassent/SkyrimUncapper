//#include "GLobalVariables.h"
//#include "f4se_common/Relocation.h"
//#include "f4se_common/f4se_version.h"
//
//#define GET_RVA(relocPtr) relocPtr.GetUIntPtr() - RelocationManager::s_baseAddr
//
///*
//This file makes globals version-independent.
//Initialization order is important for this file.
//Since RelocPtrs are static globals with constructors they are initialized during the dynamic initialization phase.
//Static initialization order is undefined for variables in different translation units, so it is not possible to deterministically obtain the value of a RelocPtr during static init.
//Initialization must thus be done explicitly:
//Call G::Init() in the plugin load routine before calling RVAManager::UpdateAddresses().
//Doing so ensures that all RelocPtrs have been initialized and can be used to initialize an RVA.
//*/
//
//#include "f4se/GameData.h"
//#include "f4se/GameInput.h"
//#include "f4se/GameMenus.h"
//#include "f4se/PapyrusVM.h"
//#include "f4se/ScaleformLoader.h"
//
//namespace GLobalVariables
//{
//	void Init()
//	{
//		g_scaleformManager	= RVA<BSScaleformManager**>(g_scaleformManager.GetUIntPtr(), "48 8B 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 8B D3", 0, 3, 7);
//		g_menuControls		= RVA<MenuControls**>(g_menuControls.GetUIntPtr(), "48 8B 0D ? ? ? ? E8 ? ? ? ? 80 3D ? ? ? ? ? 0F B6 F8", 0, 3, 7);
//		g_ui				= RVA<UI**>(g_ui.GetUIntPtr(), "48 8B 0D ? ? ? ? BA ? ? ? ? 8B 1C 16", 0, 3, 7);
//		g_inputMgr			= RVA<InputManager**>(g_inputMgr.GetUIntPtr(), "48 83 3D ? ? ? ? ? 74 3F 48 83 C1 40", 0, 3, 8);
//		g_gameVM			= RVA<GameVM**>(g_gameVM.GetUIntPtr(), "4C 8B 05 ? ? ? ? 48 8B F9", 0, 3, 7);
//		g_dataHandler		= RVA<DataHandler**>(g_dataHandler.GetUIntPtr(), "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7);
//	}
//}