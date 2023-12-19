#include "Config.h"

#include "types.h"
#include "util.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <json/reader.h>

using namespace jlg;

Json::Reader reader;
Json::FastWriter Config::w = [](){
	Json::FastWriter w;
	w.omitEndingLineFeed();
	return w;
}();

static Config* c = nullptr;
Config* Config::instance()
{
	return c;
}

void Config::initialize( const std::shared_ptr<CVarManagerWrapper>& cvm )
{
	if( c == nullptr )
		c = new Config( cvm );
}

namespace {
auto genDefaults = []( const std::shared_ptr<CVarManagerWrapper>& cvm ){
	using Value = Config::Value;
	std::map<std::string, Value> map;
	for( auto& v : { 
			Value( cvm, "enabled",                    "", true, true ),
			Value( cvm, "scoreboard_display_with",    "", true, true ),
			Value( cvm, "playlists",                  "", std::vector<int32_t>{10,11,13,34} ),
			Value( cvm, "display_platform",           "", true ),
			Value( cvm, "scale",                      "", 1.0 ),
			Value( cvm, "table_position_x",           "", 0.7 ),
			Value( cvm, "table_position_y",           "", 0.5 ),
			Value( cvm, "table_anchor_x",             "", 0 ),
			Value( cvm, "table_anchor_y",             "", 1 ),
			Value( cvm, "color_table_hdr_foreground", "", LinearColor{255, 255, 255, 255} ),
			Value( cvm, "color_table_hdr_background", "", LinearColor{  0,   0,   0, 255} ),
			Value( cvm, "color_table_row_even",       "", LinearColor( 40,  40,  40, 255) ),
			Value( cvm, "color_table_row_odd",        "", LinearColor( 20,  20,  20, 255) ),
			Value( cvm, "color_team_blue",            "", Color::Team::Blue ),
			Value( cvm, "color_team_orange",          "", Color::Team::Orange ),
			Value( cvm, "color_team_none",            "", Color::Team::White ),
			Value( cvm, "color_platform_steam",       "", Color::Platform::Steam ),
			Value( cvm, "color_platform_playstation", "", Color::Platform::Playstation ),
			Value( cvm, "color_platform_xbox",        "", Color::Platform::Xbox ),
			Value( cvm, "color_platform_nintendo",    "", Color::Platform::Nintendo ),
			Value( cvm, "color_platform_epic",        "", Color::Platform::Epic ),
			Value( cvm, "color_rank_unranked",        "", Color::Rank::Unranked ),
			Value( cvm, "color_rank_bronze",          "", Color::Rank::Bronze ),
			Value( cvm, "color_rank_silver",          "", Color::Rank::Silver ),
			Value( cvm, "color_rank_gold",            "", Color::Rank::Gold ),
			Value( cvm, "color_rank_platinum",        "", Color::Rank::Platinum ),
			Value( cvm, "color_rank_diamond",         "", Color::Rank::Diamond ),
			Value( cvm, "color_rank_champ",           "", Color::Rank::Champ ),
			Value( cvm, "color_rank_grandchamp",      "", Color::Rank::GrandChamp ),
			Value( cvm, "color_rank_ssl",             "", Color::Rank::SupersonicLegend )
		 }) map.insert( {v.name, v} );


	return map;
};
}

#pragma region Gui

bool gui::reset( const char* name )
{
	auto label = (strlen(name) ? std::string("Reset ").append(name) : "Reset" );
	bool reset = ImGui::Button( "R" );
	if( ImGui::IsItemHovered() )
		ImGui::SetTooltip( label.c_str() );
	return reset;
}

bool gui::drawBool( const char* name, bool& v )
{
	return ImGui::Checkbox( name, &v );
}
bool gui::drawFloat( const char* name, float& v, float min, float max )
{
	return ImGui::SliderFloat( name, &v, min, max );
}
bool gui::drawColour( const char* name, LinearColor& v, LinearColor defaultV )
{
	bool changed = false;
	bool reset = false;

	ImGui::PushID(name);
	auto varname = str::split(name, '.').back();
	reset = gui::reset( varname.c_str() );

	ImVec4 c(v.R/255.f, v.G/255.f, v.B/255.f, v.A/255.f );
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha;

	ImGui::SameLine();
	changed = ImGui::ColorEdit3( "##color", (float*)&c );
	ImGui::SameLine(); ImGui::Text( varname.c_str() );
	ImGui::PopID();

	if( changed ) v = LinearColor(c.x*255.f, c.y*255.f, c.z*255.f, c.w*255.f);
	if( reset ) v = defaultV;

	return changed || reset;
}

