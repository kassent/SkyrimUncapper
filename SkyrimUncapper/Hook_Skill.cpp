#include "Hook_Skill.h"
#include "Settings.h"
#include "GameSettings.h"
#include "skse64_common/Relocation.h"
#include "skse64_common/BranchTrampoline.h"
#include "skse64_common/SafeWrite.h"
#include "xbyak.h"

using LevelData = PlayerSkills::StatData::LevelData;

RelocPtr <void*>		g_thePlayer = 0x02F40458;

RelocAddr <uintptr_t *> kHook_ModifyPerkPool_Ent = 0x008C584F;
RelocAddr <uintptr_t *> kHook_ModifyPerkPool_Ret = 0x008C584F + 0x1C; //1C

RelocAddr <uintptr_t *> kHook_SkillCapPatch_Ent = 0x006E4B78;
RelocAddr <uintptr_t *> kHook_SkillCapPatch_Ret = 0x006E4B78 + 0x9;

RelocAddr <uintptr_t *> kHook_ExecuteLegendarySkill_Ent = 0x08C95FD;//get it when legendary skill by trace setbaseav.
RelocAddr <uintptr_t *> kHook_ExecuteLegendarySkill_Ret = 0x08C95FD + 0x6;

RelocAddr <uintptr_t *> kHook_CheckConditionForLegendarySkill_Ent = 0x08BF655;
RelocAddr <uintptr_t *> kHook_CheckConditionForLegendarySkill_Ret = 0x08BF655 + 0x13;

RelocAddr <uintptr_t *> kHook_HideLegendaryButton_Ent = 0x08C1456;
RelocAddr <uintptr_t *> kHook_HideLegendaryButton_Ret = 0x08C1456 + 0x1D;

typedef void(*_ImproveSkillByTraining)(void* pPlayer, UInt32 skillID, UInt32 count);
RelocAddr <_ImproveSkillByTraining> ImproveSkillByTraining_Original = 0x06A0EE0;

typedef void(*_ImprovePlayerSkillPoints)(void*, UInt32, float, UInt64, UInt32, UInt8, bool);
RelocAddr <_ImprovePlayerSkillPoints> ImprovePlayerSkillPoints = 0x06E4B30;
_ImprovePlayerSkillPoints ImprovePlayerSkillPoints_Original = nullptr;

typedef UInt64(*_ImproveAttributeWhenLevelUp)(void*, UInt8);
RelocAddr <_ImproveAttributeWhenLevelUp> ImproveAttributeWhenLevelUp = 0x08925D0;
_ImproveAttributeWhenLevelUp ImproveAttributeWhenLevelUp_Original = nullptr;

typedef bool(*_GetSkillCoefficients)(UInt32, float*, float*, float*, float*);
RelocAddr <_GetSkillCoefficients> GetSkillCoefficients = 0x03E37F0;

typedef UInt16(*_GetLevel)(void* pPlayer);
RelocAddr <_GetLevel> GetLevel = 0x05D4C40;

typedef float(*_ImproveLevelExpBySkillLevel)(float skillLevel);
RelocAddr <_ImproveLevelExpBySkillLevel> ImproveLevelExpBySkillLevel_Original = 0x06E5D90;

typedef float(*_GetCurrentActorValue)(void*, UInt32);
RelocAddr <_GetCurrentActorValue> GetCurrentActorValue = 0x061F6C0;
_GetCurrentActorValue GetCurrentActorValue_Original = nullptr;

typedef float(*_GetBaseActorValue)(void*, UInt32);
RelocAddr <_GetBaseActorValue> GetBaseActorValue = 0x061F890;	//GetBaseActorValue

typedef float(*_CalculateChargePointsPerUse)(float basePoints, float enchantingLevel);
RelocAddr <_CalculateChargePointsPerUse> CalculateChargePointsPerUse_Original = 0x03C0F10;

class ActorValueOwner;
typedef float(*_GetEffectiveSkillLevel)(ActorValueOwner *, UInt32 skillID);
RelocAddr <_GetEffectiveSkillLevel> GetEffectiveSkillLevel = 0x03E5400;	//V1.5.3

