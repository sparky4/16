typedef struct
{
  unsigned bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} huffnode;

extern long counts[256];
extern huffnode nodearray[256];


void CountBytes (unsigned char huge *start, long length);
void Huffmanize (void);
void OptimizeNodes (huffnode *table);
long HuffCompress (unsigned char huge *source, long length,
  unsigned char huge *dest);
void HuffExpand (unsigned char huge *source, unsigned char huge *dest,
  long length,huffnode *hufftable);
void RLEWExpand (unsigned huge *source, unsigned huge *dest,long length,
  unsigned rlewtag);
long RLEWCompress (unsigned huge *source, long length, unsigned huge *dest,
  unsigned rlewtag);
void RLEBExpand (unsigned char huge *source, unsigned char huge *dest,
  long length, unsigned char rlebtag);
long RLEBCompress (unsigned char huge *source, long length,
  unsigned char huge *dest, unsigned char rlebtag);
long CarmackCompress (unsigned far *source,long length,
	unsigned far *dest);
