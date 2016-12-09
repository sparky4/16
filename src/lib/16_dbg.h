
#ifndef _SRC_LIB_16_DBG
#define _SRC_LIB_16_DBG

#include "src/lib/16_head.h"
#include "src/lib/16_tdef.h"

#define __DEBUG__
#define __DEBUG_InputMgr__
//#define __DEBUG_PM__
//#define __DEBUG_MM__


#ifdef __DEBUG__
#ifdef __DEBUG_PM__
extern boolean dbg_debugpm;
#endif
#ifdef __DEBUG_InputMgr__
extern boolean dbg_testkeyin,dbg_testcontrolnoisy;
#endif
extern boolean dbg_nommpmca;
#endif

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
#endif // _SRC_LIB_16_DBG

