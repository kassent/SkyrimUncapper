#include "Hook_Skill.h"
#include "Settings.h"
#include "Relocation.h"
#include "BranchTrampoline.h"
#include "SafeWrite.h"
#include "xbyak.h"
using LevelData = PlayerSkills::StatData::LevelData;

RelocPtr <void*>			g_pCharacter		   = 0x02EC86A8;
/*
RelocPtr <UInt8>			g_numPerkPoints = reinterpret_cast<uintptr_t>(*g_pCharacter.GetPtr()) + 0xB01;
RelocPtr <PlayerSkills>		g_playerSkills = reinterpret_cast<uintptr_t>(*g_pCharacter.GetPtr()) + 0x9B0;
RelocPtr <ActorValueOwner>	g_actorValueOwner = reinterpret_cast<uintptr_t>(*g_pCharacter.GetPtr()) + 0xB0;
*/
RelocAddr <uintptr_t> kHook_ModPerkPool_Ent = 0x008AA60F;
RelocAddr <uintptr_t> kHook_ModPerkPool_Ret = 0x008AA62B;

RelocAddr <uintptr_t> kHook_SkillCapPatch_Ent = 0x006CD2F8;
RelocAddr <uintptr_t> kHook_SkillCapPatch_Ret = 0x006CD301;

typedef void(*_ImproveSkillByTraining)(void* pPlayer, UInt32 skillID, UInt32 count);
RelocAddr <_ImproveSkillByTraining> ImproveSkillByTraining_Original = 0x0689AC0;

typedef void(*_ImprovePlayerSkillPoints)(void*, UInt32, float, UInt64, UInt32, UInt8, bool);
RelocAddr <_ImprovePlayerSkillPoints> ImprovePlayerSkillPoints = 0x06CD2B0;
_ImprovePlayerSkillPoints ImprovePlayerSkillPoints_Original = nullptr;

typedef UInt64(*_ImproveAttributeWhenLevelUp)(void*, UInt8);
RelocAddr <_ImproveAttributeWhenLevelUp> ImproveAttributeWhenLevelUp = 0x087A420;
_ImproveAttributeWhenLevelUp ImproveAttributeWhenLevelUp_Original = nullptr;

typedef bool(*_GetSkillCoefficients)(UInt32, float*, float*, float*, float*);
RelocAddr <_GetSkillCoefficients> GetSkillCoefficients = 0x03E0440;

typedef UInt16(*_GetLevel)(void* pPlayer);
RelocAddr <_GetLevel> GetLevel = 0x05BD8F0;

typedef float(*_ImproveLevelExpBySkillLevel)(float skillLevel);
RelocAddr <_ImproveLevelExpBySkillLevel> ImproveLevelExpBySkillLevel_Original = 0x06CE510;

typedef float(*_GetCurrentActorValue)(void*, UInt32);
RelocAddr <_GetCurrentActorValue> GetCurrentActorValue = 0x0608370;
_GetCurrentActorValue GetCurrentActorValue_Original = nullptr;


float CalculateSkillExpForLevel(UInt32 skillID, float skillLevel)
{
#ifdef _DEBUG
	_MESSAGE("function:%s, skillId:%d, skillLevel:%.2f", __FUNCTION__, skillID, skillLevel);
#endif
	float result = 0.0f;
	RelocPtr <float> pow = 0x01D88258;
	typedef float(*Fn)(float, float);
	RelocAddr <Fn> fn = 0x01312C52;
	if (skillLevel < 100.0f)
	{
		result = fn(skillLevel, *pow.GetPtr());
		float a = 1.0f, b = 0.0f, c = 1.0f, d = 0.0f;
		if (GetSkillCoefficients(skillID, &a, &b, &c, &d))
			result = result * c + d;
	}
	return result;
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
			typedef float(*Fn)(void*, UInt32);
			RelocAddr <Fn> fn = 0x0608540;	//GetBaseActorValue
			float skillLevel = fn(*(char**)(g_pCharacter.GetPtr()) + 0xB0, skillID);
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
		exp *= settings.settingsSkillExpGainMults[skillID - 6];
	}

#ifdef INVALID_CODE
	_MESSAGE("function: %s", __FUNCTION__);
	void* actorValue = static_cast<char*>(*g_pCharacter) + 0xB0;
	for (size_t i = 0; i <= 8; ++i)
		_MESSAGE("Index: %d, Function: %016X", i, *(*static_cast<uintptr_t**>(actorValue)+i));
#endif

	ImprovePlayerSkillPoints_Original(skillData, skillID, exp, unk1, unk2, unk3, unk4);
}

float ImproveLevelExpBySkillLevel_Hook(float skillLevel, UInt32 skillID)
{
	float skillMult = 1.0f;
	if ((skillID >= 6) && (skillID <= 23))
		skillMult = settings.settingsLevelSkillExpMults[skillID - 6];
	float result = ImproveLevelExpBySkillLevel_Original(skillLevel);
#ifdef _DEBUG
	_MESSAGE("function:%s, skillId:%d, skillLevel:%.2f, skillMult:%.2f, result:%.2f", __FUNCTION__, skillID, skillLevel, skillMult, result);
#endif
	return result * skillMult;
}

