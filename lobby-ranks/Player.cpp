#include "Player.h"

#include "util.h"
#include "Config.h"

using namespace jlg;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Player::Player(  const std::shared_ptr<GameWrapper>& gw, PriWrapper wrapper )
	: isBot( false )
{
	name = wrapper.GetPlayerName().ToString();
	uuid = wrapper.GetUniqueIdWrapper().GetIdString();
	team = Team( wrapper.GetTeamNum() );
	isCurrentPlayer = wrapper.IsPlayerOwned();
	isBot = wrapper.GetbBot();
	platform = toPlatform( wrapper.GetPlatform() );

	MMRWrapper mw = gw->GetMMRWrapper();
	for( const Playlist& p : PlaylistValues )
	{
		stats[p] = PlaylistStats{
			p,
			mw.GetPlayerRank( wrapper.GetUniqueIdWrapper(), int(p) ),
			mw.GetPlayerMMR( wrapper.GetUniqueIdWrapper(), int(p) )
		};
	}
}

Player::Player( std::string name, Team team, Platform platform, bool isBot, bool isCurrentPlayer, Rank a, Rank b )
	: name( name )
	, team( team )
	, platform( platform )
	, isBot( isBot )
	, isCurrentPlayer( isCurrentPlayer )
{
	for( auto p : PlaylistValues )
	{
		stats[p] = PlaylistStats{
			p,
			SkillRank{ (int(p)%2==0 ? int(a) : int(b)), int(p)%3, 537 },
			1026
		};
	}
}

Player::~Player()
{}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
std::list<Table::Row> Player::row( LinearColor bg,
                                   const std::list<Playlist>& p,
                                   Vector2F pad  )
{
	Table::Row rankColumn;
	Table::Row mmrColumn;
	Table::Row gamesColumn;

	Config* config = Config::instance();

	auto nameColor = config->getColour( team );
	if( isCurrentPlayer )
		nameColor = Color::White;
	rankColumn.push_back( Table::Cell( "", nameColor, bg ) );
	rankColumn.back().setPadding( pad.Y, pad.X, 0.0f, pad.X );
	mmrColumn.push_back( Table::Cell( name, nameColor, bg, Table::Align::Left ) );
	mmrColumn.back().setPadding( pad.X, 0.0f );
	if( config->isPlatformDisplayed() )
	{
		gamesColumn.push_back( Table::Cell( util::toString(platform), config->getColour(platform), bg, Table::Align::Left ) );
		gamesColumn.back().setPadding( pad.X, 0.0 );
	}
	else
	{
		gamesColumn.push_back( Table::Cell( "", nameColor, bg ) );
		gamesColumn.back().setPadding( 0.0f, pad.X, pad.Y, pad.X );
	}

	for( const auto playlist : p )
	{
		auto ps = stats.at( playlist );
		auto color = config->getColour( ps.rank );
		rankColumn.push_back( Table::Cell( util::toString<SkillRank>( ps.rank ), color, bg, Table::Align::Center, Table::Align::Bottom ) );
		rankColumn.back().setPadding( pad.Y, pad.X, 0.0f, pad.X );
		mmrColumn.push_back( Table::Cell( util::toString<double>( ps.mmr ), color, bg, Table::Align::Center, Table::Align::Center ) );
		mmrColumn.back().setPadding( pad.X, 0.0f );
		gamesColumn.push_back( Table::Cell( util::toString<int>( ps.rank.MatchesPlayed ), color, bg, Table::Align::Center, Table::Align::Top ) );
		gamesColumn.back().setPadding( 0.0f, pad.X, pad.Y, pad.X );
	}

	return { rankColumn, mmrColumn, gamesColumn };
}
