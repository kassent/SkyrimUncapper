#pragma once

#include "SimpleIni.h"
#include <map>
#include <string>
#include <algorithm>

#define CONFIG_VERSION 2
#define REQUIRED_VERSION "1.4.2.0"

template<typename T>
struct SettingList : public std::map < UInt32, T>
{
	typename SettingList::mapped_type GetValue(const typename SettingList::key_type& index)
	{
		typename SettingList::mapped_type result = NULL;
		for (auto it = rbegin(); it != rend(); ++it)
		{
			if (it->first <= index)
			{
				result = it->second;
				break;
			}
		}
		return result;
	}

	float GetDecimal(const typename SettingList::key_type& index)
	{
		float decimal = 0.00f;
		for (auto iterator = rbegin(); iterator != rend(); ++iterator)
		{
			if (iterator->first < index)
			{
				if (iterator != --rend())
				{
					decimal += (index - iterator->first) * (iterator->second);
					auto it = iterator;
					++it;
					for (; it != rend(); ++it)
					{
						auto nextIterator = it;
						--nextIterator;
						if (it != --rend())
							decimal += (nextIterator->first - it->first) * (it->second);
						else
							decimal += (nextIterator->first - it->first - 1) * (it->second);
					}
				}
				else
					decimal += (index - iterator->first - 1) * (iterator->second);
				break;
			}
		}
		return decimal - static_cast<UInt32>(decimal);
	}
};

struct SettingsGeneral
{
	UInt32				version;
	std::string			author;
};

struct SettingsLegenarySkill
{
	bool				bLegenaryKeepSkillLevel;
	bool				bHideLegendaryButton;
	UInt32				iSkillLevelEnableLegenary;
	UInt32				iSkillLevelAfterLengenary;
};

struct Settings
{
	Settings();
	void ReadConfig();
	void SaveConfig(CSimpleIniA* ini, const std::string& path);
	//static Settings* GetSingleton();

	SettingsGeneral				settingsGeneral;
	SettingsLegenarySkill		settingsLegenarySkill;
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