//typedef float(*_Pow)(float, float);
//RelocAddr <_Pow> Pow = 0x01312C52;


//#include <cmath>
float CalculateSkillExpForLevel(UInt32 skillID, float skillLevel)
{
#ifdef _DEBUG
	_MESSAGE("function:%s, skillId:%d, skillLevel:%.2f", __FUNCTION__, skillID, skillLevel);
#endif
	float result = 0.0f;
	float fSkillUseCurve = 1.95f;//0x01D88258;
	if ((*g_gameSettingCollection) != nullptr)
	{
		auto pSetting = (*g_gameSettingCollection)->Get("fSkillUseCurve");
		fSkillUseCurve = (pSetting != nullptr) ? pSetting->data.f32 : 1.95f;
	}
	if(skillLevel < settings.settingsSkillCaps[skillID - 6])
	{
		result = pow(skillLevel, fSkillUseCurve);
		float a = 1.0f, b = 0.0f, c = 1.0f, d = 0.0f;
		if (GetSkillCoefficients(skillID, &a, &b, &c, &d))
			result = result * c + d;
	}
	return result;
}

float CalculateChargePointsPerUse_Hook(float basePoints, float enchantingLevel)
{


	float fEnchantingCostExponent = 1.10f;// 0x01D8A058;			 //1.10
	float fEnchantingSkillCostBase = 0.005f; // 0x01D8A010;		 //1/200 = 0.005
	float fEnchantingSkillCostScale = 0.5f;// 0x01D8A040;		 //0.5 sqrt
	//RelocPtr<float> unk0 = 0x014E8F78;			 //1.00f
	float fEnchantingSkillCostMult = 3.00f;// 0x01D8A028;			 //3.00

	if ((*g_gameSettingCollection) != nullptr)
	{
		Setting * pSetting = (*g_gameSettingCollection)->Get("fEnchantingCostExponent");
		fEnchantingCostExponent = (pSetting != nullptr) ? pSetting->data.f32 : 1.10f;
		pSetting = (*g_gameSettingCollection)->Get("fEnchantingSkillCostBase");
		fEnchantingSkillCostBase = (pSetting != nullptr) ? pSetting->data.f32 : 0.005f;
		pSetting = (*g_gameSettingCollection)->Get("fEnchantingSkillCostScale");
		fEnchantingSkillCostScale = (pSetting != nullptr) ? pSetting->data.f32 : 0.5f;
		pSetting = (*g_gameSettingCollection)->Get("fEnchantingSkillCostMult");
		fEnchantingSkillCostMult = (pSetting != nullptr) ? pSetting->data.f32 : 3.00f;
	}

	enchantingLevel = (enchantingLevel > 199.0f) ? 199.0f : enchantingLevel;
	float result = fEnchantingSkillCostMult * pow(basePoints, fEnchantingCostExponent) * (1.00f - pow((enchantingLevel * fEnchantingSkillCostBase), fEnchantingSkillCostScale));
#ifdef _DEBUG
	_MESSAGE("function:%s, basePoints:%.2f, enchantingLevel:%.2f, result:%.2f", __FUNCTION__, basePoints, enchantingLevel, result);
#endif
	return result;
	//Charges Per Use = 3 * (base enchantment cost * magnitude / maximum magnitude)^1.1 * (1 - sqrt(skill/200))
}

