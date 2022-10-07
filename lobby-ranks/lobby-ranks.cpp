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

const char* LobbyRanks::GameEvent::OnOpenScoreboard =  "Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard";
const char* LobbyRanks::GameEvent::OnCloseScoreboard = "Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard";
const char* LobbyRanks::GameEvent::OnTeamChanged =     "Function TAGame.PRI_TA.OnTeamChanged";
const char* LobbyRanks::GameEvent::GetBotName =        "Function TAGame.PRI_TA.GetBotName";
const char* LobbyRanks::GameEvent::OnMatchEnded =      "Function TAGame.GameEvent_Soccar_TA.OnMatchEnded";
const char* LobbyRanks::GameEvent::PlayerLeft =        "Function TAGame.GRI_TA.Destroyed";
const char* LobbyRanks::GameEvent::OnAllTeamsCreated = "Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated";
const char* LobbyRanks::GameEvent::ReplayEnd =         "Function ReplayDirector_TA.Playing.EndState";
const char* LobbyRanks::GameEvent::ReplayBegin =       "Function ReplayDirector_TA.Playing.BeginState";

const char* LobbyRanks::Var::enabled =            LR_CVAR_PREFIX "enabled";
//const char* LobbyRanks::Var::shown =              LR_CVAR_PREFIX "show";
//const char* LobbyRanks::Var::scoreboardOpen =     LR_CVAR_PREFIX "scoreboard_open";
const char* LobbyRanks::Var::showWithScoreboard = LR_CVAR_PREFIX "show_w_scoreboard";
const char* LobbyRanks::Var::playlists =          LR_CVAR_PREFIX "playlists";
const char* LobbyRanks::Var::backgroundOpacity =  LR_CVAR_PREFIX "background_opacity";
const char* LobbyRanks::Var::xPosition =          LR_CVAR_PREFIX "x_position";
const char* LobbyRanks::Var::yPosition =          LR_CVAR_PREFIX "y_position";
const char* LobbyRanks::Var::xAnchor =            LR_CVAR_PREFIX "x_anchor";
const char* LobbyRanks::Var::yAnchor =            LR_CVAR_PREFIX "y_anchor";
const char* LobbyRanks::Var::scale =              LR_CVAR_PREFIX "scale";


const char* LobbyRanks::Command::refresh =    LR_CMD_PREFIX "refresh";
const char* LobbyRanks::Command::toggleShow = LR_CMD_PREFIX "toggle";
const char* LobbyRanks::Command::debug =      "jlg_debug";

void LobbyRanks::onLoad()
{
	// Regular Variables
	auto cvar_enab = cvarManager->registerCvar( Var::enabled,            "1",   "Enable Lobby Ranks", true,  true, 0, true, 1,    true );
	auto cvar_show = cvarManager->registerCvar( Var::showWithScoreboard, "1",   "Show Lobby Ranks W/ Scoreboard", true,  true, 0, true, 1,    true );
	auto cvar_back = cvarManager->registerCvar( Var::backgroundOpacity,  "200", "Lobby Ranks Background opacity", false, true, 0, true, 255,  true );
	auto cvar_xPos = cvarManager->registerCvar( Var::xPosition,          "0",   "Lobby Ranks Table Position X",   false, true, 0, true, 1.0f, true );
	auto cvar_yPos = cvarManager->registerCvar( Var::yPosition,          "0",   "Lobby Ranks Table Position Y",   false, true, 0, true, 1.0f, true );
	auto cvar_xAnc = cvarManager->registerCvar( Var::xAnchor,            "0",   "Lobby Ranks Table Anchor X",     false, true, 0, true, 2,    true );
	auto cvar_yAnc = cvarManager->registerCvar( Var::yAnchor,            "0",   "Lobby Ranks Table Anchor Y",     false, true, 0, true, 2,    true );
	auto cvar_scal = cvarManager->registerCvar( Var::scale,              "1",   "Lobby Ranks Overlay scale",      false, true, 0.5, true, 3,  true );
	auto cvar_play = cvarManager->registerCvar( Var::playlists, "10 11 13 34", "Lobby Ranks shown playlists", false );

	cvar_enab.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			setEnabled( old == "0" );
		});
	cvar_show.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			setShowWithSb( old == "0" );
		});
	cvar_xPos.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			tablePosition->X = cv.getFloatValue();
		});
	cvar_yPos.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			tablePosition->Y = cv.getFloatValue();
		});
	cvar_xAnc.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			anchorOffset->X = cv.getFloatValue() / 2.0f;
		});
	cvar_yAnc.addOnValueChanged(
		[this]( std::string old, CVarWrapper cv ) {
			anchorOffset->Y = cv.getFloatValue() / 2.0f;
		});

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

	// Sync stored plugin settings
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
	auto cvar_enabled = cvarManager->getCvar( Var::enabled );
	if( cvar_enabled )
		setEnabled( cvar_enabled.getBoolValue() );

	auto cvar_showWithScoreboard = cvarManager->getCvar( Var::showWithScoreboard );
	if( cvar_showWithScoreboard )
		setShowWithSb( cvar_showWithScoreboard.getBoolValue() );

	auto cvar_xPosition = cvarManager->getCvar(Var::xPosition);
	auto cvar_yPosition = cvarManager->getCvar(Var::yPosition);
	if( cvar_xPosition && cvar_yPosition )
	{
		tablePosition->X = cvar_xPosition.getFloatValue();
		tablePosition->Y = cvar_yPosition.getFloatValue();
	}
	auto cvar_xAnchor = cvarManager->getCvar(Var::xAnchor);
	auto cvar_yAnchor = cvarManager->getCvar(Var::yAnchor);
	if( cvar_xAnchor && cvar_yAnchor )
	{
		anchorOffset->X = cvar_xAnchor.getFloatValue() / 2.0f;
		anchorOffset->Y = cvar_yAnchor.getFloatValue() / 2.0f;
	}

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
	int l = 40; int d = 20; int a = 255;
	LinearColor light{ l, l, l, a };
	LinearColor dark{ d, d, d, a };
	LinearColor black{ 0, 0, 0, a };
	Vector2F pad{ 8.0f, 2.5f };

	auto getHeader = [&]() {
		Table::Row row;
		row.push_back( Table::Cell( "Name", Color::White, black, Table::Align::Left ) );
		row.back().setPadding( pad.X, pad.Y );
		for( auto p : LobbyRanks::Playlists )
		{
			row.push_back( Table::Cell( util::toString<Playlist>(p), Color::White, black ) );
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
			LobbyRanks::Playlists,
			pad );
		for( auto& row : playerRows )
			ptable.addRow( row );
		even = !even;
	}

	return ptable;
}

