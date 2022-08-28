#pragma once
#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1
#define VERSION_BUILD 0

#define VERSION_FULL stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD)

#define stringify(a) stringify_(a)
#define stringify_(a) #a

namespace jlg { namespace rl { namespace lobby_ranks {
}; }; }; // END namespace jlg::rl::lobby_ranks