void ImproveSkillByTraining_Hook(void* pPlayer, UInt32 skillID, UInt32 count)
{
	PlayerSkills* skillData = *reinterpret_cast<PlayerSkills**>(reinterpret_cast<uintptr_t>(pPlayer)+0x9B0);
	if (count < 1)
		count = 1;
	if ((skillID >= 6) && (skillID <= 23))
	{
		LevelData* levelData = &(skillData->data->levelData[skillID - 6]);
		float skillProgression = 0.0f;
		if (levelData->pointsMax > 0.0f)
		{
			skillProgression = levelData->points / levelData->pointsMax;
#ifdef _DEBUG
			_MESSAGE("player:%p, skill:%d, points:%.2f, maxPoints:%.2f, level:%.2f", pPlayer, skillID, levelData->points, levelData->pointsMax, levelData->level);
#endif
			if (skillProgression >= 1.0f)
				skillProgression = 0.99f; 
		}
		else
			skillProgression = 0.0f;
		for (UInt32 i = 0; i < count; ++i)
		{
			float skillLevel = GetBaseActorValue((char*)(*g_thePlayer) + 0xB0, skillID);
			float expRequired = CalculateSkillExpForLevel(skillID, skillLevel);
#ifdef _DEBUG
			_MESSAGE("maxPoints:%.2f, expRequired:%.2f", levelData->pointsMax, expRequired);
#endif
			if (levelData->pointsMax != expRequired)
				levelData->pointsMax = expRequired;
			if (levelData->points <= 0.0f)
				levelData->points = (levelData->pointsMax > 0.0f) ? 0.1f : 0.0f;
			if (levelData->points >= levelData->pointsMax)
				levelData->points = (levelData->pointsMax > 0.0f) ? (levelData->pointsMax - 0.1f) : 0.0f;
			float expNeeded = levelData->pointsMax - levelData->points;
			ImprovePlayerSkillPoints_Original(skillData, skillID, expNeeded, 0, 0, 0, (i < count - 1));
		}
		levelData->points += levelData->pointsMax * skillProgression;
	}
}

void ImprovePlayerSkillPoints_Hook(PlayerSkills* skillData, UInt32 skillID, float exp, UInt64 unk1, UInt32 unk2, UInt8 unk3, bool unk4)
{
	if ((skillID >= 6) && (skillID <= 23))
	{
#ifdef _DEBUG
		_MESSAGE("function: %s, skillID: %d, SkillExpGainMults: %.2f", __FUNCTION__, skillID, settings.settingsSkillExpGainMults[skillID - 6]);
#endif
		UInt32 baseSkillLevel = static_cast<UInt32>(GetBaseActorValue((char*)(*g_thePlayer) + 0xB0, skillID));
		float skillMult = settings.settingsSkillExpGainMultsWithSkills[skillID - 6].GetValue(baseSkillLevel);
		UInt16 level = GetLevel(*g_thePlayer);
		float levelMult = settings.settingsSkillExpGainMultsWithPCLevel[skillID - 6].GetValue(level);
		exp *= settings.settingsSkillExpGainMults[skillID - 6] * skillMult * levelMult;
	}

#ifdef _DEBUG
	_MESSAGE("function: %s", __FUNCTION__);
	void* actorValue = static_cast<char*>(*g_thePlayer) + 0xB0;
	for (size_t i = 0; i <= 8; ++i)
		_MESSAGE("Index: %d, Function: %016I64X", i, *(*static_cast<uintptr_t**>(actorValue)+i));
#endif

	ImprovePlayerSkillPoints_Original(skillData, skillID, exp, unk1, unk2, unk3, unk4);
}

float ImproveLevelExpBySkillLevel_Hook(float skillLevel, UInt32 skillID)
{
	float baseMult = 1.0f, skillMult = 1.0f, levelMult = 1.0f;
	if ((skillID >= 6) && (skillID <= 23))
	{
		baseMult = settings.settingsLevelSkillExpMults[skillID - 6];
		UInt32 baseSkillLevel = static_cast<UInt32>(GetBaseActorValue((char*)(*g_thePlayer) + 0xB0, skillID));
		skillMult = settings.settingsLevelSkillExpMultsWithSkills[skillID - 6].GetValue(baseSkillLevel);
		UInt16 level = GetLevel(*g_thePlayer);
		levelMult = settings.settingsLevelSkillExpMultsWithPCLevel[skillID - 6].GetValue(level);
		//PCLevel has some minor glitch.I need to calculate player's actual level.
	}
	float result = ImproveLevelExpBySkillLevel_Original(skillLevel) * baseMult * skillMult * levelMult;
#ifdef _DEBUG
	_MESSAGE("function:%s, skillId:%d, skillLevel:%.2f, skillMult:%.2f, result:%.2f", __FUNCTION__, skillID, skillLevel, skillMult, result);
#endif
	return result;
}

