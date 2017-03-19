#pragma once

#include "SimpleIni.h"
#include <map>
#include <string>
#include <algorithm>

#define CONFIG_VERSION 3

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

struct SettingsLegendarySkill
{
	bool				bLegendaryKeepSkillLevel;
	bool				bHideLegendaryButton;
	UInt32				iSkillLevelEnableLegendary;
	UInt32				iSkillLevelAfterLengenary;
};

struct Settings
{
	enum {
		kAdvanceableSkillOffset = 6,
		kNumAdvanceableSkills = 18
	};

	Settings();
	void ReadConfig();
	void SaveConfig(CSimpleIniA* ini, const std::string& path);

	SettingsGeneral				settingsGeneral;
	SettingsLegendarySkill		settingsLegendarySkill;
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
	SettingList<float>			settingsSkillExpGainMultsWithSkills[kNumAdvanceableSkills];
	SettingList<float>			settingsLevelSkillExpMultsWithSkills[kNumAdvanceableSkills];
};

extern Settings settings;
