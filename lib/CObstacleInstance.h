#pragma once
#include "BattleHex.h"

/*
 * CObstacleInstance.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

struct CObstacleInfo;

struct DLL_LINKAGE CObstacleInstance
{
	BattleHex pos; //position on battlefield, typically left bottom corner
	ui8 obstacleType; //if true, then position is meaningless
	si32 uniqueID;
	si32 ID; //ID of obstacle (defines type of it)

	enum EObstacleType
	{
		//ABSOLUTE needs an underscore because it's a Win
		USUAL, ABSOLUTE_OBSTACLE, QUICKSAND, LAND_MINE, FORCE_FIELD, FIRE_WALL, MOAT
	};


	//used only for spell-created obstacles

	CObstacleInstance();
	virtual ~CObstacleInstance();

	//bool spellGenerated() const;
	const CObstacleInfo &getInfo() const; //allowed only when not generated by spell (usual or absolute)

	std::vector<BattleHex> getBlockedTiles() const;
	std::vector<BattleHex> getStoppingTile() const; //hexes that will stop stack move

	//The two functions below describe how the obstacle affects affected tiles
	//additional effects (like hurting stack or disappearing) are hardcoded for appropriate obstacleTypes
	bool blocksTiles() const;
	bool stopsMovement() const; //if unit stepping onto obstacle, can't continue movement (in general, doesn't checks for the side)

	virtual std::vector<BattleHex> getAffectedTiles() const;
	virtual bool visibleForSide(ui8 side, bool hasNativeStack) const; //0 attacker

	virtual void battleTurnPassed(){};

	template <typename Handler> void serialize(Handler &h, const int version)
	{
		h & ID & pos & obstacleType & uniqueID;
	}
};

struct DLL_LINKAGE MoatObstacle : CObstacleInstance
{
	virtual std::vector<BattleHex> getAffectedTiles() const override; //for special effects (not blocking)
};

struct DLL_LINKAGE SpellCreatedObstacle : CObstacleInstance
{
	si8 turnsRemaining;
	si8 casterSpellPower;
	si8 spellLevel;
	si8 casterSide; //0 - obstacle created by attacker; 1 - by defender
	si8 visibleForAnotherSide;

	SpellCreatedObstacle();

	virtual std::vector<BattleHex> getAffectedTiles() const override; //for special effects (not blocking)
	virtual bool visibleForSide(ui8 side, bool hasNativeStack) const override; //0 attacker

	virtual void battleTurnPassed() override;

	template <typename Handler> void serialize(Handler &h, const int version)
	{
		h & static_cast<CObstacleInstance&>(*this);
		h & turnsRemaining & casterSpellPower & spellLevel & casterSide & visibleForAnotherSide;
	}
};
