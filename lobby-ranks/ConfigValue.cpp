#include "Config.h"

#include "types.h"
#include "util.h"

#include <json/reader.h>

using namespace jlg;

template<>
bool        Config::Value::get<bool>()
{
	return val.asBool();
}
template<>
int32_t     Config::Value::get<int32_t>()
{
	return val.asInt();
}
template<>
float       Config::Value::get<float>()
{
	return val.asFloat();
}
template<>
LinearColor Config::Value::get<LinearColor>()
{
	return LinearColor(
		val[0].asFloat(),
		val[1].asFloat(),
		val[2].asFloat(),
		val[3].asFloat()
	);
}
template<>
Config::Value::IntArray    Config::Value::get<Config::Value::IntArray>()
{
	IntArray a;
	for( Json::Value::ArrayIndex i = 0; i < val.size(); i++ )
		a.push_back( val[i].asInt() );
	return a;
}

template<>
bool        Config::Value::getDefault<bool>()
{
	return dflt.asBool();
}
template<>
int32_t     Config::Value::getDefault<int32_t>()
{
	return dflt.asInt();
}
template<>
float       Config::Value::getDefault<float>()
{
	return dflt.asFloat();
}
template<>
LinearColor Config::Value::getDefault<LinearColor>()
{
	return LinearColor(
		dflt[0].asFloat(),
		dflt[1].asFloat(),
		dflt[2].asFloat(),
		dflt[3].asFloat()
	);
}
template<>
Config::Value::IntArray    Config::Value::getDefault<Config::Value::IntArray>()
{
	IntArray a;
	for( Json::Value::ArrayIndex i = 0; i < dflt.size(); i++ )
		a.push_back( dflt[i].asInt() );
	return a;
}

template<>
Json::Value Config::Value::to<LinearColor>( LinearColor v )
{
	Json::Value a( Json::arrayValue );
	a.append( v.R );
	a.append( v.G );
	a.append( v.B );
	a.append( v.A );
	return a;
}
template<>
Json::Value Config::Value::to<Config::Value::IntArray>( Config::Value::IntArray v )
{
	Json::Value a( Json::arrayValue );
	for( const auto& i : v )
		a.append( i );
	return a;
}

void Config::Value::loadFromCfg()
{
	static Json::Reader reader;
	if( !cvm ) return;
	if( auto cvar = cvm->getCvar(cname) )
		reader.parse( cvar.getStringValue(), val, false );
}

void Config::Value::saveToCfg()
{
	if( !cvm ) return;
	if( auto cvar = cvm->getCvar(cname) )
		cvar.setValue( val.asString() );
}

std::string Config::Value::asString()
{
	return w.write( val );
}