UInt64 ImproveAttributeWhenLevelUp_Hook(void* unk0, UInt8 unk1)
{
	enum
	{
		kHealth = 0x18,
		kMagicka,
		kStamina
	};
	//static RelocPtr<UInt32> iAVDhmsLevelUp = 0x01D7C200;
	//static RelocPtr<float> fLevelUpCarryWeightMod = 0x01D882B8;

	if ((*g_gameSettingCollection) != nullptr)
	{
		Setting * iAVDhmsLevelUp = (*g_gameSettingCollection)->Get("iAVDhmsLevelUp");
		Setting * fLevelUpCarryWeightMod = (*g_gameSettingCollection)->Get("fLevelUpCarryWeightMod");
		if (iAVDhmsLevelUp && fLevelUpCarryWeightMod)
		{
			UInt32 choice = *reinterpret_cast<UInt32*>(static_cast<char*>(unk0) + 0x18);
			UInt16 level = GetLevel(*g_thePlayer);

			switch (choice)
			{
			case kHealth:
			{
				iAVDhmsLevelUp->data.u32 = settings.settingsHealthAtLevelUp.GetValue(level);
				fLevelUpCarryWeightMod->data.f32 = settings.settingsCarryWeightAtHealthLevelUp.GetValue(level);
				break;
			}
			case kMagicka:
			{
				iAVDhmsLevelUp->data.u32 = settings.settingsMagickaAtLevelUp.GetValue(level);
				fLevelUpCarryWeightMod->data.f32 = settings.settingsCarryWeightAtMagickaLevelUp.GetValue(level);
				break;
			}
			case kStamina:
			{
				iAVDhmsLevelUp->data.u32 = settings.settingsStaminaAtLevelUp.GetValue(level);
				fLevelUpCarryWeightMod->data.f32 = settings.settingsCarryWeightAtStaminaLevelUp.GetValue(level);
				break;
			}
			default:
			{
				iAVDhmsLevelUp->data.u32 = 5;
				fLevelUpCarryWeightMod->data.f32 = 10.0f;
			}
		}
		}
	}

#ifdef _DEBUG
	_MESSAGE("function: %s, attributePerLevel: %d, carryweightPerLevel: %.2f, level: %d, choice: %d", __FUNCTION__, *attributePerLevel, *carryweightPerLevel, level, choice);
#endif

	return ImproveAttributeWhenLevelUp_Original(unk0, unk1);
}

void ModifyPerkPool_Hook(SInt8 count)
{
	UInt8* points = *reinterpret_cast<UInt8**>(g_thePlayer.GetPtr()) + 0xB01;
	if (count > 0) //AddPerkPoints
	{
		//static float mantissa = 0.0f;  //This vlaue needs to be stored to cross save.
		UInt16 level = GetLevel(*g_thePlayer);
		float increment = settings.settingsPerksAtLevelUp.GetValue(level) + settings.settingsPerksAtLevelUp.GetDecimal(level);
		count = static_cast<SInt8>(increment);
		//mantissa = increment - count;
#ifdef _DEBUG
		_MESSAGE("function: %s, count: %d, perkPoints: %d, level: %d", __FUNCTION__, count, *points, level);
#endif
		UInt32 sum = count + *points;
		*points = (sum > 0xFF) ? 0xFF : static_cast<UInt8>(sum);
	}
	else //RemovePerkPoints
	{
		SInt32 sum = *points + count;
		*points = (sum < 0) ? 0 : static_cast<UInt8>(sum);
	}
}

float GetSkillCap_Hook(UInt32 skillID)
{
	if ((skillID >= 6) && (skillID <= 23))
	{
#ifdef _DEBUG
		_MESSAGE("function: %s, skillID: %d, skillCap: %d", __FUNCTION__, skillID, settings.settingsSkillCaps[skillID - 6]);
#endif
		return settings.settingsSkillCaps[skillID - 6];
	}
	else
	{
		//static RelocPtr<float> skillCap = 0x014E6B68;
#ifdef _DEBUG
		_MESSAGE("function: %s, skillCap: %.2f", __FUNCTION__, *skillCap);
#endif
		return 100.0f;
	}
}

