#pragma once

#include "types.h"

#include <stdarg.h>
#include <sstream>

//namespace jlg {
//	std::string format( const char* format, ... )
//	{
//		std::string msg;
//		va_list args;
//		va_start( args, format );
//
//		char buffer[2048];
//		if( _vscprintf( format, args ) + 1 <= 2048 )
//		{
//			vsprintf_s( buffer, format, args );
//			msg = buffer;
//		}
//		va_end( args );
//
//		return msg;
//	}
//}


namespace jlg {

namespace util {

#pragma region toColor

template<class T>
LinearColor toColor( const T& v )
{
	return Color::White;
}

template<>
LinearColor util::toColor<Team>( const Team& v );
template<>
LinearColor util::toColor<SkillRank>( const SkillRank& v );
template<>
LinearColor util::toColor<Platform>( const Platform& v );

#pragma endregion

#pragma region toString

template<class T>
std::string toString( const T& v )
{
	return "";
}

template<>
std::string util::toString<double>( const double& v );
template<>
std::string util::toString<int>( const int& v );
template<>
std::string toString<SkillRank>( const SkillRank& v );
template<>
std::string toString<Playlist>( const Playlist& v );
template<>
std::string toString<Platform>( const Platform& v );

std::string toFullName( const Playlist& v );

#pragma endregion

}; }; // END namespace jlg::util
