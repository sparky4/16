
#ifdef DEBUGSERIAL
# include <hw/8250/8250.h>

void _DEBUG(const char *msg);
int _DEBUG_INIT();
#else
static inline void _DEBUG(const char *msg) {
	// NOTHING
}

static inline int _DEBUG_INIT() {
	// NOTHING
	return -1;
}
#endif

