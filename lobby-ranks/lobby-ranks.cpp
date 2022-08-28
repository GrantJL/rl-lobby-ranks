#include "lobby-ranks.h"

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

const char* LobbyRanks::Var::enabled =           LR_CVAR_PREFIX "enabled";
const char* LobbyRanks::Var::shown =             LR_CVAR_PREFIX "show";
const char* LobbyRanks::Var::scoreboardOpen =    LR_CVAR_PREFIX "scoreboard_open";
const char* LobbyRanks::Var::backgroundOpacity = LR_CVAR_PREFIX "background_opacity";
const char* LobbyRanks::Var::xPosition =         LR_CVAR_PREFIX "x_position";
const char* LobbyRanks::Var::yPosition =         LR_CVAR_PREFIX "y_position";
const char* LobbyRanks::Var::scale =             LR_CVAR_PREFIX "scale";


const char* LobbyRanks::Command::refresh =    LR_CMD_PREFIX "refresh";
const char* LobbyRanks::Command::toggleShow = LR_CMD_PREFIX "toggle";
const char* LobbyRanks::Command::debug =      "jlg_debug";

const char* LobbyRanks::Input::toggleShow = "P";

void LobbyRanks::RenderSettings() {}
std::string LobbyRanks::GetPluginName()
{
	return "Lobby Ranks";
}
void LobbyRanks::SetImGuiContext( uintptr_t ctx ) {}

#pragma region Lobby Ranks

void LobbyRanks::onLoad()
{
	auto enableCVAR = cvarManager->registerCvar( Var::enabled, "1", "Enable Lobby Ranks", true, true, 0, true, 1, true );
	enableCVAR.addOnValueChanged(
			[this]( std::string oldValue, CVarWrapper cvar )
			{
				setEnabled( oldValue == "0" );
			} );

	// Regular Variables
	cvarManager->registerCvar( Var::shown,             "0",     "Show Ranks Table",   true,  true, 0,   true, 1,    false );
	cvarManager->registerCvar( Var::scoreboardOpen,    "0",     "Scoreboard Visible", false, true, 0,   true, 1,    false );
	cvarManager->registerCvar( Var::backgroundOpacity, "200",   "Background opacity", false, true, 0,   true, 255,  true );
	cvarManager->registerCvar( Var::xPosition,         "0.800", "Overlay X position", false, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Var::yPosition,         "0.080", "Overlay Y position", false, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Var::scale,             "1",     "Overlay scale",      false, true, 0.5, true, 3,    true );

	// Commands
	cvarManager->registerCvar( Command::toggleShow, "0",     "Toggle",  true, false, .0f, false, .0f, false );
	cvarManager->registerCvar( Command::refresh,    "0",     "Refresh", true, false, .0f, false, .0f, false );
	cvarManager->registerCvar( Command::debug,      "0",     "Debug",  false, false, .0f, false, .0f, false );

	cvarManager->setBind( Input::toggleShow, Command::toggleShow );

	bindEvent( GameEvent::OnOpenScoreboard,  [&]() { setScoreboardOpen( true ); } );
	bindEvent( GameEvent::OnCloseScoreboard, [&]() { setScoreboardOpen( false ); } );
	bindEvent( GameEvent::OnTeamChanged,     [&]() { refresh(); } );
	bindEvent( GameEvent::GetBotName,        [&]() { refresh(); } );
	bindEvent( GameEvent::PlayerLeft,        [&]() { refresh(); } );
	bindEvent( GameEvent::OnAllTeamsCreated, [&]() { refresh(); } );

	cvarManager->registerNotifier(
		Command::toggleShow,
		[this]( std::vector<std::string> ) {
			setVisible( !isVisible() );
			cvarManager->getCvar(Var::shown).setValue( isVisible() );
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

	// Apparently this wont run unless it happens a bit later
	gameWrapper->SetTimeout(
		[this]( GameWrapper* gw )
		{
			if( cvarManager->getCvar(Var::enabled).getBoolValue() )
				setEnabled( true );
		},
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

	if( isVisible() || (/*scoreBoardEnabled && */ isScoreboardOpen()) )
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
	Vector2F TableOrigin = { // Bottom Left (w/ padding of 40.0)
		0.0 + 10,
		(CanvasSize.Y - table.size().Y) - 120.0
	};

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
		Var::enabled, Var::shown, Var::scoreboardOpen,
		Var::backgroundOpacity, Var::xPosition, Var::yPosition,
		Var::scale };

	std::stringstream ss;
	ss << std::endl;
	for( const auto& v : vars )
		ss << '\t' << v << ": " << cvarManager->getCvar(v).getStringValue() << std::endl;
	cvarManager->log( ss.str() );
}

#pragma endregion
