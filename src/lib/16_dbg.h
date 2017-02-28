
#ifndef _SRC_LIB_16_DBG
#define _SRC_LIB_16_DBG

#include "src/lib/16_head.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_tdef.h"

#define __DEBUG__
#define __DEBUG_InputMgr__
#define __DEBUG_MAP__
//#define __DEBUG_CA__
#define __DEBUG_PM__
//#define __DEBUG_MM__


#ifdef __DEBUG__
#ifdef __DEBUG_MM__
extern boolean dbg_debugmm;
#endif
#ifdef __DEBUG_PM__
extern boolean dbg_debugpm;
#endif
#ifdef __DEBUG_CA__
extern boolean dbg_debugca;
#endif
#ifdef __DEBUG_InputMgr__
extern boolean dbg_testkeyin,dbg_testcontrolnoisy,dbg_nointest;
#endif
#ifdef __DEBUG_MAP__
extern boolean dbg_maptext;
extern byte *dbg_mapdata;
#endif
#endif

#ifdef __WATCOMC__
# ifdef DEBUGSERIAL
#  include <hw/8250/8250.h>

void _DEBUGF(const char *fmt,...);
void _DEBUG(const char *msg);
int _DEBUG_INIT();
# else
static inline void _DEBUGF(const char *fmt,...) {
	// NOTHING
}
static inline void _DEBUG(const char *msg) {
	// NOTHING
}

static inline int _DEBUG_INIT() {
	// NOTHING
	return -1;
}
# endif
#endif //watcomc

void ShapeTest (global_game_variables_t *gvar);

#endif // _SRC_LIB_16_DBG
