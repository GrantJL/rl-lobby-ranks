#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "types.h"
#include "Table.h"

namespace jlg {

	struct PlaylistStats {
		Playlist playlist;
		SkillRank rank;
		double mmr;
	};

	/**
	 * JLG TODO Document Player
	 */
	class Player
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		public:
			std::string name;
			std::string uuid;
			Team team;
			Platform platform;
			bool isBot;
			bool isCurrentPlayer;

			std::map<Playlist, PlaylistStats> stats;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			Player() = delete;
			Player( const std::shared_ptr<GameWrapper>& gw, PriWrapper pw );
			virtual ~Player();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			std::list<Table::Row> row( LinearColor bg, const std::list<Playlist>& p, Vector2F pad = { 16.0, 4.0 } );
		// JLG TODO UniqueIDWrapper::IsPlayerOwned

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};

}; // END namespace jlg
