
#ifndef _SRC_LIB_16_DBG
#define _SRC_LIB_16_DBG

//#define __DEBUG__
//#define __DEBUG_PM__
//#define __DEBUG_MM__

#ifdef __DEBUG__
#ifdef __DEBUG_PM__
extern boolean dbg_debugpm=0;
#endif
#ifdef __DEBUG_InputMgr__
extern boolean dbg_testkeyin=0,dbg_testcontrolnoisy=0;
#endif
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

