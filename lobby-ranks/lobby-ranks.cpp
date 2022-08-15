#include "lobby-ranks.h"

#include "Enums.h"
#include "RankAndPlaylistData.h"

#include <stdarg.h>
#include <iomanip>
#include <sstream>

#include <algorithm>
#include <array>
#include <list>
#include <numeric>

#define LR_CVAR_PREFIX "jlg_lobby_rank_"

BAKKESMOD_PLUGIN( jlg::rl::lobby_ranks::LobbyRanks, "Lobby Ranks", plugin_version, PLUGINTYPE_FREEPLAY )

using namespace jlg::rl::lobby_ranks;

const char* LobbyRanks::Vars::enabled =           LR_CVAR_PREFIX "enabled";
const char* LobbyRanks::Vars::backgroundOpacity = LR_CVAR_PREFIX "background_opacity";
const char* LobbyRanks::Vars::xPosition =         LR_CVAR_PREFIX "x_position";
const char* LobbyRanks::Vars::yPosition =         LR_CVAR_PREFIX "y_position";
const char* LobbyRanks::Vars::scale =             LR_CVAR_PREFIX "scale";
const char* LobbyRanks::Vars::refresh =           LR_CVAR_PREFIX "refresh";
const char* LobbyRanks::Vars::cycle =             LR_CVAR_PREFIX "cycle";

const char* LobbyRanks::Input::enabled = "P";
const char* LobbyRanks::Input::next = "N";
const char* LobbyRanks::Input::autoCycle = "LeftShift+M";
const char* LobbyRanks::Input::cycle = "O";
const char* LobbyRanks::Input::refresh = "LeftShift+P";



namespace jlg {
	std::string format( const char* format, ... )
	{
		std::string msg;
		va_list args;
		va_start( args, format );

		char buffer[2048];
		if( _vscprintf( format, args ) + 1 <= 2048 )
		{
			vsprintf_s( buffer, format, args );
			msg = buffer;
		}
		va_end( args );

		return msg;
	}
}

typedef std::array<std::string, 9> DataRow;

static std::string center( const std::string& txt, size_t width )
{
	if( txt.empty() )
		return std::string( width, ' ' );

	size_t totalPad = std::max( size_t(0), width - txt.length() );
	size_t leftPad = totalPad / 2;
	size_t rightPad = totalPad - leftPad;
	return jlg::format( "%*s%s%*s", leftPad, "", txt.c_str(), rightPad, "" );
}

std::list<std::string> tabulate( std::list<Player>& rows, int dataType )
{
	static std::string headingPlayer = "Player";
	std::array<size_t, 5> lengths = { 0, 0, 0 ,0 ,0 };

	{ // Calc Heading width
		int i = 0;
		if( headingPlayer.length() > lengths.at( i++ ) )
			lengths[i - 1] = headingPlayer.length();

		for( PPlaylist playlist : { PPlaylist::Solo, PPlaylist::Twos, PPlaylist::Threes, PPlaylist::Tournament } )
		{
			lengths[i] = std::max( lengths[i], name( playlist ).length() );
			i++;
		}
	}

	 // Calc Player row width
	for( const auto& player : rows )
	{
		int i = 0;
		if( player.name.length() > lengths.at( i++ ) )
			lengths[i - 1] = player.name.length();

		for( PPlaylist playlist : { PPlaylist::Solo, PPlaylist::Twos, PPlaylist::Threes, PPlaylist::Tournament } )
		{
			if( player.data.find( playlist ) == player.data.end() ) continue;
			auto& data = player.data.at( playlist );
			auto elems = { lengths[i], data.mmr.length(), data.rank.length(), data.games.length() };
			lengths[i++] = *std::max_element( elems.begin(), elems.end() );
		}
	}

	std::list<std::string> table;
	{
		int i = 0;
		std::stringstream ss;
		ss << std::left << std::setw( lengths[i++] ) << headingPlayer << "    ";
		ss << std::resetiosflags( std::ios_base::adjustfield );
		for( PPlaylist playlist : { PPlaylist::Solo, PPlaylist::Twos, PPlaylist::Threes, PPlaylist::Tournament } )
			ss << center( name(playlist), lengths[i++] ) << "  ";
		table.push_back( ss.str() );
	}
	for( const auto& player : rows )
	{
		int i = 0;
		std::stringstream ss;
		ss << std::left << std::setw( lengths[i++] ) << player.name << "    ";
		ss << std::resetiosflags( std::ios_base::adjustfield );
		for( PPlaylist playlist : { PPlaylist::Solo, PPlaylist::Twos, PPlaylist::Threes, PPlaylist::Tournament } )
		{
			std::string deets;
			if( player.data.find( playlist ) == player.data.end() ) continue;
			auto& data = player.data.at( playlist );
			if( dataType == 0 ) deets = data.rank; else if( dataType == 1 ) deets = data.mmr; else deets = data.games;
			ss << center( deets, lengths[i++] ) << "  ";
		}
		table.push_back( ss.str() );
	}
	return table;
}

