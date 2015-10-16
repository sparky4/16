/*
	TWEAK2C version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Converts a TWEAK version 1.0 file to an #include-able C file
	defining the equivalent Register array, which is directly
	passable to the outRegArray() function defined in the TwkUser
	module.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

extern "C"
	{
	#include "TwkUser.h"
	}

main(int argc, char **argv)
	{
	if (argc < 4)
		{
		printf("TWEAK2C version 1.0\n"
			   "by Robert Schmidt of Ztiff Zox Softwear 1993\n"
			   "Converts a TWEAK version 1.x file to an #include-able C file.\n"
			   "\n"
                           "Syntax:  TWEAK2C <TWEAK-file> <C file to create> <array name>\n"
			   "All parameters are required.\n"
			   );

		return 0;
		}

	Register *table;
	int regs = loadRegArray(argv[1], &table);
	if (!table)
		return 1;			// loadRegArray provides error message

	ofstream out(argv[2], ios::out | ios::trunc);

	out << "#include \"TwkUser.h\" // get Register definition" << endl;
	out << "Register " << argv[3] << "[] =" << endl;
	out << "\t{" << hex << endl;

	Register *reg = table;
	while (regs--)
		{
		out << "\t{ 0x" << reg->port << ", 0x" << int(reg->index)
			<< ", 0x" << int(reg->value) << (regs?"},\n":"}\n");
		reg++;
		}
	out << "\t};" << endl;

	free(table);

	return 0;
	}
