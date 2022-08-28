#pragma once
#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1
#define VERSION_BUILD 33

#define VERSION_FULL stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD)

#define stringify(a) stringify_(a)
#define stringify_(a) #a

namespace jlg {
}; // END namespace jlg