void LobbyRanks::RenderSettings() {}
std::string LobbyRanks::GetPluginName()
{
	return "Lobby Ranks";
}
void LobbyRanks::SetImGuiContext( uintptr_t ctx ) {}

void LobbyRanks::log( std::string str )
{
	cvarManager->log( str );
}

void LobbyRanks::onLoad()
{
	auto enableCVAR = cvarManager->registerCvar( Vars::enabled, "1", "Display Lobby Ranks", true, true, 0, true, 1, true );
	enableCVAR.addOnValueChanged(
			[this]( std::string oldValue, CVarWrapper cvar )
			{
				if( setEnabled(oldValue == "0") ) refreshLobbyRanks();
			} );

	cvarManager->registerCvar( Vars::backgroundOpacity, "200",   "Background opacity", true, true, 0,   true, 255,  true );
	cvarManager->registerCvar( Vars::xPosition,         "0.800", "Overlay X position", true, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Vars::yPosition,         "0.080", "Overlay Y position", true, true, 0,   true, 1.0f, true );
	cvarManager->registerCvar( Vars::scale,             "1",     "Overlay scale",      true, true, 0.5, true, 3,    true );

	cvarManager->setBind( Input::enabled, Vars::enabled );
	// cvarManager->setBind( Input::next, Vars::next );

	cvarManager->setBind( Input::refresh, Vars::refresh );
	cvarManager->setBind( Input::cycle, Vars::cycle );

	cvarManager->registerNotifier(
		Vars::enabled,
		[this]( std::vector<std::string> ) {
			setEnabled( !isEnabled() );
			cvarManager->getCvar(Vars::enabled).setValue( isEnabled() );
			refreshLobbyRanks();
		},
		"Toggle plugin",
		PERMISSION_ALL );

	cvarManager->registerNotifier(
		Vars::cycle,
		[this]( std::vector<std::string> ) {
			displayType = ++displayType % 3;
			refreshLobbyRanks();
		},
		"Toggle plugin",
		PERMISSION_ALL );

	cvarManager->registerNotifier(
		Vars::refresh,
		[this]( std::vector<std::string> ) {
			refreshLobbyRanks();
		},
		"refresh plugin",
		PERMISSION_ALL );

	//gameWrapper->HookEvent( "Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
	gameWrapper->HookEvent( "Function GameEvent_Soccar_TA.Countdown.BeginState",
	                        std::bind(&LobbyRanks::refreshLobbyRanks, this) );

	gameWrapper->RegisterDrawable( std::bind( &LobbyRanks::render, this, std::placeholders::_1 ) );

	// Apparently this wont run unless it happens a bit later
	gameWrapper->SetTimeout(
		[this]( GameWrapper* gw )
		{
			if( cvarManager->getCvar(Vars::enabled).getBoolValue() )
				setEnabled( true );
		},
		1 );

	gameWrapper->SetTimeout( [this](GameWrapper* g) { refreshLobbyRanks(); }, 5 );

}

void LobbyRanks::refreshLobbyRanks()
{
	ServerWrapper server = gameWrapper->GetOnlineGame();
	if( server.IsNull() ) return;

	MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();

	static auto getRankColor = []( int rank ) {
		if( rank < 0 || rank > 22 )
		{
			return RankTiers[Rank::Unranked].color;
		}
		else
		{
			Rank realRank = (Rank)(rank);
			return RankTiers[realRank].color;
		}
	};

	//playerz.clear();
	for( int i = 0; i < server.GetPRIs().Count(); i++ )
	{
		PriWrapper pri = server.GetPRIs().Get(i);
		if( pri.IsNull() ) continue;

		Player p;
		auto uid = pri.GetUniqueIdWrapper();
		p.name = pri.GetPlayerName().ToString();
		for( PPlaylist playlist : playlists )
		{
			SkillRank userRank = mmrWrapper.GetPlayerRank( uid, int(playlist) );
			p.data[playlist] = PlaylistData{
				/* Color */ getRankColor( userRank.Tier ),
				/* Rank  */ GetRankName( Rank(userRank.Tier), userRank.Division ),
				/* MMR   */ to_fstring( mmrWrapper.GetPlayerMMR(uid, int(playlist)), 1 ),
				/* Games */ std::to_string( userRank.MatchesPlayed )
			};
		}
		playerz[uid.GetUID()] = p;
	}
}

