#include "lobby-ranks.h"

#include "util.h"

#include <iomanip>
#include <sstream>

#include <algorithm>
#include <array>
#include <list>
#include <numeric>

#define LR_CVAR_PREFIX "jlg_lobby_rank_"

BAKKESMOD_PLUGIN( jlg::rl::lobby_ranks::LobbyRanks, "Lobby Ranks", plugin_version, PLUGINTYPE_FREEPLAY )

using namespace jlg::rl::lobby_ranks;

const char* LobbyRanks::OnOpenScoreboard =  "Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard";
const char* LobbyRanks::OnCloseScoreboard = "Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard";
const char* LobbyRanks::OnTeamChanged =     "Function TAGame.PRI_TA.OnTeamChanged";
const char* LobbyRanks::GetBotName =        "Function TAGame.PRI_TA.GetBotName";
const char* LobbyRanks::OnMatchEnded =      "Function TAGame.GameEvent_Soccar_TA.OnMatchEnded";
const char* LobbyRanks::PlayerLeft =        "Function TAGame.GRI_TA.Destroyed";
const char* LobbyRanks::OnAllTeamsCreated = "Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated";
const char* LobbyRanks::ReplayEnd =         "Function ReplayDirector_TA.Playing.EndState";
const char* LobbyRanks::ReplayBegin =       "Function ReplayDirector_TA.Playing.BeginState";

const char* LobbyRanks::Vars::enabled =           LR_CVAR_PREFIX "enabled";
const char* LobbyRanks::Vars::refresh =           LR_CVAR_PREFIX "refresh";

const char* LobbyRanks::Vars::backgroundOpacity = LR_CVAR_PREFIX "background_opacity";
const char* LobbyRanks::Vars::xPosition =         LR_CVAR_PREFIX "x_position";
const char* LobbyRanks::Vars::yPosition =         LR_CVAR_PREFIX "y_position";
const char* LobbyRanks::Vars::scale =             LR_CVAR_PREFIX "scale";

const char* LobbyRanks::Input::enabled = "P";
const char* LobbyRanks::Input::refreshL = "LeftShift+P";
const char* LobbyRanks::Input::refreshR = "RightShift+P";

void LobbyRanks::RenderSettings() {}
std::string LobbyRanks::GetPluginName()
{
	return "Lobby Ranks";
}
void LobbyRanks::SetImGuiContext( uintptr_t ctx ) {}
#pragma region Lobby Ranks

void LobbyRanks::onLoad()
{
	auto enableCVAR = cvarManager->registerCvar( Vars::enabled, "1", "Display Lobby Ranks", true, true, 0, true, 1, true );
	enableCVAR.addOnValueChanged(
			[this]( std::string oldValue, CVarWrapper cvar )
			{
				setEnabled( oldValue == "0" );
			} );

	// Regular Variables
	cvarManager->registerCvar( Vars::backgroundOpacity, "200",   "Background opacity", true, true, 0,   true, 255,  true );
	cvarManager->registerCvar( Vars::xPosition,         "0.800", "Overlay X position", true, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Vars::yPosition,         "0.080", "Overlay Y position", true, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Vars::scale,             "1",     "Overlay scale",      true, true, 0.5, true, 3,    true );

	cvarManager->setBind( Input::enabled, Vars::enabled );
	cvarManager->setBind( Input::refreshL, Vars::refresh );
	cvarManager->setBind( Input::refreshR, Vars::refresh );

	bindEvent( OnOpenScoreboard,  [&]() { setEnabled( true ); } );
	bindEvent( OnCloseScoreboard, [&]() { setEnabled( false ); } );
	bindEvent( OnTeamChanged,     [&]() { refresh(); } );
	bindEvent( GetBotName,        [&]() { refresh(); } );
	bindEvent( PlayerLeft,        [&]() { refresh(); } );
	bindEvent( OnAllTeamsCreated, [&]() { refresh(); } );

	cvarManager->registerNotifier(
		Vars::enabled,
		[this]( std::vector<std::string> ) {
			setEnabled( !isEnabled() );
			cvarManager->getCvar(Vars::enabled).setValue( isEnabled() );
		},
		"Toggle Lobby Ranks Plugin",
		PERMISSION_ALL );

	cvarManager->registerNotifier(
		Vars::refresh,
		[this]( std::vector<std::string> ) {
			refresh();
		},
		"Refresh Lobby Ranks Table",
		PERMISSION_ALL );

	gameWrapper->RegisterDrawable( std::bind( &LobbyRanks::render, this, std::placeholders::_1 ) );

	// Apparently this wont run unless it happens a bit later
	gameWrapper->SetTimeout(
		[this]( GameWrapper* gw )
		{
			if( cvarManager->getCvar(Vars::enabled).getBoolValue() )
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
	if( isEnabled() )
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

void LobbyRanks::drawTable( CanvasWrapper& canvas )
{
	if( recalculate )
		resizeTable( canvas );

	if( table.empty() ) return;

	auto mw = gameWrapper->GetMMRWrapper();
	jlg::rl::lobby_ranks::Playlist p =
		jlg::rl::lobby_ranks::Playlist( mw.GetCurrentPlaylist() );
	/*size_t playListIndex = 0;
	auto piter = Playlists.begin();
	while( piter != Playlists.end() && *piter != p )
	{ playListIndex++; piter++; }*/

	float scale = 1.0;// cvarManager->getCvar( Vars::scale ).getFloatValue();
	float xpos = cvarManager->getCvar(Vars::xPosition).getFloatValue();
	float ypos = cvarManager->getCvar(Vars::yPosition).getFloatValue();
	float opacity = (cvarManager->getCvar(Vars::backgroundOpacity).getIntValue() / 255.0f);

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
		/*if( c != playListIndex + 1 )
			color *= 0.8;*/
		
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

#pragma endregion
