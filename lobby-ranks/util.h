#pragma once

#include "types.h"

#include <stdarg.h>
#include <sstream>
#include <string>
#include <vector>

namespace jlg
{
	namespace str
	{
		std::vector<std::string> split( const std::string& str, char sep, bool omitEmpty = true );
		void addSubStr( std::vector<std::string>& list, const std::string& str, size_t first, size_t end, bool omitEmpty );
	}

	namespace util
	{
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
	};
}; // END namespace jlg