void LobbyRanks::onUnload() {}

std::string LobbyRanks::to_fstring( float val, int len )
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision( len ) << val;
	return oss.str();
}

std::string LobbyRanks::GetRankName( Rank rank, int div )
{
	if( rank < Rank::Unranked || rank > Rank::SupersonicLegend )
		return "ERROR";

	Rank realRank = (Rank)(rank);
	std::string rankName = RankTiers[realRank].abbvr;

	if( rank != Rank::Unranked && rank != Rank::SupersonicLegend )
		rankName += ".D" + std::to_string( div + 1 );

	return rankName;
}

Color GetRankColor( Rank rank )
{
	if( rank < Rank::Unranked || rank > Rank::SupersonicLegend )
		return RankTiers[Rank::Unranked].color;

	return RankTiers[rank].color;
}

Vector2 LobbyRanks::center( CanvasWrapper& canvas, const std::string& txt, const Vector2& cell, float scale )
{
	Vector2F size = canvas.GetStringSize( txt, scale, scale );
	Vector2 padded;
	padded.X = (cell.X - size.X) / 2.0f;
	padded.Y = (cell.Y - size.Y) / 2.0f;
	return padded;
}

void LobbyRanks::draw( CanvasWrapper& canvas )
{
	// 5 Columns: Player, 1s, 2s, 3s, Tournament
	static const auto headings = { "Name", "1v1", "2v2", "3v3", "T" };
	static const auto playlistz = { PPlaylist::Solo, PPlaylist::Twos, PPlaylist::Threes, PPlaylist::Tournament };

	float scale = 1.0;// cvarManager->getCvar( Vars::scale ).getFloatValue();
	float xpos = cvarManager->getCvar(Vars::xPosition).getFloatValue();
	float ypos = cvarManager->getCvar(Vars::yPosition).getFloatValue();
	int opacity = cvarManager->getCvar(Vars::backgroundOpacity).getIntValue();


	// ----- CALCULATE TABLE CELL SIZES -----
	std::vector<Vector2F> tableCellSizes;
	std::vector<std::string> tableCellValues;
	auto addCellText = [&]( const std::string& t ) {
		tableCellSizes.push_back( canvas.GetStringSize( t, scale, scale ) );
		tableCellValues.push_back( t );
	};

	auto getCurrentData = [&]( const PlaylistData& pd, int i ) {
		if( i == 0 )
			return pd.rank;
		else if( i == 1 )
			return pd.mmr;
		else
			return pd.games;
	};

	size_t index = 0;
	for( const auto& heading : headings )
		addCellText( heading );
	for( const auto& player : playerz )
	{
		const Player& p = player.second;
		for( int i = 0; i < 3; i++ )
		{
			if( i == 1 ) addCellText( p.name ); else addCellText( "" );
			for( const auto& playlist : playlistz )
			{
				PlaylistData pd;
				if( p.data.find( playlist ) != p.data.end() )
					pd = p.data.at( playlist );
				addCellText( getCurrentData( pd, i ) );
			}
		}
	}

	auto dothething = [&]( const std::string& t ) {
		tableCellSizes.push_back( canvas.GetStringSize( t, scale, scale ) );
		tableCellValues.push_back( t );
	};
	// dothething( "tankfish" );
	// dothething( "G1.D4" );
	// dothething( "P3.D4" );
	// dothething( "P3.D1" );
	// dothething( "P3.D2" );

	size_t rowCount = tableCellSizes.size() / 5;
	std::vector<float> columnWidths( 5, 0.0f );
	std::vector<float> rowHeigts( rowCount, 0.0f );
		auto SizeAt = [&]( int x, int y ) {
		Vector2 v;
		v.X = columnWidths[x];
		v.Y = rowHeigts[y];
		return v;
	};
	for( size_t x = 0; x < 5; x++ )
	{
		for( size_t y = 0; y < rowCount; y++ )
		{
			Vector2F cell = tableCellSizes.at( (y * 5) + x );
			columnWidths[x] = std::max( columnWidths[x], cell.X );
			rowHeigts[y] = std::max( rowHeigts[y], cell.Y );
		}
	}
	for( auto& c : columnWidths )
		c += 16.0;
	/*for( auto& r : rowHeigts )
		r += 4.0;*/

	// ----- DRAWING -----
	int pad = 40;
	const Vector2 Origin = {
		0,
		(canvas.GetSize().Y) - (std::accumulate( rowHeigts.begin(), rowHeigts.end(), 0 ) + pad )
	};// { canvas.GetSize().X / 2, canvas.GetSize().Y / 2 };
	auto getCellOrigin = [&]( size_t cx, size_t cy ) {
		Vector2 cpos = Origin; cpos.X += pad / 2; cpos.Y += pad / 2;
		std::vector<float>::iterator col = columnWidths.begin();
		std::vector<float>::iterator row = rowHeigts.begin();
		std::advance( col, cx ); std::advance( row, cy );
		// position to correct cell
		cpos.X += std::accumulate( columnWidths.begin(), col, .0f );
		cpos.Y += std::accumulate( rowHeigts.begin(), row, .0f );
		return cpos;
	};
	auto getCellTextOrigin = [&]( size_t cx, size_t cy ) {
		Vector2 cpos = getCellOrigin( cx, cy );
		// add padding (first column is left aligned
		Vector2F cell = tableCellSizes.at( (cy * 5) + cx );
		std::string val = tableCellValues.at( (cy * 5) + cx );
		Vector2 centerPad = center( canvas, val, SizeAt(cx, cy), scale );
		if( cx != 0 ) cpos.X += centerPad.X;
		else cpos.X += 8.0;
		cpos.Y += centerPad.Y;
		return cpos;
	};

	Vector2 pos = Origin;
	Vector2 sizeBox = {
		pad + std::accumulate( columnWidths.begin(), columnWidths.end(), .0f ) * scale,
		pad + std::accumulate( rowHeigts.begin(), rowHeigts.end(), .0f ) * scale,
	};



	// Table background
	/*canvas.SetPosition( pos );
	canvas.SetColor( 0, 0, 0, opacity );
	canvas.FillBox( sizeBox );*/
	int lightc = 25, darkc = 10;
	opacity = 220;
	LinearColor light; light.R = lightc; light.G = lightc; light.B = lightc; light.A = opacity;
	LinearColor dark; dark.R = darkc; dark.G = darkc; dark.B = darkc; dark.A = opacity;
	int tickTock = 2;
	for( size_t y = 0; y < rowCount; y++ )
	{
		for( size_t x = 0; x < 5; x++ )
		{
			Vector2 cellPos = getCellOrigin( x, y );
			canvas.SetPosition( cellPos );
			auto cell = tableCellSizes.at( (y * 5) + x );
			canvas.SetColor( (tickTock < 3 ? dark : light) );
			canvas.FillBox( SizeAt(x, y) );
		}
		tickTock = ++tickTock % 6;
	}

	canvas.SetColor( 255, 255, 255, 255 );
	// TABLE!
	for( size_t y = 0; y < rowCount; y++ )
	{
		int pn = y - 1;
		pn = pn / 3;
		for( size_t x = 0; x < 5; x++ )
		{
			if( y==0 || x==0 ) canvas.SetColor( 255, 255, 255, 255 ); else {
				auto it = playerz.begin();
				std::advance( it, pn );
				if( it != playerz.end() )
				{
					auto pit = playlistz.begin();
					std::advance( pit, (x - 1) % 4 );
					if( pit != playlistz.end() )
					{
						auto pd = it->second.data.find( *pit );
						if( pd != it->second.data.end() )
						{
							auto c = pd->second.color;
							canvas.SetColor( c.r, c.g, c.b, 255 );
						}
					}
				}
			}

			canvas.SetPosition( getCellTextOrigin( x, y ) );
			canvas.DrawString( tableCellValues.at( (y * 5) + x ), scale, scale );
		}
	}
}

void LobbyRanks::render( CanvasWrapper canvas )
{
	// Only render if the plugin is enabled
	//if( !isEnabled() ) return;

	//if( !gameWrapper->IsInOnlineGame() )
	//{
	//	writeStats( canvas, gameWrapper->GetUniqueID(), "You", -1 );
	//	return;
	//}
	if( isEnabled() )
		draw( canvas );
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
