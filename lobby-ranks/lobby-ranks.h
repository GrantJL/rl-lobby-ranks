#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "types.h"
#include "Player.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

namespace jlg { namespace rl { namespace lobby_ranks {

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
		};
		struct Input {
			static const char* enabled;
			static const char* refresh;
		};

		const std::list<jlg::rl::lobby_ranks::Playlist> Playlists = {
			jlg::rl::lobby_ranks::Playlist::Solo,
			jlg::rl::lobby_ranks::Playlist::Twos,
			jlg::rl::lobby_ranks::Playlist::Threes,
			jlg::rl::lobby_ranks::Playlist::Tournament };

	public:
		virtual void onLoad();
		virtual void onUnload();

		virtual void RenderSettings();
		virtual std::string GetPluginName();
		virtual void SetImGuiContext(uintptr_t ctx);

	private:
		void refresh();
		void updatePlayers();
		Table getTable();
		void resizeTable( CanvasWrapper c );

		bool isInGame();
		ServerWrapper getActiveGameServer();

		bool isEnabled();
		bool setEnabled(bool enabled);

		// For each column:
		//     drawColor( table[x][y].color );
		//     if( x != 0 && playlist != mmrWrapper.GetCurrentPlaylist() )
		//         drawColor *= 0.75;
		void render(CanvasWrapper canvas);
		void drawTable( CanvasWrapper& canvas );
	
	private:
		bool recalculate = false;
		std::list<Player> players;
		Table table;
		std::shared_ptr<bool> enabled = std::make_shared<bool>(true);
};

}; }; }; // END namespace jlg::rl::lobby_ranks
