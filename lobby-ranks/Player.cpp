#include "Player.h"

#include "util.h"

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

	auto nameColor = util::toColor<Team>( team );
	if( isCurrentPlayer )
		nameColor = Color::White;
	rankColumn.push_back( Table::Cell( "", nameColor, bg ) );
	rankColumn.back().setPadding( pad.Y, pad.X, 0.0f, pad.X );
	mmrColumn.push_back( Table::Cell( name, nameColor, bg, Table::Align::Left ) );
	mmrColumn.back().setPadding( pad.X, 0.0f );
	gamesColumn.push_back( Table::Cell( util::toString(platform), util::toColor(platform), bg, Table::Align::Left ) );
	gamesColumn.back().setPadding( pad.X, 0.0 );

	for( const auto playlist : p )
	{
		auto ps = stats.at( playlist );
		auto color = util::toColor<SkillRank>( ps.rank );
		rankColumn.push_back( Table::Cell( util::toString<SkillRank>( ps.rank ), color, bg, Table::Align::Center, Table::Align::Bottom ) );
		rankColumn.back().setPadding( pad.Y, pad.X, 0.0f, pad.X );
		mmrColumn.push_back( Table::Cell( util::toString<double>( ps.mmr ), color, bg, Table::Align::Center, Table::Align::Center ) );
		mmrColumn.back().setPadding( pad.X, 0.0f );
		gamesColumn.push_back( Table::Cell( util::toString<int>( ps.rank.MatchesPlayed ), color, bg, Table::Align::Center, Table::Align::Top ) );
		gamesColumn.back().setPadding( 0.0f, pad.X, pad.Y, pad.X );
	}

	return { rankColumn, mmrColumn, gamesColumn };
}
