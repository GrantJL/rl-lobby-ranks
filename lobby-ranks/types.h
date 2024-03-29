#pragma once

#include "bakkesmod/wrappers/wrapperstructs.h"

#include <string>

namespace jlg {

#pragma region Types

enum class Team
{
	Blue = 0,
	Orange = 1,
	None = 2
};
const static auto TeamValues = { Team::Blue, Team::Orange, Team::None };

enum class Playlist
{
	Unranked = 1,
	Solo = 10,
	Twos = 11,
	Threes = 13,
	Hoops = 27,
	Rumble = 28,
	Dropshot = 29,
	Snowday = 30,
	Tournament = 34,
};
const static auto PlaylistValues = { Playlist::Unranked, Playlist::Solo, Playlist::Twos, Playlist::Threes, Playlist::Hoops, Playlist::Rumble, Playlist::Dropshot, Playlist::Snowday, Playlist::Tournament };

enum class Rank
{
	SupersonicLegend = 22,
	GrandChamp3 = 21,
	GrandChamp2 = 20,
	GrandChamp1 = 19,
	Champ3 = 18,
	Champ2 = 17,
	Champ1 = 16,
	Diamond3 = 15,
	Diamond2 = 14,
	Diamond1 = 13,
	Platinum3 = 12,
	Platinum2 = 11,
	Platinum1 = 10,
	Gold3 = 9,
	Gold2 = 8,
	Gold1 = 7,
	Silver3 = 6,
	Silver2 = 5,
	Silver1 = 4,
	Bronze3 = 3,
	Bronze2 = 2,
	Bronze1 = 1,
	Unranked = 0,
};
const static auto RankDivValues = { Rank::SupersonicLegend, Rank::GrandChamp3, Rank::GrandChamp2, Rank::GrandChamp1, Rank::Champ3, Rank::Champ2, Rank::Champ1, Rank::Diamond3, Rank::Diamond2, Rank::Diamond1, Rank::Platinum3, Rank::Platinum2, Rank::Platinum1, Rank::Gold3, Rank::Gold2, Rank::Gold1, Rank::Silver3, Rank::Silver2, Rank::Silver1, Rank::Bronze3, Rank::Bronze2, Rank::Bronze1, Rank::Unranked };

enum class Platform
{
	Unknown,
	Steam,
	Playstation,
	Xbox,
	Nintendo,
	Epic,
};
const static auto PlatformValues = { Platform::Unknown, Platform::Steam, Platform::Playstation, Platform::Xbox, Platform::Nintendo, Platform::Epic };

static Platform toPlatform( OnlinePlatform platform )
{
	switch( platform )
	{
		case OnlinePlatform_Steam:
			return Platform::Steam;
		case OnlinePlatform_PS4:
		case OnlinePlatform_PS3:
			return Platform::Playstation;
		case OnlinePlatform_Dingo:
			return Platform::Xbox;
		case OnlinePlatform_OldNNX:
		case OnlinePlatform_NNX:
			return Platform::Nintendo;
		case OnlinePlatform_PsyNet:
		case OnlinePlatform_Epic:
			return Platform::Epic;
		default:
		case OnlinePlatform_QQ:
		case OnlinePlatform_Deleted:
		case OnlinePlatform_WeGame:
		case OnlinePlatform_MAX:
			return Platform::Unknown;
	}
}

#pragma endregion

#pragma region Type Values

namespace Color
{
	static const LinearColor White { 255, 255, 255, 255 };

	namespace Team
	{
		static const LinearColor White { 255, 255, 255, 255 };
		static const LinearColor Blue  {  32, 116, 255, 255 };
		static const LinearColor Orange{ 255, 156,  12, 255 };
	} // END namespace Team

	namespace Rank
	{
		static const LinearColor Unranked         { 179, 179, 179, 255 };
		static const LinearColor Bronze           { 255, 153,   0, 255 };
		static const LinearColor Silver           { 255, 255, 255, 255 };
		static const LinearColor Gold             { 255, 217,   0, 255 };
		static const LinearColor Platinum         {  57, 248, 255, 255 };
		static const LinearColor Diamond          {  67, 164, 255, 255 };
		static const LinearColor Champ            { 202, 137, 255, 255 };
		static const LinearColor GrandChamp       { 255,  93,  93, 255 };
		static const LinearColor SupersonicLegend { 255, 111, 219, 255 };
	} // END namespace Rank

	namespace Platform
	{
		static const LinearColor Unknown          {   0,   0,   0,   0};
		static const LinearColor Steam            { 142, 150, 179, 255};
		static const LinearColor Playstation      {  30,  77, 182, 255};
		static const LinearColor Xbox             {  61, 117,  35, 255};
		static const LinearColor Nintendo         { 205,  53,  32, 255};
		static const LinearColor Epic             { 164, 164, 164, 255};
	}

} // END namespace Color

namespace String
{
	namespace Playlist
	{
		const static std::string Default    { "???" };
		const static std::string Unranked   { "-" };
		const static std::string Solo       { "1v1" };
		const static std::string Twos       { "2v2" };
		const static std::string Threes     { "3v3" };
		const static std::string Hoops      { "H" };
		const static std::string Rumble     { "R" };
		const static std::string Dropshot   { "D" };
		const static std::string Snowday    { "S" };
		const static std::string Tournament { "T" };
	} // END namespace Playlist

	namespace Rank {
		static const std::string Default          { "?" };
		static const std::string Unranked         { "-" };
		static const std::string Bronze1          { "B1" };
		static const std::string Bronze2          { "B2" };
		static const std::string Bronze3          { "B3" };
		static const std::string Silver1          { "S1" };
		static const std::string Silver2          { "S2" };
		static const std::string Silver3          { "S3" };
		static const std::string Gold1            { "G1" };
		static const std::string Gold2            { "G2" };
		static const std::string Gold3            { "G3" };
		static const std::string Platinum1        { "P1" };
		static const std::string Platinum2        { "P2" };
		static const std::string Platinum3        { "P3" };
		static const std::string Diamond1         { "D1" };
		static const std::string Diamond2         { "D2" };
		static const std::string Diamond3         { "D3" };
		static const std::string Champ1           { "C1" };
		static const std::string Champ2           { "C2" };
		static const std::string Champ3           { "C3" };
		static const std::string GrandChamp1      { "GC1" };
		static const std::string GrandChamp2      { "GC2" };
		static const std::string GrandChamp3      { "GC3" };
		static const std::string SupersonicLegend { "SSL" };
	} // END namespace Rank

} // END namespace String

#pragma endregion

} // END namespace jlg
