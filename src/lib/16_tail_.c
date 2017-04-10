#include "src/lib/16_tail.h"

void	TL_VidInit(global_game_variables_t *gvar)
{
	start_timer(gvar);

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//	  So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//	  parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//	  the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	if (_DEBUG_INIT() == 0) {
#ifdef DEBUGSERIAL
		//printf("WARNING: Failed to initialize DEBUG output\n");
#endif
	}
	_DEBUG("Serial debug output started\n"); // NTS: All serial output must end messages with newline, or DOSBox-X will not emit text to log
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);

	textInit();

	// get old video mode
	//in.h.ah = 0xf;
	//int86(0x10, &in, &out);
	if(!gvar->video.old_mode) gvar->video.old_mode = vgaGetMode();//out.h.al;
	gvar->video.VL_Initiated = 1;
}
