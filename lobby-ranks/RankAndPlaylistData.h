#pragma once

#include <list>
#include <string>
#include <map>

#include "Enums.h"

namespace jlg { namespace rl { namespace lobby_ranks {

struct Color { int r, g, b; };
struct PlayerStatStruct { Color color; std::string text; };
struct RankInfo { Color color; std::string name; std::string abbvr; };

static std::string name( Playlist playlist )
{
	switch( playlist )
	{
		default:                   return "???";
		case Playlist::Unranked:   return "-";
		case Playlist::Solo:       return "1v1";
		case Playlist::Twos:       return "2v2";
		case Playlist::Threes:     return "3v3";
		case Playlist::Hoops:      return "H";
		case Playlist::Rumble:     return "R";
		case Playlist::Dropshot:   return "D";
		case Playlist::Snowday:    return "S";
		case Playlist::Tournament: return "T";
	}
}

std::map<Rank, RankInfo> RankTiers = {
	{ Rank::Unranked,         { { 179, 179, 179 }, "Unranked",          "-" } },
	{ Rank::Bronze1,          { { 255, 153, 0   }, "Bronze 1",          "B1" } },
	{ Rank::Bronze2,          { { 255, 153, 0   }, "Bronze 2",          "B2" } },
	{ Rank::Bronze3,          { { 255, 153, 0   }, "Bronze 3",          "B3" } },
	{ Rank::Silver1,          { { 255, 255, 255 }, "Silver 1",          "S1" } },
	{ Rank::Silver2,          { { 255, 255, 255 }, "Silver 2",          "S2" } },
	{ Rank::Silver3,          { { 255, 255, 255 }, "Silver 3",          "S3" } },
	{ Rank::Gold1,            { { 255, 217, 0   }, "Gold 1",            "G1" } },
	{ Rank::Gold2,            { { 255, 217, 0   }, "Gold 2",            "G2" } },
	{ Rank::Gold3,            { { 255, 217, 0   }, "Gold 3",            "G3" } },
	{ Rank::Platinum1,        { { 57,  248, 255 }, "Platinum 1",        "P1" } },
	{ Rank::Platinum2,        { { 57,  248, 255 }, "Platinum 2",        "P2" } },
	{ Rank::Platinum3,        { { 57,  248, 255 }, "Platinum 3",        "P3" } },
	{ Rank::Diamond1,         { { 67,  164, 255 }, "Diamond 1",         "D1" } },
	{ Rank::Diamond2,         { { 67,  164, 255 }, "Diamond 2",         "D2" } },
	{ Rank::Diamond3,         { { 67,  164, 255 }, "Diamond 3",         "D3" } },
	{ Rank::Champ1,           { { 202, 137, 255 }, "Champion 1",        "C1" } },
	{ Rank::Champ2,           { { 202, 137, 255 }, "Champion 2",        "C2" } },
	{ Rank::Champ3,           { { 202, 137, 255 }, "Champion 3",        "C3" } },
	{ Rank::GrandChamp1,      { { 255, 93,  93  }, "Grand Champion 1",  "GC1" } },
	{ Rank::GrandChamp2,      { { 255, 93,  93  }, "Grand Champion 2",  "GC2" } },
	{ Rank::GrandChamp3,      { { 255, 93,  93  }, "Grand Champion 3",  "GC3" } },
	{ Rank::SupersonicLegend, { { 255, 111, 219 }, "Supersonic Legend", "SSL" } }
};

}; }; }; // END namespace jlg::rl::lobby_ranks