UInt64 ImproveAttributeWhenLevelUp_Hook(void* unk0, UInt8 unk1)
{
	enum
	{
		kHealth = 0x18,
		kMagicka,
		kStamina
	};
	static RelocPtr<UInt32> attributePerLevel = 0x01D7C200;
	static RelocPtr<float> carryweightPerLevel = 0x01D882B8;

	UInt32 choice = *reinterpret_cast<UInt32*>(static_cast<char*>(unk0)+0x18);
	UInt16 level = GetLevel(*g_pCharacter);

	switch (choice)
	{
		case kHealth:
		{
			*attributePerLevel = settings.settingsHealthAtLevelUp.GetValue(level);
			*carryweightPerLevel = settings.settingsCarryWeightAtHealthLevelUp.GetValue(level);
			break;
		}
		case kMagicka:
		{
			*attributePerLevel = settings.settingsMagickaAtLevelUp.GetValue(level);
			*carryweightPerLevel = settings.settingsCarryWeightAtMagickaLevelUp.GetValue(level);
			break;
		}
		case kStamina:
		{
			*attributePerLevel = settings.settingsStaminaAtLevelUp.GetValue(level);
			*carryweightPerLevel = settings.settingsCarryWeightAtStaminaLevelUp.GetValue(level);
			break;
		}
		default:
		{
			*attributePerLevel = 5;
			*carryweightPerLevel = 10.0f;
		}
	}
#ifdef _DEBUG
	_MESSAGE("function: %s, attributePerLevel: %d, carryweightPerLevel: %.2f, level: %d, choice: %d", __FUNCTION__, *attributePerLevel, *carryweightPerLevel, level, choice);
#endif

	return ImproveAttributeWhenLevelUp_Original(unk0, unk1);
}

void ModPerkPool_Hook(SInt8 count)
{
	UInt8* points = *reinterpret_cast<UInt8**>(g_pCharacter.GetPtr()) + 0xB01;
	if (count > 0) //AddPerkPoints
	{
		static float mantissa = 0.0f;  //This vlaue needs to be stored to cross save.
		UInt16 level = GetLevel(*g_pCharacter);
		float increment = settings.settingsPerksAtLevelUp.GetValue(level) + mantissa;
		count = static_cast<SInt8>(increment);
		mantissa = increment - count;
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
		_MESSAGE("function: %s, skillCap: %.2f", __FUNCTION__, settings.settingsSkillCaps[skillID - 6]);
#endif
		return settings.settingsSkillCaps[skillID - 6];
	}
	else
	{
		static RelocPtr<float> skillCap = 0x014E6B68;
#ifdef _DEBUG
		_MESSAGE("function: %s, skillCap: %.2f", __FUNCTION__, *skillCap);
#endif
		return *skillCap;
	}
}

float GetCurrentActorValue_Hook(void* avo, UInt32 skillID)   //PC&NPC
{
	float skillLevel = GetCurrentActorValue_Original(avo, skillID);
	if ((skillID >= 6) && (skillID <= 23))
	{
		UInt32 skillFormulaCap = settings.settingsSkillFormulaCaps[skillID - 6];
		skillLevel = (skillLevel > skillFormulaCap) ? skillFormulaCap : skillLevel;
#ifdef _DEBUG
		_MESSAGE("function: %s, skillID: %d, skillLevel:%.2f, skillFormulaCap: %d, this:%p", __FUNCTION__, skillID, skillLevel, settings.settingsSkillFormulaCaps[skillID - 6], avo);
#endif
	}
	return skillLevel;
}

void Hook_Skill_Init()
{

}

void Hook_Skill_Commit()
{
	settings.ReadConfig();

	g_branchTrampoline.Write6Branch(ImproveSkillByTraining_Original.GetUIntPtr(), (uintptr_t)ImproveSkillByTraining_Hook);

	{
		struct ImproveLevelExpBySkillLevel_Code : Xbyak::CodeGenerator
		{
			ImproveLevelExpBySkillLevel_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rcx);
				sub(rsp, 0x28);
				call((void *)&ImproveLevelExpBySkillLevel_Hook);
				add(rsp, 0x28);
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

		struct ModPerkPool_Code : Xbyak::CodeGenerator
		{
			ModPerkPool_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rcx, rbx);
				sub(rsp, 0x20);
				call((void *)&ModPerkPool_Hook);
				add(rsp, 0x20);
				jmp(ptr[rip + retnLabel]);

			L(retnLabel);
				dq(kHook_ModPerkPool_Ret.GetUIntPtr());
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ModPerkPool_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_ModPerkPool_Ent.GetUIntPtr(), uintptr_t(code.getCode()));
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
#ifdef INVALID_CODE
	{
		float maxLevel = 10000.0f;
		RelocAddr<float> skillCap = 0x014E6B68;	//not sure.
		SafeWrite64(skillCap.GetUIntPtr(), *(uintptr_t*)&maxLevel);
	}
#endif
}