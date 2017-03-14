#pragma once

#include "SimpleIni.h"
#include <map>
#include <string>
#include <algorithm>

#define CONFIG_VERSION 1

template<typename T>
struct SettingList : public std::map < UInt32, T>
{
	typename SettingList::mapped_type GetValue(const typename SettingList::key_type& index)
	{
		typename SettingList::mapped_type result = NULL;
		auto it = std::find_if(begin(), end(), [index, &result](const typename SettingList::value_type& param)->bool{
			if (param.first == index)
			{
				result = param.second;
				return true;
			}
			else if ((index > param.first) && (param.first > result))
				result = param.second;
			return false;
		});
		return result;
	}
};

struct SettingsGeneral
{
	UInt32				version;
	std::string			author;
};

struct Settings
{
	Settings();
	void ReadConfig();
	void SaveConfig(CSimpleIniA* ini, const std::string& path);
	//static Settings* GetSingleton();

	SettingsGeneral				settingsGeneral;
	SettingList<UInt32>			settingsSkillCaps;
	SettingList<UInt32>			settingsSkillFormulaCaps;
	SettingList<float>			settingsSkillExpGainMults;
	SettingList<float>			settingsLevelSkillExpMults;
	SettingList<float>			settingsPerksAtLevelUp;
	SettingList<UInt32>			settingsHealthAtLevelUp;
	SettingList<UInt32>			settingsMagickaAtLevelUp;
	SettingList<UInt32>			settingsStaminaAtLevelUp;
	SettingList<UInt32>			settingsCarryWeightAtHealthLevelUp;
	SettingList<UInt32>			settingsCarryWeightAtMagickaLevelUp;
	SettingList<UInt32>			settingsCarryWeightAtStaminaLevelUp;
};

extern Settings settings;

//key检查：正整数，且重复值取value大值。

/*
SkyrimSE.exe+608370 - 40 55                 - push rbp
SkyrimSE.exe+608372 - 56                    - push rsi
SkyrimSE.exe+608373 - 57                    - push rdi
SkyrimSE.exe+608374 - 41 56                 - push r14
*/