#pragma endregion

#pragma region Get Config

bool Config::isEnabled()
{
	return configMap.at("enabled").get<bool>();
}
void Config::setEnabled( bool enabled )
{
	configMap.at("enabled").set( enabled );
}
bool Config::isVisible()
{
	return configMap.at("visible").get<bool>();
}
bool Config::isScoreboardOpen()
{
	return configMap.at("scoreboard_open").get<bool>();
}
bool Config::isDisplayWithScoreboard()
{
	return configMap.at("scoreboard_display_with").get<bool>();
}
bool Config::isPlatformDisplayed()
{
	return configMap.at("display_platform").get<bool>();
}

float Config::getScale()
{
	return configMap.at("scale").get<float>();
}
Vector2F Config::getTablePosition()
{
	return {
		configMap.at("table_position_x").get<float>(),
		configMap.at("table_position_y").get<float>()
	};
}
Vector2F Config::getTableAnchor()
{
	return {
		(configMap.at("table_anchor_x").get<int32_t>() / 2.f),
		(configMap.at("table_anchor_y").get<int32_t>() / 2.f)
	};
}

LinearColor Config::getColour(const SkillRank& v)
{
	switch( (Rank)v.Tier )
	{
		default:
		case Rank::Unranked:
			return configMap.at("color_rank_unranked").get<LinearColor>();
		case Rank::Bronze1:
		case Rank::Bronze2:
		case Rank::Bronze3:
			return configMap.at("color_rank_bronze").get<LinearColor>();
		case Rank::Silver1:
		case Rank::Silver2:
		case Rank::Silver3:
			return configMap.at("color_rank_silver").get<LinearColor>();
		case Rank::Gold1:
		case Rank::Gold2:
		case Rank::Gold3:
			return configMap.at("color_rank_gold").get<LinearColor>();
		case Rank::Platinum1:
		case Rank::Platinum2:
		case Rank::Platinum3:
			return configMap.at("color_rank_platinum").get<LinearColor>();
		case Rank::Diamond1:
		case Rank::Diamond2:
		case Rank::Diamond3:
			return configMap.at("color_rank_diamond").get<LinearColor>();
		case Rank::Champ1:
		case Rank::Champ2:
		case Rank::Champ3:
			return configMap.at("color_rank_champ").get<LinearColor>();
		case Rank::GrandChamp1:
		case Rank::GrandChamp2:
		case Rank::GrandChamp3:
			return configMap.at("color_rank_grandchamp").get<LinearColor>();
		case Rank::SupersonicLegend:
			return configMap.at("color_rank_ssl").get<LinearColor>();
	}
}
LinearColor Config::getColour(Platform v)
{
	switch( v )
	{
		default:
		case Platform::Unknown:     return LinearColor();
		case Platform::Steam:       return configMap.at("color_platform_steam").get<LinearColor>();
		case Platform::Playstation: return configMap.at("color_platform_playstation").get<LinearColor>();
		case Platform::Xbox:        return configMap.at("color_platform_xbox").get<LinearColor>();
		case Platform::Nintendo:    return configMap.at("color_platform_nintendo").get<LinearColor>();
		case Platform::Epic:        return configMap.at("color_platform_epic").get<LinearColor>();
	}
}
LinearColor Config::getColour(Team v)
{
	switch( v )
	{
		case Team::Blue:   return configMap.at("color_team_blue").get<LinearColor>();
		case Team::Orange: return configMap.at("color_team_orange").get<LinearColor>();
		default:           return configMap.at("color_team_none").get<LinearColor>();
	}
}

LinearColor Config::tableBg()
{
	return configMap.at("color_table_hdr_background").get<LinearColor>();
}
LinearColor Config::tableFg()
{
	return configMap.at("color_table_hdr_foreground").get<LinearColor>();
}
LinearColor Config::tableOdd()
{
	return configMap.at("color_table_row_odd").get<LinearColor>();
}
LinearColor Config::tableEven()
{
	return configMap.at("color_table_row_even").get<LinearColor>();
}

