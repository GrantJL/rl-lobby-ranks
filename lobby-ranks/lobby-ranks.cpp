#include "lobby-ranks.h"

#include "imgui/imgui.h"

#include "util.h"

#include <iomanip>
#include <sstream>

#include <algorithm>
#include <array>
#include <list>
#include <numeric>

#define LR_CMD_PREFIX "jlg_lobby_rank_"
#define LR_CVAR_PREFIX LR_CMD_PREFIX "v_"

BAKKESMOD_PLUGIN( jlg::LobbyRanks, "Lobby Ranks", plugin_version, PLUGINTYPE_FREEPLAY )

using namespace jlg;

#pragma region Example table
Table LobbyRanks::buildExampleTable()
{
	Table ptable;
	Vector2F pad{ 8.0f, 2.5f };
	Config* cfg = config;

	auto fg = cfg->tableFg();
	auto bg = cfg->tableBg();
	auto light = cfg->tableEven();
	auto dark = cfg->tableOdd();

	auto getHeader = [&]() {
		Table::Row row;
		row.push_back( Table::Cell( "Name", fg, bg, Table::Align::Left ) );
		row.back().setPadding( pad.X, pad.Y );
		for( auto p : cfg->getPlaylists() )
		{
			row.push_back( Table::Cell( util::toString<Playlist>(p), fg, bg ) );
			row.back().setPadding( pad.X, pad.Y );
		}
		return row;
	};

	std::list<Player> exmaplePlayers = {
		Player( "Charlie",   Team::Orange, Platform::Unknown,     false, false, Rank::Unranked, Rank::Bronze1 ),
		Player( "Foxtrot",   Team::Orange, Platform::Steam,       true,  false, Rank::Silver1, Rank::Gold1 ),
		Player( "Juliet",    Team::Orange, Platform::Playstation, false, false, Rank::Platinum1, Rank::Diamond1 ),
		Player( "Mike",      Team::Blue,   Platform::Xbox,        false, false, Rank::Diamond3, Rank::Champ2 ),
		Player( "Oscar",     Team::Blue,   Platform::Nintendo,    false, false, Rank::GrandChamp2, Rank::GrandChamp3 ),
		Player( "Sierra",    Team::Blue,   Platform::Epic,        false, false, Rank::SupersonicLegend, Rank::SupersonicLegend ),
	};

	ptable.addRow( getHeader() );
	bool even = true;
	for( auto& player : exmaplePlayers )
	{
		auto playerRows = player.row(
			(even ? light : dark),
			cfg->getPlaylists(),
			pad );
		for( auto& row : playerRows )
			ptable.addRow( row );
		even = !even;
	}

	return ptable;
};
#pragma endregion


const char* LobbyRanks::GameEvent::OnOpenScoreboard =  "Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard";
const char* LobbyRanks::GameEvent::OnCloseScoreboard = "Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard";
const char* LobbyRanks::GameEvent::OnTeamChanged =     "Function TAGame.PRI_TA.OnTeamChanged";
const char* LobbyRanks::GameEvent::GetBotName =        "Function TAGame.PRI_TA.GetBotName";
const char* LobbyRanks::GameEvent::OnMatchEnded =      "Function TAGame.GameEvent_Soccar_TA.OnMatchEnded";
const char* LobbyRanks::GameEvent::PlayerLeft =        "Function TAGame.GRI_TA.Destroyed";
const char* LobbyRanks::GameEvent::OnAllTeamsCreated = "Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated";
const char* LobbyRanks::GameEvent::ReplayEnd =         "Function ReplayDirector_TA.Playing.EndState";
const char* LobbyRanks::GameEvent::ReplayBegin =       "Function ReplayDirector_TA.Playing.BeginState";

const char* LobbyRanks::Var::backgroundOpacity =  LR_CVAR_PREFIX "background_opacity";


const char* LobbyRanks::Command::enable =     LR_CMD_PREFIX "enable";
const char* LobbyRanks::Command::refresh =    LR_CMD_PREFIX "refresh";
const char* LobbyRanks::Command::toggleShow = LR_CMD_PREFIX "toggle";
const char* LobbyRanks::Command::debug =      "jlg_debug";

