#ifndef __COMMON_H__
#define __COMMON_H__

#include "src/lib/vgmsnd/stdtype.h"
#include "src/lib/16_head.h"
//#include "src/lib/vgmsnd/stdbool.h"

#ifndef INLINE
#if defined(_MSC_VER)
#define INLINE	static __inline	// __forceinline
#elif defined(__GNUC__)
#define INLINE	static __inline__
#else
#define INLINE	static inline
#endif
#endif	// INLINE

#endif	// __COMMON_H__
