#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <list>

#include <json/value.h>
#include <json/writer.h>
#include "types.h"

#include "bakkesmod/wrappers/CVarManagerWrapper.h"

namespace jlg {

/**
 * JLG TODO Document Config
 */
class Config
{
	//--------------------STATIC VARIABLES----------------------
	private:
		static Json::FastWriter w;
	public:
		struct Value {
			using IntArray = std::vector<int32_t>;

			template<class T>
			Value( const std::shared_ptr<CVarManagerWrapper>& cvm, const std::string& name, const std::string& desc, T dflt, bool searchable = false )
				: cvm( cvm )
				, cname( std::string("jlg_lobby_rank_v_").append(name) )
				, name( name )
				, desc( desc )
				, dflt( to(dflt) )
				, val( to(dflt) )
			{
				cvm->registerCvar( cname, w.write(to(dflt)), "", searchable );
			};

			const std::string cname;
			const std::string name;
			const std::string desc;

			public:
				const Json::Value dflt;
			private:
				std::shared_ptr<CVarManagerWrapper> cvm;
				Json::Value val;

			public:
				void loadFromCfg();
				void saveToCfg();
				std::string asString();

				template<class T>
				T get();
				template<> bool        get<bool>();
				template<> int32_t     get<int32_t>();
				template<> float       get<float>();
				template<> LinearColor get<LinearColor>();
				template<> IntArray    get<IntArray>();

				template<class T>
				T getDefault();
				template<> bool        getDefault<bool>();
				template<> int32_t     getDefault<int32_t>();
				template<> float       getDefault<float>();
				template<> LinearColor getDefault<LinearColor>();
				template<> IntArray    getDefault<IntArray>();

				template<class T>
				Json::Value to( T v )
				{ return Json::Value(v); }
				template<> Json::Value to<LinearColor>( LinearColor value );
				template<> Json::Value to<IntArray>( IntArray value );

				template<class T>
				void set( T v )
				{
					val = to<T>(v);
					saveToCfg();
				}
		};

	//---------------------STATIC METHODS-----------------------
	public:
		static Config* instance();
		static void initialize( const std::shared_ptr<CVarManagerWrapper>& );

	//----------------------CONSTRUCTORS------------------------
	public:
		Config() = delete;
		Config( const std::shared_ptr<CVarManagerWrapper>& cvm );
		virtual ~Config();

	//--------------------INSTANCE METHODS----------------------
	public:
		void loadFromCfg();
		std::string debug();

		//------------------- Config Options -------------------
		bool isEnabled();
		void setEnabled( bool enabled );
		bool isVisible();
		bool isScoreboardOpen();
		bool isDisplayWithScoreboard();

		bool isExampleDisplayed() { return showExampleTable; };
		bool isRefreshExampleTable() { return refreshExampleTable; };
		void isRefreshExampleTable( bool newValue ) { refreshExampleTable= newValue; };

		bool isPlatformDisplayed();

		float getScale();
		Vector2F getTablePosition();
		Vector2F getTableAnchor();

		LinearColor getColour(const SkillRank&);
		LinearColor getColour(Platform);
		LinearColor getColour(Team);
		LinearColor tableBg();
		LinearColor tableFg();
		LinearColor tableOdd();
		LinearColor tableEven();

		std::list<Playlist> getPlaylists();

		//------------------------ Draw -------------------------
		void drawImGuiOptions();

		void drawTablePosOptions();
		void drawTeamOptions();
		void drawPlatformOptions();
		void drawRankOptions();
		void drawTableOptions();
		void drawColorOptions( const std::string& name );

		void drawPlaylistSelection();

	//-------------------INSTANCE VARIABLES---------------------
	private:
		std::map<std::string, Value> configMap;
		Json::Value json;
		bool showExampleTable;
		bool refreshExampleTable;
};

namespace gui {
	bool reset( const char* );
	bool drawBool( const char*, bool& );
	bool drawFloat( const char*, float&, float min = 0.0f, float max = 1.0f );
	bool drawColour( const char*, LinearColor&, LinearColor defaultV = LinearColor() );
}

} // END namespace jlg