#include "src\lib\lib_com.h"

/* local function */
void wait(clock_t wait);

/* Function: Wait **********************************************************
*
*     Parameters:    wait - time in microseconds
*
*     Description:    pauses for a specified number of microseconds.
*
*/
void wait(clock_t wait){
	clock_t goal;

	if(!wait) return;

	goal = wait + clock();
	while((goal > clock()) && !kbhit()) ;
} /* End of wait */