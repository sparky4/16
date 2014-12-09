#ifndef _XMS_H_
#define _XMS_H_
#include <stddef.h> /* Definition of NULL */
#include <limits.h> /* Definition of UINT_MAX */
#include <stdio.h>  /* fprintf and (FILE *) */

/* Allow external configuration of maximum concurrent XMS allocations */
#ifndef MAX_XMS_ALLOCATIONS
#define MAX_XMS_ALLOCATIONS 4
#endif

/* Address of the XMS driver */
static long XMSControl;

/* Mapping of XMS handle <-> normal pointer */
typedef struct {
	unsigned int XMSHandle;
	void huge * XMSPointer;
} XMSHandleMap;

static XMSHandleMap allocMapXMS[MAX_XMS_ALLOCATIONS];

static int initxms(void);
void huge * xmsmalloc(long unsigned int size);
void xmsfree(void huge * XMSPointer);
void xmsreport(FILE * stream);

#endif/*_XMS_H_*/
