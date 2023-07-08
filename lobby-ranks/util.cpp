#include "util.h"

#include <iomanip>
#include <sstream>

using namespace jlg;

template<>
LinearColor util::toColor<Team>( const Team& v )
{
	switch( v )
	{
		default:           return Color::Team::White;
		case Team::Blue:   return Color::Team::Blue;
		case Team::Orange: return Color::Team::Orange;
	}
}

template<>
LinearColor util::toColor<SkillRank>( const SkillRank& v )
{
	switch( (Rank)v.Tier )
	{
		default:
			return Color::White;
		case Rank::Unranked:
			return Color::Rank::Unranked;
		case Rank::Bronze1:
		case Rank::Bronze2:
		case Rank::Bronze3:
			return Color::Rank::Bronze;
		case Rank::Silver1:
		case Rank::Silver2:
		case Rank::Silver3:
			return Color::Rank::Silver;
		case Rank::Gold1:
		case Rank::Gold2:
		case Rank::Gold3:
			return Color::Rank::Gold;
		case Rank::Platinum1:
		case Rank::Platinum2:
		case Rank::Platinum3:
			return Color::Rank::Platinum;
		case Rank::Diamond1:
		case Rank::Diamond2:
		case Rank::Diamond3:
			return Color::Rank::Diamond;
		case Rank::Champ1:
		case Rank::Champ2:
		case Rank::Champ3:
			return Color::Rank::Champ;
		case Rank::GrandChamp1:
		case Rank::GrandChamp2:
		case Rank::GrandChamp3:
			return Color::Rank::GrandChamp;
		case Rank::SupersonicLegend:
			return Color::Rank::SupersonicLegend;
	}
}

template<>
LinearColor util::toColor<Platform>( const Platform& v )
{
	switch( v )
	{
		default:
		case Platform::Unknown:     return LinearColor();
		case Platform::Steam:       return LinearColor{ 142, 150, 179, 255};
		case Platform::Playstation: return LinearColor{  30,  66, 147, 255};
		case Platform::Xbox:        return LinearColor{  61, 117,  35, 255};
		case Platform::Nintendo:    return LinearColor{ 205,  53,  32, 255};
		case Platform::Epic:        return LinearColor{ 164, 164, 164, 255};
	}
}

template<>
std::string util::toString<double>( const double& v )
{
	std::stringstream ss;
	ss << std::setprecision( 1 ) << std::fixed << v;
	return ss.str();
}

template<>
std::string util::toString<int>( const int& v )
{
	std::stringstream ss;
	ss << v;
	return ss.str();
}

template<>
std::string util::toString<SkillRank>( const SkillRank& v )
{
	std::stringstream ss;
	Rank rank = (Rank)v.Tier;
	switch( (Rank)v.Tier )
	{
		default:                     ss << String::Rank::Default; break;
		case Rank::Unranked:         ss << String::Rank::Unranked; break;
		case Rank::Bronze1:          ss << String::Rank::Bronze1; break;
		case Rank::Bronze2:          ss << String::Rank::Bronze2; break;
		case Rank::Bronze3:          ss << String::Rank::Bronze3; break;
		case Rank::Silver1:          ss << String::Rank::Silver1; break;
		case Rank::Silver2:          ss << String::Rank::Silver2; break;
		case Rank::Silver3:          ss << String::Rank::Silver3; break;
		case Rank::Gold1:            ss << String::Rank::Gold1; break;
		case Rank::Gold2:            ss << String::Rank::Gold2; break;
		case Rank::Gold3:            ss << String::Rank::Gold3; break;
		case Rank::Platinum1:        ss << String::Rank::Platinum1; break;
		case Rank::Platinum2:        ss << String::Rank::Platinum2; break;
		case Rank::Platinum3:        ss << String::Rank::Platinum3; break;
		case Rank::Diamond1:         ss << String::Rank::Diamond1; break;
		case Rank::Diamond2:         ss << String::Rank::Diamond2; break;
		case Rank::Diamond3:         ss << String::Rank::Diamond3; break;
		case Rank::Champ1:           ss << String::Rank::Champ1; break;
		case Rank::Champ2:           ss << String::Rank::Champ2; break;
		case Rank::Champ3:           ss << String::Rank::Champ3; break;
		case Rank::GrandChamp1:      ss << String::Rank::GrandChamp1; break;
		case Rank::GrandChamp2:      ss << String::Rank::GrandChamp2; break;
		case Rank::GrandChamp3:      ss << String::Rank::GrandChamp3; break;
		case Rank::SupersonicLegend: ss << String::Rank::SupersonicLegend; break;
	}
	if( rank > Rank::Unranked && rank < Rank::SupersonicLegend )
		ss << ".D" << v.Division+1;

	return ss.str();
}

template<>
std::string util::toString<Playlist>( const Playlist& playlist )
{
	switch( playlist )
	{
		default:                   return String::Playlist::Default;
		case Playlist::Unranked:   return String::Playlist::Unranked;
		case Playlist::Solo:       return String::Playlist::Solo;
		case Playlist::Twos:       return String::Playlist::Twos;
		case Playlist::Threes:     return String::Playlist::Threes;
		case Playlist::Hoops:      return String::Playlist::Hoops;
		case Playlist::Rumble:     return String::Playlist::Rumble;
		case Playlist::Dropshot:   return String::Playlist::Dropshot;
		case Playlist::Snowday:    return String::Playlist::Snowday;
		case Playlist::Tournament: return String::Playlist::Tournament;
	}
}
template<>
std::string util::toString<Platform>( const Platform& v )
{
	switch( v )
	{
		default:
		case Platform::Unknown:     return "";
		case Platform::Steam:       return "Steam";
		case Platform::Playstation: return "PlayStation";
		case Platform::Xbox:        return "Xbox";
		case Platform::Nintendo:    return "Nintendo";
		case Platform::Epic:        return "Epic";
	}
}

std::string util::toFullName( const Playlist& v )
{
	switch( v )
	{
		default:                   return "";
		case Playlist::Unranked:   return "Unranked";
		case Playlist::Solo:       return "Solo";
		case Playlist::Twos:       return "Twos";
		case Playlist::Threes:     return "Threes";
		case Playlist::Hoops:      return "Hoops";
		case Playlist::Rumble:     return "Rumble";
		case Playlist::Dropshot:   return "Dropshot";
		case Playlist::Snowday:    return "Snowday";
		case Playlist::Tournament: return "Tournament";
	}
}