float GetCurrentActorValue_Hook(void* avo, UInt32 skillID)   //PC&NPC  //61F6C0
{
	float skillLevel = GetCurrentActorValue_Original(avo, skillID);
	if ((skillID >= 6) && (skillID <= 23))
	{
		UInt32 skillFormulaCap = settings.settingsSkillFormulaCaps[skillID - 6];
		skillLevel = (skillLevel > skillFormulaCap) ? skillFormulaCap : skillLevel;
#ifdef _DEBUG
		//_MESSAGE("function: %s, skillID: %d, skillLevel:%.2f, skillFormulaCap: %d, this:%p", __FUNCTION__, skillID, skillLevel, settings.settingsSkillFormulaCaps[skillID - 6], avo);
#endif
	}
	return skillLevel;
}

void LegendaryResetSkillLevel_Hook(float baseLevel, UInt32 skillID) 
{
	if ((*g_gameSettingCollection) != nullptr)
	{
		Setting * fLegendarySkillResetValue = (*g_gameSettingCollection)->Get("fLegendarySkillResetValue");
		if (fLegendarySkillResetValue != nullptr)
		{
			static float originalSetting = fLegendarySkillResetValue->data.f32;
			if ((skillID >= 6) && (skillID <= 23))
			{
				if (settings.settingsLegendarySkill.bLegendaryKeepSkillLevel)
					fLegendarySkillResetValue->data.f32 = baseLevel;
				else
				{
					UInt32 legendaryLevel = settings.settingsLegendarySkill.iSkillLevelAfterLegendary;
					if ((legendaryLevel && legendaryLevel > baseLevel) || (!legendaryLevel && originalSetting > baseLevel))
						fLegendarySkillResetValue->data.f32 = baseLevel;
					else
						fLegendarySkillResetValue->data.f32 = (!legendaryLevel) ? originalSetting : legendaryLevel;
				}
			}
			else
				fLegendarySkillResetValue->data.f32 = originalSetting;
		}
	}
}

bool CheckConditionForLegendarySkill_Hook(void* pActorValueOwner, UInt32 skillID)
{
	float skillLevel = GetBaseActorValue(*(char**)(g_thePlayer.GetPtr()) + 0xB0, skillID);
	return (skillLevel >= settings.settingsLegendarySkill.iSkillLevelEnableLegendary) ? true : false;
}

bool HideLegendaryButton_Hook(UInt32 skillID)
{
	float skillLevel = GetBaseActorValue(*(char**)(g_thePlayer.GetPtr()) + 0xB0, skillID);
	if (skillLevel >= settings.settingsLegendarySkill.iSkillLevelEnableLegendary && !settings.settingsLegendarySkill.bHideLegendaryButton)
		return true;
	return false;
}


void Hook_Skill_Init()
{

}

