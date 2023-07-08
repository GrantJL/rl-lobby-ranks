#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "types.h"
#include "Player.h"
#include "Config.h"

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
			static const char* showWithScoreboard;

			static const char* playlists;

			static const char* backgroundOpacity;
			static const char* xPosition;
			static const char* yPosition;
			static const char* xAnchor;
			static const char* yAnchor;
			static const char* scale;
			static const char* displayPlatform;
		};

		struct Command {
			static const char* toggleShow;
			static const char* refresh;

			static const char* debug;
		};
		struct Input {};

		struct Anchor {
			enum class Horizontal {
				Left = 0,
				Center = 1,
				Right = 2
			};
			enum class Verticle {
				Top = 0,
				Center = 1,
				Bottom = 2
			};
		};

		const std::list<jlg::Playlist> Playlists = {
			jlg::Playlist::Solo,
			jlg::Playlist::Twos,
			jlg::Playlist::Threes,
			jlg::Playlist::Tournament };

	//------------------- BakkesModPlugin ----------------------
	private:
		virtual void onLoad() override;
		virtual void onUnload() override;

	//----------------- PluginSettingsWindow -------------------
	public:
		virtual void RenderSettings() override;
		virtual std::string GetPluginName() override;
		virtual void SetImGuiContext( uintptr_t ctx ) override;

	//------------------- INSTANCE METHODS ---------------------
	private:
		void bindEvent( const char* event, const std::function<void()>& f );

		void sync();
		void refresh();
		void updatePlayers();

		Table getTable();
		void resizeTable( CanvasWrapper c, Table& table );

		bool isInGame();
		ServerWrapper getActiveGameServer();

		bool isEnabled();
		bool setEnabled( bool enabled );
		bool isVisible();
		bool setVisible( bool visible );
		bool isScoreboardOpen();
		bool setScoreboardOpen( bool open );
		bool isShownWithSb();
		bool setShowWithSb( bool showWithSb );

		void render( CanvasWrapper canvas );
		void drawTable( CanvasWrapper& canvas );

		void drawTable( CanvasWrapper& canvas, Table& table );

		void debugPrint();

		Table buildExampleTable();

	//------------------ INSTANCE VARIABLES --------------------
	private:
		// Flag to indicate table sizing need update using CanvasWrapper
		bool recalculate = false;

		Table table;
		Table exampleTable;
		std::list<Player> players;
		Config* config;

		std::shared_ptr<bool> enabled = std::make_shared<bool>( true );
		std::shared_ptr<bool> displayPlatform = std::make_shared<bool>( true );
		std::shared_ptr<bool> visible = std::make_shared<bool>( false );
		std::shared_ptr<bool> showWithSb  = std::make_shared<bool>( false );
		std::shared_ptr<bool> sbOpen  = std::make_shared<bool>( false );

		std::shared_ptr<Vector2F> tablePosition = std::make_shared<Vector2F>( Vector2F{0.0f, 0.0f} );
		std::shared_ptr<Vector2F> anchorOffset = std::make_shared<Vector2F>( Vector2F{0.0f, 0.0f} );
};

}; // END namespace jlg