void LobbyRanks::resizeTable( CanvasWrapper c )
{
	auto calcSize = [&]( Table::Cell& cell, size_t row, size_t col ) {
		cell.valueSize = c.GetStringSize( cell.value );
		cell.size = cell.valueSize;
	};
	table.forEach( calcSize );
	table.fitSize();
	table.realign();
	recalculate = false;
}

void LobbyRanks::onUnload() {}

void LobbyRanks::render( CanvasWrapper canvas )
{
	if( !isEnabled() ) return;

	if( isVisible() || (isScoreboardOpen() && isShownWithSb()) )
		drawTable( canvas );
}

bool LobbyRanks::isEnabled()
{
	return *enabled;
}
bool LobbyRanks::setEnabled( bool enabled )
{
	this->enabled = std::make_shared<bool>( enabled );
	return enabled;
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
bool LobbyRanks::isShownWithSb()
{
	return *showWithSb;
}
bool LobbyRanks::setShowWithSb( bool showWithSb )
{
	this->showWithSb = std::make_shared<bool>( showWithSb );
	return showWithSb;
}

void LobbyRanks::drawTable( CanvasWrapper& canvas )
{
	if( recalculate )
		resizeTable( canvas );

	if( table.empty() ) return;

	auto mw = gameWrapper->GetMMRWrapper();
	jlg::Playlist p = jlg::Playlist( mw.GetCurrentPlaylist() );
	/*size_t playListIndex = 0;
	auto piter = Playlists.begin();
	while( piter != Playlists.end() && *piter != p )
	{ playListIndex++; piter++; }*/

	float scale = 1.0;// cvarManager->getCvar( Var::scale ).getFloatValue();
	float xpos = cvarManager->getCvar(Var::xPosition).getFloatValue();
	float ypos = cvarManager->getCvar(Var::yPosition).getFloatValue();
	float opacity = (cvarManager->getCvar(Var::backgroundOpacity).getIntValue() / 255.0f);

	Vector2F Origin = {
		0.0f,
		0.0f
	};
	Vector2F CanvasSize = {
		canvas.GetSize().X,
		canvas.GetSize().Y
	};

	auto xp = cvarManager->getCvar( Var::xPosition );
	auto yp = cvarManager->getCvar( Var::yPosition );
	Vector2F TableOrigin = CanvasSize * (*tablePosition);
	TableOrigin -= table.size() * (*anchorOffset);

	bool tickTock = true;
	auto drawCell = [&]( const Table::Cell& cell, size_t r, size_t c ) {
		auto cellOrigin = TableOrigin + table.getCellOrigin( r, c );
		auto color = cell.color;
		/*if( c != 0 && c != playListIndex + 1 )
			color.A *= 0.8;*/

		auto bg = cell.backgroundColor;
		bg.A = 200;
		// don checker borad columns
		/*auto bgo = bg.A;
		if( c % 2 == 0 ){
			bg *= 0.8; bg.A = bgo;
		}*/
		canvas.SetColor( bg );
		canvas.SetPosition( cellOrigin );
		canvas.FillBox( cell.paddedSize() );

		bool debugDraw = false;
		if( debugDraw )
		{
			// Checkboard cells
			if( tickTock ){
				canvas.SetPosition( cellOrigin );
				canvas.SetColor( LinearColor{ 255, 255, 255, 50 } );
				canvas.FillBox( cell.paddedSize() );
			}
			// Draw Padding Inner
			canvas.SetColor( LinearColor{ 0, 255, 255, 80 } );
			canvas.SetPosition( cellOrigin + Vector2F{ cell.padLeft, cell.padTop } );
			canvas.FillBox( cell.size );

			// Draw Value Size
			canvas.SetColor( LinearColor{ 255, 0, 255, 80 } );
			canvas.SetPosition( cellOrigin + Vector2F{ cell.padLeft, cell.padTop } + cell.valueOffset );
			canvas.FillBox( cell.valueSize );
		}

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
	static const auto vars = {
		Var::enabled, Var::showWithScoreboard,
		Var::backgroundOpacity, Var::scale,
		Var::xPosition, Var::yPosition,
		Var::xAnchor, Var::yAnchor };

	std::stringstream ss;
	ss << std::endl;
	for( const auto& v : vars )
		ss << '\t' << v << ": " << cvarManager->getCvar(v).getStringValue() << std::endl;
	cvarManager->log( ss.str() );
}