void InitRVA()
{
	g_thePlayer								= RVAScan<void**>(GET_RVA(g_thePlayer), "F6 C1 04 74 65 4C 8B B7 18 01 00 00 4D 85 F6 74 59 49 8B D6 48 8B 0D ? ? ? ? E8 ? ? ? ? 0F B6 F0 49 8D 4E 30 84 C0 74 16", 0x14, 3, 7);

	g_gameSettingCollection					= RVAScan<SettingCollectionMap**>(GET_RVA(g_gameSettingCollection), "74 1A 8B 15 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03 EB 02 33 DB 48 89 1D ? ? ? ? 48 83 C4 30", 0x1E, 3, 7);

	kHook_ModifyPerkPool_Ent				= RVAScan<uintptr_t *>(GET_RVA(kHook_ModifyPerkPool_Ent), "48 85 C0 74 33 66 0F 6E C3 0F 5B C0 F3 0F 58 40 34 F3 0F 11 40 34 48 83 C4 20 5B C3 48 8B 15 ? ? ? ? 0F B6 8A 01 0B 00 00 8B C1 03 C3 78 08 02 CB 88 8A 01 0B 00 00", 0x1C);
	kHook_ModifyPerkPool_Ret				= kHook_ModifyPerkPool_Ent;
	kHook_ModifyPerkPool_Ret				+= 0x1C;

	kHook_SkillCapPatch_Ent					= RVAScan<uintptr_t *>(GET_RVA(kHook_SkillCapPatch_Ent), "48 81 C1 B0 00 00 00 41 0F 29 73 D8 45 0F 29 43 B8 48 8B 01 FF 50 18 F3 44 0F 10 ? ? ? ? 00 0F 28 F0 41 0F 2F F0 0F 83 74 02 00 00 48 8D 44 24 3C", 0x17);
	kHook_SkillCapPatch_Ret					= kHook_SkillCapPatch_Ent;
	kHook_SkillCapPatch_Ret					+= 9;

	kHook_ExecuteLegendarySkill_Ent			= RVAScan<uintptr_t *>(GET_RVA(kHook_ExecuteLegendarySkill_Ent), "0F 82 85 00 00 00 48 8B 0D ? ? ? ? 48 81 C1 B0 00 00 00 48 8B 01 F3 0F 10 15 ? ? ? ? 8B 56 1C FF 50 20 48 8B 05 ? ? ? ? 8B 56 1C 48 8B 88 B0 09 00 00");
	kHook_ExecuteLegendarySkill_Ret			= kHook_ExecuteLegendarySkill_Ent;
	kHook_ExecuteLegendarySkill_Ret			+= 6;

	kHook_CheckConditionForLegendarySkill_Ent = RVAScan<uintptr_t *>(GET_RVA(kHook_CheckConditionForLegendarySkill_Ent), "8B D0 48 8D 8F B0 00 00 00 FF 53 18 0F 2F 05 ? ? ? ? 0F 82 10 0A 00 00 45 33 FF 4C 89 7D 80 44 89 7D 88 45 33 C0 48 8B 15 ? ? ? ? 48 8D 4D 80");
	kHook_CheckConditionForLegendarySkill_Ret = kHook_CheckConditionForLegendarySkill_Ent;
	kHook_CheckConditionForLegendarySkill_Ret += 0x13;

	kHook_HideLegendaryButton_Ent			= RVAScan<uintptr_t *>(GET_RVA(kHook_HideLegendaryButton_Ent), "48 8B 0D ? ? ? ? 48 81 C1 B0 00 00 00 48 8B 01 8B D6 FF 50 18 0F 2F 05 ? ? ? ? 72 64 48 8D 05 ? ? ? ? 48 89 85 C0 00 00 00 4C 89 64 24 20");
	kHook_HideLegendaryButton_Ret			= kHook_HideLegendaryButton_Ent;
	kHook_HideLegendaryButton_Ret			+= 0x1D;

	ImproveSkillByTraining_Original			= RVAScan<_ImproveSkillByTraining>(GET_RVA(ImproveSkillByTraining_Original), "48 8B 89 B0 09 00 00 B8 01 00 00 00 44 3B C0 44 0F 42 C0 E9 ? ? ? ?");

	ImprovePlayerSkillPoints				= RVAScan<_ImprovePlayerSkillPoints>(GET_RVA(ImprovePlayerSkillPoints), "4C 8B DC 55 56 41 56 48 81 EC 60 01 00 00 8D 42 FA 41 0F 29 7B C8 49 8B E9 0F 28 FA 8B F2 4C 8B F1 83 F8 11 0F 87 B9 02 00 00");

	ImproveAttributeWhenLevelUp				= RVAScan<_ImproveAttributeWhenLevelUp>(GET_RVA(ImproveAttributeWhenLevelUp), "0F B6 DA 48 8B F9 48 8B 15 ? ? ? ? 48 81 C2 28 01 00 00 48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 84 BA 00 00 00 84 DB 0F 85 AA 00 00 00", -0x1E);

	GetSkillCoefficients					= RVAScan<_GetSkillCoefficients>(GET_RVA(GetSkillCoefficients), "81 F9 A3 00 00 00 77 52 48 8B 05 ? ? ? ? 48 63 C9 4C 8B 54 C8 08 4D 85 D2 74 3E 49 8B 82 08 01 00 00 48 85 C0 74 32");

	GetLevel								= RVAScan<_GetLevel>(GET_RVA(GetLevel), "84 C0 74 24 48 8B CB E8 ? ? ? ? 0F B7 C8 48 8B D7 E8 ? ? ? ? 84 C0 74 0D B0 01 48 8B 5C 24 30 48 83 C4 20 5F C3", 7, 1, 5);

	ImproveLevelExpBySkillLevel_Original	= RVAScan<_ImproveLevelExpBySkillLevel>(GET_RVA(ImproveLevelExpBySkillLevel_Original), "F3 0F 58 D3 0F 28 E0 0F 29 34 24 0F 57 F6 0F 28 CA F3 0F 58 CB F3 0F 59 CA F3 0F 59 CD F3 0F 2C C1 66 0F 6E C0 0F 5B C0 F3 0F 5C C8 0F 2F CE 73 04 F3 0F 5C C3", -0x17);

	GetCurrentActorValue					= RVAScan<_GetCurrentActorValue>(GET_RVA(GetCurrentActorValue), "4C 8B 44 F8 08 41 8B 40 60 C1 E8 12 A8 01 74 38 48 8B 49 40 48 85 C9 74 2F 48 83 79 50 00 74 28 40 B5 01 0F 57 C0 F3 0F 11 44 24 78 4C 8D 44 24 78 8B D7", -0x2C);

	GetBaseActorValue						= RVAScan<_GetBaseActorValue>(GET_RVA(GetBaseActorValue), "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 30 48 63 DA 4C 8D 44 24 40 48 8B F9 0F 57 C0 8B D3 F3 0F 11 44 24 40 48 81 C1 50 01 00 00 E8 ? ? ? ? 84 C0 0F 85 DE 00 00 00");

	CalculateChargePointsPerUse_Original	= RVAScan<_CalculateChargePointsPerUse>(GET_RVA(CalculateChargePointsPerUse_Original), "48 83 EC 48 0F 29 74 24 30 0F 29 7C 24 20 0F 28 F8 F3 0F 10 05 ? ? ? ? F3 0F 59 C1 F3 0F 10 0D ? ? ? ? E8 ? ? ? ? F3 0F 10 35 ? ? ? ? F3 0F 10 0D ? ? ? ?");

	GetEffectiveSkillLevel					= RVAScan<_GetEffectiveSkillLevel>(GET_RVA(GetEffectiveSkillLevel), "40 53 48 83 EC 20 48 8B 01 48 63 DA 8B D3 FF 50 08 48 8B 05 ? ? ? ? 48 8B 4C D8 08 8B 51 60 8B C2 C1 E8 04 A8 01 74 1E F3 0F 10 0D ? ? ? ? 0F 2F C1 73 44 0F 57 D2 0F 2F C2 73 3F");
}

