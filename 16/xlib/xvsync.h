#ifndef _XVSYNC_H_
#define _XVSYNC_H_


#ifdef __cplusplus
extern "C" {
#endif

	void x_install_vsync_handler(int skipcount);
	void x_remove_vsync_handler(void);
	void x_set_user_vsync_handler(void far (*)());

#ifdef __cplusplus
}
#endif


extern WORD             TicksPerSecond;
extern volatile long    VsyncIntTicks;
extern WORD             VsyncPeriod;

#endif
