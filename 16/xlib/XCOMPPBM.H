#ifndef _XCOMPPBM_H_
#define _XCOMPPBM_H_

#ifdef __cplusplus
extern "C" {
#endif


 int x_compile_pbm (           /* Compile a planar bitmap to generate  */
    WORD logical_screen_width, /* machine code to plot it at any */
    char far * bitmap,         /* required screen coordinates FAST. */
    char far * output);

 int x_sizeof_cpbm (           /* Find the size of the code which a  */
    WORD logical_screen_width, /* pbm would result in, if it  */
    char far * bitmap);        /* were compiled (used for allocation). */


#ifdef __cplusplus
}
#endif

#endif
