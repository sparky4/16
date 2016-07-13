/*********************************************************************\
| XBEZIER header file
|
|
| This module was written by Matthew MacKenzie
| matm@eng.umd.edu
\*********************************************************************/

#ifndef _XBEZIER_H_
#define _XBEZIER_H_
#ifdef __cplusplus

extern "C" {
#endif

void x_bezier (int E1x, int E1y, int C1x, int C1y, int C2x, int C2y,
			   int E2x, int E2y, int levels, char color,
			   unsigned PageOffset);

#ifdef __cplusplus
}
#endif

#endif

