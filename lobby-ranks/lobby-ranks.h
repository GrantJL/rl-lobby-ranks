#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "types.h"
#include "Player.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

namespace jlg {

class LobbyRanks : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{
	//------------------- STATIC VARIABLES ---------------------
	public:
		struct GameEvent {
			static const char* OnOpenScoreboard;
			static const char* OnCloseScoreboard;
			static const char* OnTeamChanged;
			static const char* GetBotName;
			static const char* PlayerLeft;
			static const char* OnAllTeamsCreated;
			static const char* OnMatchEnded;
			static const char* ReplayEnd;
			static const char* ReplayBegin;
		};

		struct Var {
			static const char* enabled;
			static const char* shown;
			static const char* scoreboardOpen;

			static const char* backgroundOpacity;
			static const char* xPosition;
			static const char* yPosition;
			static const char* scale;

		};
		struct Command {
			static const char* toggleShow;
			static const char* refresh;

			static const char* debug;
		};
		struct Input {
			static const char* toggleShow;
			static const char* refreshL;
			static const char* refreshR;
		};

		const std::list<jlg::Playlist> Playlists = {
			jlg::Playlist::Solo,
			jlg::Playlist::Twos,
			jlg::Playlist::Threes,
			jlg::Playlist::Tournament };

	//------------------- BakkesModPlugin ----------------------
	public:
		virtual void onLoad();
		virtual void onUnload();

	//----------------- PluginSettingsWindow -------------------
	public:
		virtual void RenderSettings();
		virtual std::string GetPluginName();
		virtual void SetImGuiContext( uintptr_t ctx );

	//------------------- INSTANCE METHODS ---------------------
	private:
		void bindEvent( const char* event, const std::function<void()>& f );

		void refresh();
		void updatePlayers();

		Table getTable();
		void resizeTable( CanvasWrapper c );

		bool isInGame();
		ServerWrapper getActiveGameServer();

		bool isEnabled();
		bool setEnabled( bool enabled );
		bool isVisible();
		bool setVisible( bool visible );
		bool isScoreboardOpen();
		bool setScoreboardOpen( bool open );

		void render( CanvasWrapper canvas );
		void drawTable( CanvasWrapper& canvas );

		void debugPrint();

	//------------------ INSTANCE VARIABLES --------------------
	private:
		// Flag to indicate table sizing need update using CanvasWrapper
		bool recalculate = false;

		Table table;
		std::list<Player> players;

		std::shared_ptr<bool> enabled = std::make_shared<bool>( true );
		std::shared_ptr<bool> visible = std::make_shared<bool>( false );
		std::shared_ptr<bool> sbOpen  = std::make_shared<bool>( false );
};

}; // END namespace jlg