void LobbyRanks::onLoad()
{
	Config::initialize( cvarManager );
	config = Config::instance();

	// Regular Variables
	auto cvar_back = cvarManager->registerCvar( Var::backgroundOpacity,  "200", "Lobby Ranks Background opacity", false, true, 0, true, 255,  true );

	exampleTable = buildExampleTable();

	// Commands
	cvarManager->registerCvar( Command::toggleShow, "0",     "Toggle show Lobby Ranks table",  true, false, .0f, false, .0f, false );
	cvarManager->registerCvar( Command::refresh,    "0",     "Refresh Lobby Ranks table", true, false, .0f, false, .0f, false );
	cvarManager->registerCvar( Command::debug,      "0",     "Debug Lobby Ranks",  false, false, .0f, false, .0f, false );

	bindEvent( GameEvent::OnOpenScoreboard,  [&]() { refresh(); setScoreboardOpen( true ); } );
	bindEvent( GameEvent::OnCloseScoreboard, [&]() { refresh(); setScoreboardOpen( false ); } );
	bindEvent( GameEvent::OnTeamChanged,     [&]() { refresh(); } );
	bindEvent( GameEvent::GetBotName,        [&]() { refresh(); } );
	bindEvent( GameEvent::PlayerLeft,        [&]() { refresh(); } );
	bindEvent( GameEvent::OnAllTeamsCreated, [&]() { refresh(); } );

	cvarManager->registerNotifier(
		Command::enable,
		[this]( std::vector<std::string> cmd) {
			try
			{
				int enabled = config->isEnabled();
			
				if( cmd.size() <= 1 )
					enabled = !enabled;
				else
				{
					std::stringstream ss( cmd[1] );
					ss >> enabled;
				}

				config->setEnabled( enabled );
				cvarManager->log( (enabled ? "jlg_lobby_rank enabled" : "jlg_lobby_rank disabled") );
			}
			catch( ... )
			{
				cvarManager->log( "jlg_lobby_rank_enabled error: use `jlg_lobby_rank_enabled` to toggle, `jlg_lobby_rank_enabled 0` to disable, `jlg_lobby_rank_enabled 1` to enable" );
			}
		},
		"Enable Lobby Ranks Plugin",
			PERMISSION_ALL );
	cvarManager->registerNotifier(
		Command::toggleShow,
		[this]( std::vector<std::string> ) {
			setVisible(!isVisible());
		},
		"Toggle Lobby Ranks Plugin",
		PERMISSION_ALL );

	cvarManager->registerNotifier(
		Command::refresh,
		[this]( std::vector<std::string> ) {
			refresh();
		},
		"Refresh Lobby Ranks Table",
		PERMISSION_ALL );

	cvarManager->registerNotifier(
		Command::debug,
		[this]( std::vector<std::string> ) {
			debugPrint();
		},
		"",
		PERMISSION_ALL );

	gameWrapper->RegisterDrawable( std::bind( &LobbyRanks::render, this, std::placeholders::_1 ) );

	// Sync stored plugin settings // Always delay config loading, if we do it here it will initilize to the registered defaults
	gameWrapper->SetTimeout(
		[this]( GameWrapper* gw ) { sync(); },
		1 );
}

void LobbyRanks::bindEvent( const char* event, const std::function<void()>& f )
{
	gameWrapper->HookEvent( event, std::bind(f) );
}

void LobbyRanks::refresh()
{
	updatePlayers();
	table = getTable();
	recalculate = true;
}

void LobbyRanks::sync()
{
	config->loadFromCfg();

	// JLG TODO Sync other cvars
}

bool LobbyRanks::isInGame()
{
	if( gameWrapper->IsInReplay() ) return false;
	if( gameWrapper->IsInFreeplay() ) return false;

	return gameWrapper->IsInGame() || gameWrapper->IsInOnlineGame();
}

ServerWrapper LobbyRanks::getActiveGameServer()
{
	if( gameWrapper->IsInOnlineGame() )
		return gameWrapper->GetOnlineGame();
	else
		return gameWrapper->GetGameEventAsServer();
}

void LobbyRanks::updatePlayers()
{
	// JLG TODO isActive()

	if( !isInGame() ) return;
	auto server = getActiveGameServer();
	if( server.IsNull() )
		server = gameWrapper->GetOnlineGame();
	if( server.IsNull() ) return;

	// we dont update, we just delete and yeet (new data)
	players.clear();
	auto pris = server.GetPRIs();
	for( auto i = 0; i < pris.Count(); i++ )
		players.push_back( Player(gameWrapper, pris.Get(i)) );

	// ensure teams are together
	// JLG TODO Match scoreboard order?
	std::stable_sort( players.rbegin(), players.rend(),
		[]( const Player& a, const Player& b ) -> bool {
			return int(a.team) < int(b.team);
		} );
	std::reverse( players.begin(), players.end() );
}

