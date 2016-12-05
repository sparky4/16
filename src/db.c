#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */

const char *byte_to_binary(int x)
{
	static char b[9];
	int z;

	b[0] = '\0';
	for (z = 128; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}
	return b;
}

void main()
{
	/* binary string to int */
	char *tmp;
	char *b = "1101";

	printf("%d\n", strtol(b, &tmp, 2));
	printf("%x\n", strtol(b, &tmp, 2));

	/* byte to binary string */
	printf("%s\n", byte_to_binary(4));
	printf("%s\n", byte_to_binary(16));
//    return 0;
}
