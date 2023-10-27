
typedef void _seg * memptr;

extern long TTLMainMem;
extern unsigned totalmem;		// total paragraphs available with 64k EMS
extern int	EMSpresent,XMSpresent;


//==========================================================================

//
// public prototypes
//

void MMStartup (void);
void MMShutdown (void);
void MMMapEMS (void);
void MMAllocate(memptr *baseptr,long size);
void MMGetPtr (memptr *baseptr,unsigned size);
void MMFreePtr (memptr *baseptr);
void MMSetPurge (memptr *baseptr, int purge);
void MMSortMem (void);
void MMBlockDump (void);
unsigned MMUnusedMemory (void);
unsigned MMTotalFree (void);


void PatchPointers (void);	// must be present in the main program