#pragma once
namespace jlg { namespace rl { namespace lobby_ranks {

enum class Playlist {
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

std::list<Playlist> playlists = {
	Playlist::Unranked,
	Playlist::Solo,
	Playlist::Twos,
	Playlist::Threes,
	Playlist::Hoops,
	Playlist::Rumble,
	Playlist::Dropshot,
	Playlist::Snowday,
	Playlist::Tournament };

enum class Rank {
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

}; }; }; // END namespace jlg::rl::lobby_ranks
