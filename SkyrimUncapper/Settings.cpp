#include "Settings.h"
#include "Utilities.h"

#define ReadFloatLevelListSection(destination , section) 																	\
			if (ini.GetAllKeys(##section##, keys))																			\
			{																												\
			for (auto& element : keys){																						\
				##destination##.insert({ atoi(element.pItem), atof(ini.GetValue(##section##, element.pItem,	"1.00")) });}	\
			}																												\
			##destination##.insert({ 1, 1.00f })																			\

#define SaveFloatLevelListSection(source, section)																			\
			for (const auto& pair : ##source##){																			\
				char key[0x8];																								\
				char value[0x10];																							\
				sprintf_s(key, "%d", pair.first);																			\
				sprintf_s(value, "%.2f", pair.second);																		\
				ini->SetValue(##section##, key, value, NULL);																\
			}																												\

Settings settings;

Settings::Settings()
{

}

void Settings::ReadConfig()
{
	std::string path = GetCurrentDLLDirectory() + "\\SkyrimUncapper.ini";
	CSimpleIniA ini;
	ini.LoadFile(path.c_str());

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

	settingsGeneral.version = ini.GetLongValue("General", "Version", 0);
	settingsGeneral.author = ini.GetValue("General", "Author", "Kassent");
	_MESSAGE("INI version: %d", settingsGeneral.version);

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

	CSimpleIniA::TNamesDepend keys;

	for (size_t index = 0; index < kNumAdvanceableSkills; ++index)
	{
		switch (index)
		{
		case 0:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\OneHanded");
			break;
		case 1:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\TwoHanded");
			break;
		case 2:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Marksman");
			break;
		case 3:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Block");
			break;
		case 4:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Smithing");
			break;
		case 5:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\HeavyArmor");
			break;
		case 6:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\LightArmor");
			break;
		case 7:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Pickpocket");
			break;
		case 8:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\LockPicking");
			break;
		case 9:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Sneak");
			break;
		case 10:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Alchemy");
			break;
		case 11:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\SpeechCraft");
			break;
		case 12:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Alteration");
			break;
		case 13:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Conjuration");
			break;
		case 14:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Destruction");
			break;
		case 15:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Illusion");
			break;
		case 16:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Restoration");
			break;
		case 17:
			ReadFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Enchanting");
			break;
		default:
			_MESSAGE("Unknown skill index");
		}
	}

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

	for (size_t index = 0; index < kNumAdvanceableSkills; ++index)
	{
		switch (index)
		{
		case 0:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\OneHanded");
			break;
		case 1:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\TwoHanded");
			break;
		case 2:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Marksman");
			break;
		case 3:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Block");
			break;
		case 4:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Smithing");
			break;
		case 5:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\HeavyArmor");
			break;
		case 6:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\LightArmor");
			break;
		case 7:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Pickpocket");
			break;
		case 8:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\LockPicking");
			break;
		case 9:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Sneak");
			break;
		case 10:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Alchemy");
			break;
		case 11:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\SpeechCraft");
			break;
		case 12:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Alteration");
			break;
		case 13:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Conjuration");
			break;
		case 14:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Destruction");
			break;
		case 15:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Illusion");
			break;
		case 16:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Restoration");
			break;
		case 17:
			ReadFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Enchanting");
			break;
		default:
			_MESSAGE("Unknown skill index");
		}
	}

	if (ini.GetAllKeys("PerksAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsPerksAtLevelUp.insert({ atoi(element.pItem), atof(ini.GetValue("PerksAtLevelUp", element.pItem,	"1.00")) });
	}
	settingsPerksAtLevelUp.insert({ 1, 1.00f });

	if (ini.GetAllKeys("HealthAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsHealthAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("HealthAtLevelUp", element.pItem,	10) });
	}
	settingsHealthAtLevelUp.insert({ 1, 10 });

	if (ini.GetAllKeys("MagickaAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsMagickaAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("MagickaAtLevelUp", element.pItem,	10) });
	}
	settingsMagickaAtLevelUp.insert({ 1, 10 });

	if (ini.GetAllKeys("StaminaAtLevelUp", keys))
	{
		for (auto& element : keys)
			settingsStaminaAtLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("StaminaAtLevelUp", element.pItem,	10) });
	}
	settingsStaminaAtLevelUp.insert({ 1, 10 });

	if (ini.GetAllKeys("CarryWeightAtHealthLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtHealthLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtHealthLevelUp", element.pItem,	5) });
	}
	settingsCarryWeightAtHealthLevelUp.insert({ 1, 0 });

	if (ini.GetAllKeys("CarryWeightAtStaminaLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtStaminaLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtStaminaLevelUp", element.pItem,	5) });
	}
	settingsCarryWeightAtStaminaLevelUp.insert({ 1, 5 });

	if (ini.GetAllKeys("CarryWeightAtMagickaLevelUp", keys))
	{
		for (auto& element : keys)
			settingsCarryWeightAtMagickaLevelUp.insert({ atoi(element.pItem), ini.GetLongValue("CarryWeightAtMagickaLevelUp", element.pItem,	5) });
	}
	settingsCarryWeightAtMagickaLevelUp.insert({ 1, 0 });

	settings.settingsLegendarySkill.bLegendaryKeepSkillLevel = ini.GetBoolValue("LegendarySkill", "bLengenaryKeepSkillLevel", false);
	settings.settingsLegendarySkill.bHideLegendaryButton = ini.GetBoolValue("LegendarySkill", "bHideLegendaryButton", true);
	settings.settingsLegendarySkill.iSkillLevelEnableLegendary = ini.GetLongValue("LegendarySkill", "iSkillLevelEnableLegendary", 100);
	settings.settingsLegendarySkill.iSkillLevelAfterLengenary = ini.GetLongValue("LegendarySkill", "iSkillLevelAfterLengenary", 0);

	if (settingsGeneral.version != CONFIG_VERSION)
	{
		_MESSAGE("INI file is outdated, try to update INI file...");
		SaveConfig(&ini, path);
	}

	ini.Reset();
}

void Settings::SaveConfig(CSimpleIniA* ini,const std::string& path)
{
	ini->Reset();
	settingsGeneral.version = CONFIG_VERSION;

	ini->SetLongValue("General", "Version", CONFIG_VERSION, "#Configuration file version, DO NOT CHANGE");
	ini->SetValue("General", "Author", "Kassent", NULL);

	ini->SetLongValue("SkillCaps", "iOneHanded", settings.settingsSkillCaps[0], "#Set the Skill Level Cap.");
	ini->SetLongValue("SkillCaps", "iTwoHanded", settings.settingsSkillCaps[1], NULL);
	ini->SetLongValue("SkillCaps", "iMarksman", settings.settingsSkillCaps[2], NULL);
	ini->SetLongValue("SkillCaps", "iBlock", settings.settingsSkillCaps[3], NULL);
	ini->SetLongValue("SkillCaps", "iSmithing", settings.settingsSkillCaps[4], NULL);
	ini->SetLongValue("SkillCaps", "iHeavyArmor", settings.settingsSkillCaps[5], NULL);
	ini->SetLongValue("SkillCaps", "iLightArmor", settings.settingsSkillCaps[6], NULL);
	ini->SetLongValue("SkillCaps", "iPickpocket", settings.settingsSkillCaps[7], NULL);
	ini->SetLongValue("SkillCaps", "iLockPicking", settings.settingsSkillCaps[8], NULL);
	ini->SetLongValue("SkillCaps", "iSneak", settings.settingsSkillCaps[9], NULL);
	ini->SetLongValue("SkillCaps", "iAlchemy", settings.settingsSkillCaps[10], NULL);
	ini->SetLongValue("SkillCaps", "iSpeechCraft", settings.settingsSkillCaps[11], NULL);
	ini->SetLongValue("SkillCaps", "iAlteration", settings.settingsSkillCaps[12], NULL);
	ini->SetLongValue("SkillCaps", "iConjuration", settings.settingsSkillCaps[13], NULL);
	ini->SetLongValue("SkillCaps", "iDestruction", settings.settingsSkillCaps[14], NULL);
	ini->SetLongValue("SkillCaps", "iIllusion", settings.settingsSkillCaps[15], NULL);
	ini->SetLongValue("SkillCaps", "iRestoration", settings.settingsSkillCaps[16], NULL);
	ini->SetLongValue("SkillCaps", "iEnchanting", settings.settingsSkillCaps[17], NULL);

	ini->SetLongValue("SkillFormulaCaps", "iOneHanded", settings.settingsSkillFormulaCaps[0], "#Set the Skill Formula Cap.");
	ini->SetLongValue("SkillFormulaCaps", "iTwoHanded", settings.settingsSkillFormulaCaps[1], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iMarksman", settings.settingsSkillFormulaCaps[2], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iBlock", settings.settingsSkillFormulaCaps[3], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSmithing", settings.settingsSkillFormulaCaps[4], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iHeavyArmor", settings.settingsSkillFormulaCaps[5], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iLightArmor", settings.settingsSkillFormulaCaps[6], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iPickpocket", settings.settingsSkillFormulaCaps[7], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iLockPicking", settings.settingsSkillFormulaCaps[8], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSneak", settings.settingsSkillFormulaCaps[9], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iAlchemy", settings.settingsSkillFormulaCaps[10], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iSpeechCraft", settings.settingsSkillFormulaCaps[11], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iAlteration", settings.settingsSkillFormulaCaps[12], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iConjuration", settings.settingsSkillFormulaCaps[13], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iDestruction", settings.settingsSkillFormulaCaps[14], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iIllusion", settings.settingsSkillFormulaCaps[15], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iRestoration", settings.settingsSkillFormulaCaps[16], NULL);
	ini->SetLongValue("SkillFormulaCaps", "iEnchanting", settings.settingsSkillFormulaCaps[17], NULL);

	char buffer[0x10];

	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[0]);
	ini->SetValue("SkillExpGainMults", "fOneHanded", buffer, "#Set the Skill Experience Gained Multiplier.");
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[1]);
	ini->SetValue("SkillExpGainMults", "fTwoHanded", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[2]);
	ini->SetValue("SkillExpGainMults", "fMarksman", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[3]);
	ini->SetValue("SkillExpGainMults", "fBlock", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[4]);
	ini->SetValue("SkillExpGainMults", "fSmithing", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[5]);
	ini->SetValue("SkillExpGainMults", "fHeavyArmor", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[6]);
	ini->SetValue("SkillExpGainMults", "fLightArmor", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[7]);
	ini->SetValue("SkillExpGainMults", "fPickpocket", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[8]);
	ini->SetValue("SkillExpGainMults", "fLockPicking", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[9]);
	ini->SetValue("SkillExpGainMults", "fSneak", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[10]);
	ini->SetValue("SkillExpGainMults", "fAlchemy", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[11]);
	ini->SetValue("SkillExpGainMults", "fSpeechCraft", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[12]);
	ini->SetValue("SkillExpGainMults", "fAlteration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[13]);
	ini->SetValue("SkillExpGainMults", "fConjuration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[14]);
	ini->SetValue("SkillExpGainMults", "fDestruction", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[15]);
	ini->SetValue("SkillExpGainMults", "fIllusion", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[16]);
	ini->SetValue("SkillExpGainMults", "fRestoration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsSkillExpGainMults[17]);
	ini->SetValue("SkillExpGainMults", "fEnchanting", buffer, NULL);

	ini->SetValue("SkillExpGainMults\\OneHanded", NULL, NULL, "#All the subsections of SkillExpGainMults below allow to set an additional multiplier depending on the base skill level, independantly for each skill. ");
	for (size_t index = 0; index < kNumAdvanceableSkills; ++index)
	{
		switch (index)
		{
		case 0:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\OneHanded");
			break;
		case 1:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\TwoHanded");
			break;
		case 2:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Marksman");
			break;
		case 3:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Block");
			break;
		case 4:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Smithing");
			break;
		case 5:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\HeavyArmor");
			break;
		case 6:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\LightArmor");
			break;
		case 7:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Pickpocket");
			break;
		case 8:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\LockPicking");
			break;
		case 9:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Sneak");
			break;
		case 10:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Alchemy");
			break;
		case 11:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\SpeechCraft");
			break;
		case 12:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Alteration");
			break;
		case 13:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Conjuration");
			break;
		case 14:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Destruction");
			break;
		case 15:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Illusion");
			break;
		case 16:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Restoration");
			break;
		case 17:
			SaveFloatLevelListSection(settingsSkillExpGainMultsWithSkills[index], "SkillExpGainMults\\Enchanting");
			break;
		default:
			_MESSAGE("Unknown skill index");
		}
	}

	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[0]);
	ini->SetValue("LevelSkillExpMults", "fOneHanded", buffer, "#Set the Skill Experience to Player's Character Experience Multipliers.");
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[1]);
	ini->SetValue("LevelSkillExpMults", "fTwoHanded", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[2]);
	ini->SetValue("LevelSkillExpMults", "fMarksman", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[3]);
	ini->SetValue("LevelSkillExpMults", "fBlock", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[4]);
	ini->SetValue("LevelSkillExpMults", "fSmithing", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[5]);
	ini->SetValue("LevelSkillExpMults", "fHeavyArmor", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[6]);
	ini->SetValue("LevelSkillExpMults", "fLightArmor", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[7]);
	ini->SetValue("LevelSkillExpMults", "fPickpocket", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[8]);
	ini->SetValue("LevelSkillExpMults", "fLockPicking", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[9]);
	ini->SetValue("LevelSkillExpMults", "fSneak", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[10]);
	ini->SetValue("LevelSkillExpMults", "fAlchemy", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[11]);
	ini->SetValue("LevelSkillExpMults", "fSpeechCraft", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[12]);
	ini->SetValue("LevelSkillExpMults", "fAlteration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[13]);
	ini->SetValue("LevelSkillExpMults", "fConjuration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[14]);
	ini->SetValue("LevelSkillExpMults", "fDestruction", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[15]);
	ini->SetValue("LevelSkillExpMults", "fIllusion", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[16]);
	ini->SetValue("LevelSkillExpMults", "fRestoration", buffer, NULL);
	sprintf_s(buffer, "%.2f", settings.settingsLevelSkillExpMults[17]);
	ini->SetValue("LevelSkillExpMults", "fEnchanting", buffer, NULL);

	ini->SetValue("LevelSkillExpMults\\OneHanded", NULL, NULL, "#All the subsections of PCLevelSkillExpMults below allow to set an additional multipliers depending on the base skill level, independantly for each skill.");
	for (size_t index = 0; index < kNumAdvanceableSkills; ++index)
	{
		switch (index)
		{
		case 0:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\OneHanded");
			break;
		case 1:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\TwoHanded");
			break;
		case 2:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Marksman");
			break;
		case 3:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Block");
			break;
		case 4:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Smithing");
			break;
		case 5:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\HeavyArmor");
			break;
		case 6:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\LightArmor");
			break;
		case 7:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Pickpocket");
			break;
		case 8:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\LockPicking");
			break;
		case 9:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Sneak");
			break;
		case 10:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Alchemy");
			break;
		case 11:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\SpeechCraft");
			break;
		case 12:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Alteration");
			break;
		case 13:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Conjuration");
			break;
		case 14:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Destruction");
			break;
		case 15:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Illusion");
			break;
		case 16:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Restoration");
			break;
		case 17:
			SaveFloatLevelListSection(settingsLevelSkillExpMultsWithSkills[index], "LevelSkillExpMults\\Enchanting");
			break;
		default:
			_MESSAGE("Unknown skill index");
		}
	}

	for (const auto& pair : settings.settingsPerksAtLevelUp)
	{
		char key[0x8];
		char value[0x10];
		sprintf_s(key, "%d", pair.first);
		sprintf_s(value, "%.2f", pair.second);
		if (pair.first == 1)
			ini->SetValue("PerksAtLevelUp", key, value, "#Set the number of perks gained at each level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetValue("PerksAtLevelUp", key, value, NULL);
	}

	for (const auto& pair : settings.settingsHealthAtLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("HealthAtLevelUp", key, pair.second, "#Set the number of health gained at each level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("HealthAtLevelUp", key, pair.second, NULL);
	}

	for (const auto& pair : settings.settingsMagickaAtLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("MagickaAtLevelUp", key, pair.second, "#Set the number of magicka gained at each level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("MagickaAtLevelUp", key, pair.second, NULL);
	}

	for (const auto& pair : settings.settingsStaminaAtLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("StaminaAtLevelUp", key, pair.second, "#Set the number of Stamina gained at each level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("StaminaAtLevelUp", key, pair.second, NULL);
	}

	for (const auto& pair : settings.settingsCarryWeightAtHealthLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("CarryWeightAtHealthLevelUp", key, pair.second, "#Set the number of carryweight gained at each health level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("CarryWeightAtHealthLevelUp", key, pair.second, NULL);
	}

	for (const auto& pair : settings.settingsCarryWeightAtMagickaLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("CarryWeightAtMagickaLevelUp", key, pair.second, "#Set the number of carryweight gained at each magicka level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("CarryWeightAtMagickaLevelUp", key, pair.second, NULL);
	}

	for (const auto& pair : settings.settingsCarryWeightAtStaminaLevelUp)
	{
		char key[0x8];
		sprintf_s(key, "%d", pair.first);
		if (pair.first == 1)
			ini->SetLongValue("CarryWeightAtStaminaLevelUp", key, pair.second, "#Set the number of carryweight gained at each stamina level up. If a specific level is not specified then the closest lower level setting is used.");
		else
			ini->SetLongValue("CarryWeightAtStaminaLevelUp", key, pair.second, NULL);
	}

	ini->SetBoolValue("LegendarySkill", "bLengenaryKeepSkillLevel", settings.settingsLegendarySkill.bLegendaryKeepSkillLevel, "#This option determines whether the legendary feature will reset the skill level.Set this option to true will make option \"iSkillLevelAfterLengenary\" have no effect.");
	ini->SetBoolValue("LegendarySkill", "bHideLegendaryButton", settings.settingsLegendarySkill.bHideLegendaryButton, "#This option determines whether to display the Legendary button in state menu when you meet the requirements of legendary skills.");
	ini->SetLongValue("LegendarySkill", "iSkillLevelEnableLegendary", settings.settingsLegendarySkill.iSkillLevelEnableLegendary, "#This option determines the skill level required to make a skill Legendary.");
	ini->SetLongValue("LegendarySkill", "iSkillLevelAfterLengenary", settings.settingsLegendarySkill.iSkillLevelAfterLengenary, "#This option determines the level of a skill after making this skill Legendary.Set this option to 0 will reset the skill level to default level.");

#ifdef INVALID_CODE
	std::string path = ".\\Data\\SKSE\\Plugin\\SkyrimUncapper.ini";
	CreateDirectory(".\\Data", NULL);
	CreateDirectory(".\\Data\\SKSE", NULL);
	CreateDirectory(".\\Data\\SKSE\\Plugin", NULL);
#endif

	ini->SaveFile(path.c_str());

	_MESSAGE("Finish updating ini file...");
}
