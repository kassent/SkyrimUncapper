#pragma once

class ActorValueOwner
{
public:
	//00007FF71DFA71B0 + 28 = 00007FF71DFA71D8
	virtual ~ActorValueOwner(){}
	//C0 86 F8 1C F7 7F 00 00   7FF71CF886C0

	// Argument is the ActorValue ID
	virtual float	GetCurrent(UInt32 arg);
	virtual float	GetMaximum(UInt32 arg);
	virtual float	GetBase(UInt32 arg);
	virtual void	SetBase(UInt32 arg0, float arg1);
	virtual void	ModBase(UInt32 arg0, float arg1);
	virtual void	Unk_06(UInt32 arg0, UInt32 arg1, UInt32 arg2);// Force/Mod AV?
	virtual void	SetCurrent(UInt32 arg0, float arg1);
	virtual bool	Unk_08(void);

	//	void	** _vtbl;	// 00
};

class PlayerSkills
{
public:

	enum {
		kAdvanceableSkillOffset = 6, //
		kNumAdvanceableSkills = 18
	};

	struct StatData {
		struct LevelData {
			float level;		// 00
			float points;		// 04
			float pointsMax;	// 08
		};

		float		levelPoints;							// 00
		float		levelPointsMax;							// 04
		LevelData	levelData[kNumAdvanceableSkills];		// 08
		UInt32		legendaryLevel[kNumAdvanceableSkills];	// 90
	};

	StatData * data;
};


void Hook_Skill_Init();
void Hook_Skill_Commit();