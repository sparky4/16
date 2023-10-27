void InitXMS(void);
int XMSAllocate(long size);
unsigned XMSTotalFree(void);	// returns KB free
void XMSFreeMem(int handle);
void XMSmove(int srchandle,long srcoff,int desthandle,long destoff,long size);

extern unsigned XMSavail;
extern long TTLxms;