Table LobbyRanks::getTable()
{
	Table ptable;
	auto fg = config->tableFg();
	auto bg = config->tableBg();
	auto light = config->tableEven();
	auto dark = config->tableOdd();
	auto playlists = config->getPlaylists();
	Vector2F pad{ 8.0f, 2.5f };

	auto getHeader = [&]() {
		Table::Row row;
		row.push_back( Table::Cell( "Name", fg, bg , Table::Align::Left ) );
		row.back().setPadding( pad.X, pad.Y );
		for( auto p : playlists )
		{
			row.push_back( Table::Cell( util::toString<Playlist>(p), fg, bg ) );
			row.back().setPadding( pad.X, pad.Y );
		}
		return row;
	};

	ptable.addRow( getHeader() );
	bool even = true;
	for( auto& player : players )
	{
		auto playerRows = player.row(
			(even ? light : dark),
			playlists,
			pad );
		for( auto& row : playerRows )
			ptable.addRow( row );
		even = !even;
	}

	return ptable;
}

void LobbyRanks::resizeTable( CanvasWrapper c, Table& t )
{
	auto calcSize = [&]( Table::Cell& cell, size_t row, size_t col ) {
		cell.valueSize = c.GetStringSize( cell.value );
		cell.size = cell.valueSize;
	};
	t.forEach( calcSize );
	t.fitSize();
	t.realign();
}

void LobbyRanks::onUnload()
{
	cvarManager->backupCfg( "back_jlg_cfg");

	Config* c = Config::instance();
	delete c;
	c = nullptr;
}

void LobbyRanks::render( CanvasWrapper canvas )
{
	if( !config->isEnabled() ) return;

	if( isVisible() || (isScoreboardOpen() && config->isDisplayWithScoreboard()) )
	{
		if( recalculate )
		{
			resizeTable( canvas, table );
			recalculate = false;
		}

		drawTable( canvas, table );
	}

	if( config->isExampleDisplayed() )
	{
		if( config->isRefreshExampleTable() )
		{
			exampleTable = buildExampleTable();
			resizeTable( canvas, exampleTable );
			config->isRefreshExampleTable( false );
		}
		drawTable( canvas, exampleTable );
	}
}

bool LobbyRanks::isVisible()
{
	return *visible;
}
bool LobbyRanks::setVisible(bool visible)
{
	this->visible = std::make_shared<bool>( visible );
	return visible;
}
bool LobbyRanks::isScoreboardOpen()
{
	return *sbOpen;
}
bool LobbyRanks::setScoreboardOpen(bool open)
{
	this->sbOpen = std::make_shared<bool>( open );
	return open;
}

void LobbyRanks::drawTable( CanvasWrapper& canvas, Table& table )
{
	if( table.empty() ) return;

	float scale = 1.0;// cvarManager->getCvar( Var::scale ).getFloatValue();
	auto [xpos, ypos] = config->getTablePosition();
	float opacity = (cvarManager->getCvar(Var::backgroundOpacity).getIntValue() / 255.0f);

	Vector2F Origin = {
		0.0f,
		0.0f
	};
	Vector2F CanvasSize = {
		canvas.GetSize().X,
		canvas.GetSize().Y
	};
	Vector2F TableOrigin = CanvasSize * config->getTablePosition();
	TableOrigin -= table.size() * config->getTableAnchor();

	bool tickTock = true;
	auto drawCell = [&]( const Table::Cell& cell, size_t r, size_t c ) {
		auto cellOrigin = TableOrigin + table.getCellOrigin( r, c );
		auto color = cell.color;

		auto bg = cell.backgroundColor;
		bg.A = 200;
		canvas.SetColor( bg );
		canvas.SetPosition( cellOrigin );
		canvas.FillBox( cell.paddedSize() );

		canvas.SetColor( color );
		canvas.SetPosition( cellOrigin + Vector2F{ cell.padLeft, cell.padTop } + cell.valueOffset );
		canvas.DrawString( cell.value, scale, scale );
		tickTock = !tickTock;

		// Draw lines
		if( c != 0 )
		{
			canvas.SetColor( 255, 255, 255, 25 );
			canvas.DrawLine( cellOrigin, cellOrigin + Vector2F{ 0.0f, cell.paddedSize().Y }, 1.0f );
		}
		/*if( (r-1) % 3 == 0 )
		{
			canvas.SetColor( 255, 255, 255, 25 );
			canvas.DrawLine( cellOrigin, cellOrigin + Vector2F{ cell.paddedSize().X, 0.0f }, 1.0f );
		}*/
	};
	table.forEach( drawCell );
}

void LobbyRanks::debugPrint()
{
	cvarManager->log( config->debug() );
}