std::list<Playlist> Config::getPlaylists()
{
	std::list<jlg::Playlist> p;
	for( auto i : configMap.at("playlists").get<Value::IntArray>() )
		p.push_back( Playlist(i) );
	return p;
}

#pragma endregion

// ----------------------------------------------------------
//                       CONSTRUCTORS
// ----------------------------------------------------------
Config::Config( const std::shared_ptr<CVarManagerWrapper>& cvm )
	: configMap( genDefaults(cvm) )
	, showExampleTable( false )
	, refreshExampleTable( true )
{}

Config::~Config()
{}

// ----------------------------------------------------------
//                     INSTANCE METHODS
// ----------------------------------------------------------
void Config::drawImGuiOptions()
{
	bool enabled = isEnabled();
	if( gui::drawBool("Enabled", enabled) )
		configMap.at("enabled").set( enabled );

	bool dispWithScoreboard = isDisplayWithScoreboard();
	if( gui::drawBool("Display with scoreboard", dispWithScoreboard) )
		configMap.at("scoreboard_display_with").set( dispWithScoreboard );

	bool displayPlatform = isPlatformDisplayed();
	if( gui::drawBool("Display user platform", displayPlatform) )
		configMap.at("display_platform").set( displayPlatform );

	if( ImGui::Button((showExampleTable?"Hide Example Table":"Show Example Table")) )
		showExampleTable = !showExampleTable;

	ImGui::SameLine();
	if( ImGui::Button("Refresh Example Table") )
		refreshExampleTable = true;

	//if( showExampleTable ) return; // avoid sychconous crash

	auto colapseFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
	if( ImGui::CollapsingHeader("Table Position", colapseFlags) )
		drawTablePosOptions();

	if( ImGui::CollapsingHeader("Colors", colapseFlags) )
	{
		ImGui::Indent();
			if( ImGui::CollapsingHeader("Lobby Table", colapseFlags) ) drawTableOptions();
			if( ImGui::CollapsingHeader("Player Team", colapseFlags) ) drawTeamOptions();
			if( ImGui::CollapsingHeader("Player Platform", colapseFlags) ) drawPlatformOptions();
			if( ImGui::CollapsingHeader("Playlist Rank", colapseFlags) ) drawRankOptions();
		ImGui::Unindent();
	}

	drawPlaylistSelection();
}

void Config::drawTablePosOptions()
{
	auto anchorX = configMap.at( "table_anchor_x" ).get<int32_t>();
	auto anchorY = configMap.at( "table_anchor_y" ).get<int32_t>();

	if( ImGui::Combo("Table Anchor X", &anchorX, "Left\0Middle\0Right\0\0") )
		configMap.at( "table_anchor_x" ).set( anchorX );
	if( ImGui::Combo("Table Anchor Y", &anchorY, "Top\0Middle\0Bottom\0\0") )
		configMap.at( "table_anchor_y" ).set( anchorY );

	auto [posX, posY] = getTablePosition();
	if( gui::drawFloat("Table Position X", posX, 0.0, 1.0) )
		configMap.at( "table_position_x" ).set( posX );
	if( gui::drawFloat("Table Position Y", posY, 0.0, 1.0) )
		configMap.at( "table_position_y" ).set( posY );

}
void Config::drawTeamOptions()
{
	ImGui::Indent();
	for( const auto& cname : {"color_team_blue", "color_team_orange", "color_team_none"} )
		drawColorOptions( cname );
	ImGui::Unindent();
}
void Config::drawPlatformOptions()
{
	ImGui::Indent();
	for( const auto& cname : {"color_platform_steam","color_platform_playstation","color_platform_xbox","color_platform_nintendo","color_platform_epic"} )
		drawColorOptions( cname );
	ImGui::Unindent();
}
void Config::drawRankOptions()
{
	ImGui::Indent();
	for( const auto& cname : {"color_rank_unranked","color_rank_bronze","color_rank_silver","color_rank_gold","color_rank_platinum","color_rank_diamond","color_rank_champ","color_rank_grandchamp","color_rank_ssl"} )
		drawColorOptions( cname );
	ImGui::Unindent();
}
void Config::drawTableOptions()
{
	ImGui::Indent();
	for( const auto& cname : {"color_table_hdr_foreground", "color_table_hdr_background", "color_table_row_even", "color_table_row_odd"} )
		drawColorOptions( cname );
	ImGui::Unindent();
}

