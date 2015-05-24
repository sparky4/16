#include "src/lib/lib_head.h"

/* local function */
void wait(clock_t wait);
void* AllocateLargestFreeBlock(size_t* Size);
size_t GetFreeSize(void);
long int filesize(FILE *fp);

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

void* AllocateLargestFreeBlock(size_t* Size)
{
  size_t s0, s1;
  void* p;

  s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);

  while (s0 && (p = malloc(s0)) == NULL)
    s0 >>= 1;

  if (p)
    free(p);

  s1 = s0 >> 1;

  while (s1)
  {
    if ((p = malloc(s0 + s1)) != NULL)
    {
      s0 += s1;
      free(p);
    }
    s1 >>= 1;
  }

  while (s0 && (p = malloc(s0)) == NULL)
    s0 ^= s0 & -s0;

  *Size = s0;
  return p;
}

size_t GetFreeSize(void)
{
  size_t total = 0;
  void* pFirst = NULL;
  void* pLast = NULL;

  for (;;)
  {
    size_t largest;
    void* p = AllocateLargestFreeBlock(&largest);

    if (largest < sizeof(void*))
    {
      if (p != NULL)
        free(p);
      break;
    }

    *(void**)p = NULL;

    total += largest;

    if (pFirst == NULL)
      pFirst = p;

    if (pLast != NULL)
      *(void**)pLast = p;

    pLast = p;
  }

  while (pFirst != NULL)
  {
    void* p = *(void**)pFirst;
    free(pFirst);
    pFirst = p;
  }

  return total;
}

long int
filesize(FILE *fp)
{
	long int save_pos, size_of_file;

	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	fseek(fp, save_pos, SEEK_SET);
	return(size_of_file);
}