void Hook_Skill_Commit()
{

	InitRVA();
	//return;

	SafeWrite16(GetEffectiveSkillLevel.GetUIntPtr() + 0x34, 0x9090);

	g_branchTrampoline.Write6Branch(ImproveSkillByTraining_Original.GetUIntPtr(), (uintptr_t)ImproveSkillByTraining_Hook);

	g_branchTrampoline.Write6Branch(CalculateChargePointsPerUse_Original.GetUIntPtr(), (uintptr_t)CalculateChargePointsPerUse_Hook);

	{
		struct ImproveLevelExpBySkillLevel_Code : Xbyak::CodeGenerator
		{
			ImproveLevelExpBySkillLevel_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rcx);
				push(rdx);
				mov(rdx, rsi);
				sub(rsp, 0x20);
				call((void *)&ImproveLevelExpBySkillLevel_Hook);
				add(rsp, 0x20);
				pop(rdx);
				pop(rcx);
				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(ImprovePlayerSkillPoints.GetUIntPtr() + 0x230);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ImproveLevelExpBySkillLevel_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(ImprovePlayerSkillPoints.GetUIntPtr() + 0x22B, uintptr_t(code.getCode()));
	}

	{

		struct ModifyPerkPool_Code : Xbyak::CodeGenerator
		{
			ModifyPerkPool_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rcx, rbx);
				sub(rsp, 0x20);
				call((void *)&ModifyPerkPool_Hook);
				add(rsp, 0x20);
				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_ModifyPerkPool_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ModifyPerkPool_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_ModifyPerkPool_Ent.GetUIntPtr(), uintptr_t(code.getCode()));
	}

	{

		struct SkillCapPatch_Code : Xbyak::CodeGenerator
		{
			SkillCapPatch_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rcx, rsi);
				sub(rsp, 0x30);
				movss(ptr[rsp + 0x28], xmm0);
				call((void *)&GetSkillCap_Hook);
				movss(xmm8, xmm0);
				movss(xmm0, ptr[rsp + 0x28]);
				add(rsp, 0x30);
				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_SkillCapPatch_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		SkillCapPatch_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_SkillCapPatch_Ent.GetUIntPtr(), uintptr_t(code.getCode()));
	}


	{
		struct ImprovePlayerSkillPoints_Code : Xbyak::CodeGenerator
		{
			ImprovePlayerSkillPoints_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(r11, rsp);
				push(rbp);
				push(rsi);
				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(ImprovePlayerSkillPoints.GetUIntPtr() + 0x5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ImprovePlayerSkillPoints_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		ImprovePlayerSkillPoints_Original = (_ImprovePlayerSkillPoints)codeBuf;

		g_branchTrampoline.Write5Branch(ImprovePlayerSkillPoints.GetUIntPtr(), (uintptr_t)ImprovePlayerSkillPoints_Hook);
	}

	{
		struct ImproveAttributeWhenLevelUp_Code : Xbyak::CodeGenerator
		{
			ImproveAttributeWhenLevelUp_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rdi);
				sub(rsp, 0x30);

				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(ImproveAttributeWhenLevelUp.GetUIntPtr() + 0x6);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ImproveAttributeWhenLevelUp_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		ImproveAttributeWhenLevelUp_Original = (_ImproveAttributeWhenLevelUp)codeBuf;

		g_branchTrampoline.Write6Branch(ImproveAttributeWhenLevelUp.GetUIntPtr(), (uintptr_t)ImproveAttributeWhenLevelUp_Hook);

		SafeWrite8(ImproveAttributeWhenLevelUp.GetUIntPtr() + 0x9B, 0);
	}

	{
		struct GetCurrentActorValue_Code : Xbyak::CodeGenerator
		{
			GetCurrentActorValue_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rbp);
				push(rsi);
				push(rdi);
				push(r14);

				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(GetCurrentActorValue.GetUIntPtr() + 0x6);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		GetCurrentActorValue_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		GetCurrentActorValue_Original = (_GetCurrentActorValue)codeBuf;

		g_branchTrampoline.Write6Branch(GetCurrentActorValue.GetUIntPtr(), (uintptr_t)GetCurrentActorValue_Hook);

	}

	{
		struct ExecuteLegendarySkill_Code : Xbyak::CodeGenerator
		{
			ExecuteLegendarySkill_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(edx, ptr[rsi + 0x1C]);
				call((void*)&LegendaryResetSkillLevel_Hook);

				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_ExecuteLegendarySkill_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ExecuteLegendarySkill_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write6Branch(kHook_ExecuteLegendarySkill_Ent.GetUIntPtr(), uintptr_t(code.getCode()));
	}

	{
		struct CheckConditionForLegendarySkill_Code : Xbyak::CodeGenerator
		{
			CheckConditionForLegendarySkill_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(edx, eax);
				lea(rcx, ptr[rdi + 0xB0]);
				call((void*)&CheckConditionForLegendarySkill_Hook);
				cmp(al, 1);

				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_CheckConditionForLegendarySkill_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		CheckConditionForLegendarySkill_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write6Branch(kHook_CheckConditionForLegendarySkill_Ent.GetUIntPtr(), uintptr_t(code.getCode()));
	}

	{
		struct HideLegendaryButton_Code : Xbyak::CodeGenerator
		{
			HideLegendaryButton_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;
				mov(ecx, esi);
				call((void*)&HideLegendaryButton_Hook);
				cmp(al, 1);

				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_HideLegendaryButton_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		HideLegendaryButton_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write6Branch(kHook_HideLegendaryButton_Ent.GetUIntPtr(), uintptr_t(code.getCode()));

	}
}