void Config::drawColorOptions( const std::string& cname )
{
	ImGui::PushItemWidth(480);

	auto& config = configMap.at( cname );

	auto val = config.get<LinearColor>();
	if( gui::drawColour(cname.c_str(), val, config.getDefault<LinearColor>()) )
		config.set<LinearColor>( val );

	ImGui::PopItemWidth();
}

void Config::drawPlaylistSelection()
{
	auto colapseFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
	if( !ImGui::CollapsingHeader("Playlists", colapseFlags) )
		return;

	struct PlaylistItem {
		PlaylistItem() : PlaylistItem( Playlist::Unranked, 0 ) {};
		PlaylistItem( Playlist p, uint32_t index, bool enabled = true )
			: playlist(p)
			, name( util::toFullName(p) )
			, enabled( enabled )
			, index( index )
		{}
		Playlist playlist;
		std::string name;
		bool enabled = false;
		uint32_t index;
	};

	// JLG NOTE This code here is an abomination, but it was easier to get this hack in
	//          then it was to try get this done properly (or the time it would take
	//          to find that solution)

	auto& configValue = configMap.at( "playlists" );
	auto config = configValue.get<Value::IntArray>();

	ImGui::PushID("PlaylistSelection");

	if( ImGui::Button("Reset Playlists") )
	{
		configValue.set( configValue.getDefault<Value::IntArray>() );
		ImGui::PopID();
		return;
	}


	std::vector<PlaylistItem> playlistItems = [config](){
		uint32_t idx = 0;
		std::vector<PlaylistItem> v;
		for( const auto& c : config )
			v.push_back(PlaylistItem(Playlist(c), idx++));

		for( auto& p : PlaylistValues )
		{
			if( std::find(config.begin(), config.end(), int32_t(p)) == config.end() )
				v.push_back(PlaylistItem(p, idx++, false));
		}
		return v;
	}();

	static auto comp = []( const PlaylistItem& a, const PlaylistItem& b )
	{
		if( a.enabled == b.enabled )
			return a.index < b.index;

		return a.enabled;
	};
	
	ImGui::Indent();
	for( size_t i = 0; i < playlistItems.size(); i++ )
	{
		auto& p = playlistItems.at(i);
		ImGui::PushID( p.name.c_str() );
		ImVec2 sz = ImVec2( ImGui::GetFrameHeight(), ImGui::GetFrameHeight() );
		ImGuiButtonFlags dnflags = (i==0 || !p.enabled ? ImGuiButtonFlags_Disabled : ImGuiButtonFlags_None );
		ImGuiButtonFlags dnbutton = (i==0 || !p.enabled ? ImGuiDir_None : ImGuiDir_Up );
		ImGui::Bullet();
		ImGui::SameLine();
		ImGui::Checkbox( "##Checkbox", &p.enabled );
		if( p.enabled )
		{
			ImGui::SameLine();
			if( ImGui::ArrowButton("##up", ImGuiDir_Down) )
			{
				if( i==config.size()-1 )
				{
					p.enabled = false;
				}
				else
				{
					p.index++;
					playlistItems.at(i+1).index--;
				}
			}
			ImGui::SameLine();
			if( ImGui::ArrowButtonEx("##dn", dnbutton, sz, dnflags) )
			{
				p.index--;
				playlistItems.at(i-1).index++;
			}
			ImGui::SameLine();
			ImGui::Text( p.name.c_str() );
		}
		else
		{
			ImGui::SameLine();
			ImGui::TextDisabled( p.name.c_str() );
		}


		ImGui::PopID();
	}

	ImGui::Unindent();
	ImGui::PopID();


	std::stable_sort(playlistItems.begin(), playlistItems.end(), comp);

	Value::IntArray newPlaylists;
	for( const auto& p : playlistItems )
		if( p.enabled )
			newPlaylists.push_back(int32_t(p.playlist));
	configValue.set( newPlaylists );
}

void Config::loadFromCfg()
{
	for( auto& [k, v] : configMap )
		v.loadFromCfg();
}

std::string Config::debug()
{
	std::stringstream ss;
	ss << "Debug config:\n";
	for( auto [name, value] : configMap )
		ss << '\t' << name << ": " << value.asString() << "\n";
	return ss.str();
}
