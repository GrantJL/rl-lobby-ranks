#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "RankAndPlaylistData.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

// JLG TODO
//  Function TAGame.GameEvent_Soccar_TA.Destroyed (grey out name : user leaves game)
//  Function GameEvent_Soccar_TA.Active.StartRound (all players ready
//  

namespace jlg { namespace rl { namespace lobby_ranks {

struct PlaylistData
{
	Color color;
	std::string rank;
	std::string mmr;
	std::string games;
};

using PPlaylist = jlg::rl::lobby_ranks::Playlist;
struct Player
{
	std::string name;
	std::map<PPlaylist, PlaylistData> data;
};

class LobbyRanks : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{
	public:
		struct Vars {
			static const char* enabled;
			static const char* backgroundOpacity;
			static const char* xPosition;
			static const char* yPosition;
			static const char* scale;

			static const char* refresh;
			static const char* cycle;
		};
		struct Input {
			static const char* enabled;
			static const char* next;
			static const char* autoCycle;

			static const char* cycle;
			static const char* refresh;
		};

	public:
		virtual void onLoad();
		virtual void onUnload();

		virtual void RenderSettings();
		virtual std::string GetPluginName();
		virtual void SetImGuiContext(uintptr_t ctx);

		void refreshLobbyRanks();

		bool isEnabled();
		bool setEnabled(bool enabled);

		void log(std::string str);
		void render(CanvasWrapper canvas);
		void draw( CanvasWrapper& canvas );
		Vector2 center( CanvasWrapper& canvas, const std::string& txt, const Vector2& cell, float scale );

		std::string to_fstring( float val, int len );
		std::string GetRankName( Rank rank, int div );

		// std::shared_ptr<bool> refreshTable = std::make_shared<bool>(true);
		std::map<long long, Player> playerz = {};
		int displayType = 0;

		std::shared_ptr<bool> enabled = std::make_shared<bool>(true);
};

}; }; }; // END namespace jlg::rl::lobby_ranks
