#include "Settings.h"
#include "Utilities.h"

Settings settings;

Settings::Settings()
{
	//_MESSAGE(__FUNCTION__);
	//ReadConfig();
}

void Settings::ReadConfig()
{
	_MESSAGE(__FUNCTION__);

	std::string path = GetCurrentDLLDirectory() + "\\SkyrimUncapper.ini";//".\\Data\\SKSE\\Plugin\\SkyrimUncapper.ini";
	//_MESSAGE(path.c_str());
	CSimpleIniA ini;
	ini.LoadFile(path.c_str());
	settingsGeneral.version = ini.GetLongValue("General", "iVersion", 0);
	_MESSAGE("Config version: %d", settingsGeneral.version);
	if (settingsGeneral.version != CONFIG_VERSION)
		SaveConfig(&ini, path);

	settingsGeneral.author = ini.GetValue("General", "iAuthor", "Kassent");

	settingsSkillCaps.clear();
	settingsSkillFormulaCaps.clear();
	settingsSkillExpGainMults.clear();
	settingsLevelSkillExpMults.clear();
	settingsPerksAtLevelUp.clear();
	settingsHealthAtLevelUp.clear();
	settingsStaminaAtLevelUp.clear();
	settingsMagickaAtLevelUp.clear();
	settingsCarryWeightAtHealthLevelUp.clear();
	settingsCarryWeightAtStaminaLevelUp.clear();
	settingsCarryWeightAtMagickaLevelUp.clear();

	settingsSkillCaps.insert({ 0,	ini.GetLongValue("SkillCaps", "iOneHanded",	    100) });
	settingsSkillCaps.insert({ 1,	ini.GetLongValue("SkillCaps", "iTwoHanded",		100) });
	settingsSkillCaps.insert({ 2,	ini.GetLongValue("SkillCaps", "iMarksman",		100) });
	settingsSkillCaps.insert({ 3,	ini.GetLongValue("SkillCaps", "iBlock",		    100) });
	settingsSkillCaps.insert({ 4,	ini.GetLongValue("SkillCaps", "iSmithing",		100) });
	settingsSkillCaps.insert({ 5,	ini.GetLongValue("SkillCaps", "iHeavyArmor",    100) });
	settingsSkillCaps.insert({ 6,	ini.GetLongValue("SkillCaps", "iLightArmor",	100) });
	settingsSkillCaps.insert({ 7,	ini.GetLongValue("SkillCaps", "iPickpocket",	100) });
	settingsSkillCaps.insert({ 8,	ini.GetLongValue("SkillCaps", "iLockPicking",   100) });
	settingsSkillCaps.insert({ 9,	ini.GetLongValue("SkillCaps", "iSneak",			100) });
	settingsSkillCaps.insert({ 10,	ini.GetLongValue("SkillCaps", "iAlchemy",		100) });
	settingsSkillCaps.insert({ 11,	ini.GetLongValue("SkillCaps", "iSpeechCraft",	100) });
	settingsSkillCaps.insert({ 12,	ini.GetLongValue("SkillCaps", "iAlteration",	100) });
	settingsSkillCaps.insert({ 13,	ini.GetLongValue("SkillCaps", "iConjuration",   100) });
	settingsSkillCaps.insert({ 14,	ini.GetLongValue("SkillCaps", "iDestruction",	100) });
	settingsSkillCaps.insert({ 15,	ini.GetLongValue("SkillCaps", "iIllusion",		100) });
	settingsSkillCaps.insert({ 16,	ini.GetLongValue("SkillCaps", "iRestoration",	100) });
	settingsSkillCaps.insert({ 17,	ini.GetLongValue("SkillCaps", "iEnchanting",	100) });

	settingsSkillFormulaCaps.insert({ 0,	ini.GetLongValue("SkillFormulaCaps", "iOneHanded",	    100) });
	settingsSkillFormulaCaps.insert({ 1,	ini.GetLongValue("SkillFormulaCaps", "iTwoHanded",		100) });
	settingsSkillFormulaCaps.insert({ 2,	ini.GetLongValue("SkillFormulaCaps", "iMarksman",		100) });
	settingsSkillFormulaCaps.insert({ 3,	ini.GetLongValue("SkillFormulaCaps", "iBlock",		    100) });
	settingsSkillFormulaCaps.insert({ 4,	ini.GetLongValue("SkillFormulaCaps", "iSmithing",		100) });
	settingsSkillFormulaCaps.insert({ 5,	ini.GetLongValue("SkillFormulaCaps", "iHeavyArmor",		100) });
	settingsSkillFormulaCaps.insert({ 6,	ini.GetLongValue("SkillFormulaCaps", "iLightArmor",		100) });
	settingsSkillFormulaCaps.insert({ 7,	ini.GetLongValue("SkillFormulaCaps", "iPickpocket",		100) });
	settingsSkillFormulaCaps.insert({ 8,	ini.GetLongValue("SkillFormulaCaps", "iLockPicking",    100) });
	settingsSkillFormulaCaps.insert({ 9,	ini.GetLongValue("SkillFormulaCaps", "iSneak",			100) });
	settingsSkillFormulaCaps.insert({ 10,	ini.GetLongValue("SkillFormulaCaps", "iAlchemy",		100) });
	settingsSkillFormulaCaps.insert({ 11,	ini.GetLongValue("SkillFormulaCaps", "iSpeechCraft",	100) });
	settingsSkillFormulaCaps.insert({ 12,	ini.GetLongValue("SkillFormulaCaps", "iAlteration",		100) });
	settingsSkillFormulaCaps.insert({ 13,	ini.GetLongValue("SkillFormulaCaps", "iConjuration",	100) });
	settingsSkillFormulaCaps.insert({ 14,	ini.GetLongValue("SkillFormulaCaps", "iDestruction",	100) });
	settingsSkillFormulaCaps.insert({ 15,	ini.GetLongValue("SkillFormulaCaps", "iIllusion",		100) });
	settingsSkillFormulaCaps.insert({ 16,	ini.GetLongValue("SkillFormulaCaps", "iRestoration",	100) });
	settingsSkillFormulaCaps.insert({ 17,	ini.GetLongValue("SkillFormulaCaps", "iEnchanting",		100) });

	settingsSkillExpGainMults.insert({ 0,	atof(ini.GetValue("SkillExpGainMults", "fOneHanded",	"1.00")) });
	settingsSkillExpGainMults.insert({ 1,	atof(ini.GetValue("SkillExpGainMults", "fTwoHanded",	"1.00")) });
	settingsSkillExpGainMults.insert({ 2,	atof(ini.GetValue("SkillExpGainMults", "fMarksman",		"1.00")) });
	settingsSkillExpGainMults.insert({ 3,	atof(ini.GetValue("SkillExpGainMults", "fBlock",		"1.00")) });
	settingsSkillExpGainMults.insert({ 4,	atof(ini.GetValue("SkillExpGainMults", "fSmithing",		"1.00")) });
	settingsSkillExpGainMults.insert({ 5,	atof(ini.GetValue("SkillExpGainMults", "fHeavyArmor",	"1.00")) });
	settingsSkillExpGainMults.insert({ 6,	atof(ini.GetValue("SkillExpGainMults", "fLightArmor",	"1.00")) });
	settingsSkillExpGainMults.insert({ 7,	atof(ini.GetValue("SkillExpGainMults", "fPickpocket",	"1.00")) });
	settingsSkillExpGainMults.insert({ 8,	atof(ini.GetValue("SkillExpGainMults", "fLockPicking",  "1.00")) });
	settingsSkillExpGainMults.insert({ 9,	atof(ini.GetValue("SkillExpGainMults", "fSneak",		"1.00")) });
	settingsSkillExpGainMults.insert({ 10,	atof(ini.GetValue("SkillExpGainMults", "fAlchemy",		"1.00")) });
	settingsSkillExpGainMults.insert({ 11,	atof(ini.GetValue("SkillExpGainMults", "fSpeechCraft",	"1.00")) });
	settingsSkillExpGainMults.insert({ 12,	atof(ini.GetValue("SkillExpGainMults", "fAlteration",	"1.00")) });
	settingsSkillExpGainMults.insert({ 13,	atof(ini.GetValue("SkillExpGainMults", "fConjuration",	"1.00")) });
	settingsSkillExpGainMults.insert({ 14,	atof(ini.GetValue("SkillExpGainMults", "fDestruction",	"1.00")) });
	settingsSkillExpGainMults.insert({ 15,	atof(ini.GetValue("SkillExpGainMults", "fIllusion",		"1.00")) });
	settingsSkillExpGainMults.insert({ 16,	atof(ini.GetValue("SkillExpGainMults", "fRestoration",	"1.00")) });
	settingsSkillExpGainMults.insert({ 17,	atof(ini.GetValue("SkillExpGainMults", "fEnchanting",	"1.00")) });

	settingsLevelSkillExpMults.insert({ 0,	atof(ini.GetValue("LevelSkillExpMults", "fOneHanded",   "1.00")) });
	settingsLevelSkillExpMults.insert({ 1,	atof(ini.GetValue("LevelSkillExpMults", "fTwoHanded",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 2,	atof(ini.GetValue("LevelSkillExpMults", "fMarksman",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 3,	atof(ini.GetValue("LevelSkillExpMults", "fBlock",	    "1.00")) });
	settingsLevelSkillExpMults.insert({ 4,	atof(ini.GetValue("LevelSkillExpMults", "fSmithing",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 5,	atof(ini.GetValue("LevelSkillExpMults", "fHeavyArmor",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 6,	atof(ini.GetValue("LevelSkillExpMults", "fLightArmor",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 7,	atof(ini.GetValue("LevelSkillExpMults", "fPickpocket",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 8,	atof(ini.GetValue("LevelSkillExpMults", "fLockPicking", "1.00")) });
	settingsLevelSkillExpMults.insert({ 9,	atof(ini.GetValue("LevelSkillExpMults", "fSneak",		"1.00")) });
	settingsLevelSkillExpMults.insert({ 10,	atof(ini.GetValue("LevelSkillExpMults", "fAlchemy",		"1.00")) });
	settingsLevelSkillExpMults.insert({ 11,	atof(ini.GetValue("LevelSkillExpMults", "fSpeechCraft",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 12,	atof(ini.GetValue("LevelSkillExpMults", "fAlteration",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 13,	atof(ini.GetValue("LevelSkillExpMults", "fConjuration",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 14,	atof(ini.GetValue("LevelSkillExpMults", "fDestruction",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 15,	atof(ini.GetValue("LevelSkillExpMults", "fIllusion",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 16,	atof(ini.GetValue("LevelSkillExpMults", "fRestoration",	"1.00")) });
	settingsLevelSkillExpMults.insert({ 17,	atof(ini.GetValue("LevelSkillExpMults", "fEnchanting",	"1.00")) });

	CSimpleIniA::TNamesDepend keys;
	if (ini.GetAllKeys("PerksAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsPerksAtLevelUp.insert({ atoi(element.pItem), atof(ini.GetValue("PerksAtLevelUp", element.pItem,	"1.00")) });
		settingsPerksAtLevelUp.insert({ 1, 1.00f });
	}

	if (ini.GetAllKeys("HealthAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsHealthAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("HealthAtLevelUp", element.pItem,	10) });
		settingsHealthAtLevelUp.insert({ 1, 10 });
	}

	if (ini.GetAllKeys("MagickaAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsMagickaAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("MagickaAtLevelUp", element.pItem,	10) });
		settingsMagickaAtLevelUp.insert({ 1, 10 });
	}

	if (ini.GetAllKeys("StaminaAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsStaminaAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("StaminaAtLevelUp", element.pItem,	10) });
		settingsStaminaAtLevelUp.insert({ 1, 10 });
	}

	if (ini.GetAllKeys("CarryWeightAtHealthLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtHealthLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtHealthLevelUp", element.pItem,	5) });
		settingsCarryWeightAtHealthLevelUp.insert({ 1, 5 });
	}

	if (ini.GetAllKeys("CarryWeightAtStaminaLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtStaminaLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtStaminaLevelUp", element.pItem,	5) });
		settingsCarryWeightAtStaminaLevelUp.insert({ 1, 5 });
	}

	if (ini.GetAllKeys("CarryWeightAtMagickaLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtMagickaLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtMagickaLevelUp", element.pItem,	5) });
		settingsCarryWeightAtMagickaLevelUp.insert({ 1, 5 });
	}
	ini.Reset();

	//std::vector<UInt8> binaryCode = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x80, 0x3D, 0x83, 0x5A, 0x64, 0x02, 0x00, 0x8B, 0xDA, 0x74, 0x7E, 0x48, 0x8B, 0x05, 0x10, 0xE1, 0x61, 0x02, 0x48, 0x89, 0x7C, 0x24, 0x30, 0x48, 0x8B, 0xB8, 0xF0, 0x01, 0x00, 0x00 };
	//BinarySearch(binaryCode);
}

void Settings::SaveConfig(CSimpleIniA* ini,const std::string& path)
{
	_MESSAGE(__FUNCTION__);

	ini->Reset();
	settingsGeneral.version = CONFIG_VERSION;

	ini->SetLongValue("General", "iVersion", settingsGeneral.version, "#Configuration file version, DO NOT CHANGE");
	ini->SetValue("General", "iAuthor", "Kassent", NULL);

	ini->SetLongValue("SkillCaps", "iOneHanded", 100, "#Set the Skill Level Cap.");
	ini->SetLongValue("SkillCaps", "iTwoHanded", 100, NULL);
	ini->SetLongValue("SkillCaps", "iMarksman", 100, NULL);
	ini->SetLongValue("SkillCaps", "iBlock", 100, NULL);
	ini->SetLongValue("SkillCaps", "iSmithing", 100, NULL);
	ini->SetLongValue("SkillCaps", "iHeavyArmor", 100, NULL);
	ini->SetLongValue("SkillCaps", "iLightArmor", 100, NULL);
	ini->SetLongValue("SkillCaps", "iPickpocket", 100, NULL);
	ini->SetLongValue("SkillCaps", "iLockPicking", 100, NULL);
	ini->SetLongValue("SkillCaps", "iSneak", 100, NULL);
	ini->SetLongValue("SkillCaps", "iAlchemy", 100, NULL);
	ini->SetLongValue("SkillCaps", "iSpeechCraft", 100, NULL);
	ini->SetLongValue("SkillCaps", "iAlteration", 100, NULL);
	ini->SetLongValue("SkillCaps", "iConjuration", 100, NULL);
	ini->SetLongValue("SkillCaps", "iDestruction", 100, NULL);
	ini->SetLongValue("SkillCaps", "iIllusion", 100, NULL);
	ini->SetLongValue("SkillCaps", "iRestoration", 100, NULL);
	ini->SetLongValue("SkillCaps", "iEnchanting", 100, NULL);

	ini->SetLongValue("SkillFormulaCaps", "iOneHanded", 100, "#Set the Skill Formula Cap.");
	ini->SetLongValue("SkillFormulaCaps", "iTwoHanded", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iMarksman", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iBlock", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSmithing", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iHeavyArmor", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iLightArmor", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iPickpocket", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iLockPicking", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSneak", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iAlchemy", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSpeechCraft", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iAlteration", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iConjuration", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iDestruction", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iIllusion", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iRestoration", 100, NULL);
	ini->SetLongValue("SkillFormulaCaps", "iEnchanting", 100, NULL);

	ini->SetValue("SkillExpGainMults", "fOneHanded", "1.00", "#Set the Skill Experience Gained Multiplier.");
	ini->SetValue("SkillExpGainMults", "fTwoHanded", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fMarksman", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fBlock", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fSmithing", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fHeavyArmor", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fLightArmor", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fPickpocket", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fLockPicking", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fSneak", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fAlchemy", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fSpeechCraft", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fAlteration", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fConjuration", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fDestruction", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fIllusion", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fRestoration", "1.00", NULL);
	ini->SetValue("SkillExpGainMults", "fEnchanting", "1.00", NULL);

	ini->SetValue("LevelSkillExpMults", "fOneHanded", "1.00", "#Set the Skill Experience to Player's Character Experience Multipliers.");
	ini->SetValue("LevelSkillExpMults", "fTwoHanded", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fMarksman", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fBlock", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fSmithing", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fHeavyArmor", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fLightArmor", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fPickpocket", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fLockPicking", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fSneak", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fAlchemy", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fSpeechCraft", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fAlteration", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fConjuration", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fDestruction", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fIllusion", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fRestoration", "1.00", NULL);
	ini->SetValue("LevelSkillExpMults", "fEnchanting", "1.00", NULL);

	ini->SetValue("PerksAtLevelUp", "1", "1.00", "#Set the number of perks gained at each level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("HealthAtLevelUp", "1", 10, "#Set the number of health gained at each level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("MagickaAtLevelUp", "1", 10, "#Set the number of magicka gained at each level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("StaminaAtLevelUp", "1", 10, "#Set the number of stamina gained at each level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("CarryWeightAtHealthLevelUp", "1", 0, "#Set the number of carryweight gained at each health level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("CarryWeightAtMagickaLevelUp", "1", 0, "#Set the number of carryweight gained at each magicka level up. If a specific level is not specified then the closest lower level setting is used.");

	ini->SetLongValue("CarryWeightAtStaminaLevelUp", "1", 5, "#Set the number of carryweight gained at each stamina level up. If a specific level is not specified then the closest lower level setting is used.");

#ifdef INVALID_CODE
	std::string path = ".\\Data\\SKSE\\Plugin\\SkyrimUncapper.ini";
	CreateDirectory(".\\Data", NULL);
	CreateDirectory(".\\Data\\SKSE", NULL);
	CreateDirectory(".\\Data\\SKSE\\Plugin", NULL);
#endif
	ini->SaveFile(path.c_str());
}
