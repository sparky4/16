/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#if !defined(__CODE_INC)
#define __CODE_INC

// Single-byte opcode descriptors

#define EXEC 1
#define VAR0_ASSIGN 2
#define VAR1_ASSIGN 3
#define VAR2_ASSIGN 4
#define GENERAL_IF 5
#define ELSE 6
#define GOTO 7
#define FOR_LOOP0 8
#define FOR_LOOP1 9
#define SWITCH 10
#define CASE 11
#define ENDSCRIPT 255

// Single-byte operand descriptors

#define OP_IMMEDIATE 1
#define OP_VAR0 2
#define OP_VAR1 3
#define OP_VAR2 4
#define OP_GROUP 5

// Single-byte IF handler parameters

#define ZERO 0
#define NONZERO 1
#define EQUALTO 2
#define NOTEQUAL 3
#define GREATERTHAN 4
#define GREATERTHANOREQUAL 5
#define LESSTHAN 6
#define LESSTHANOREQUAL 7

// Single byte assignment descriptors

#define SET 1
#define INCREMENT 2
#define DECREMENT 3
#define INCSET 4
#define DECSET 5

// Operand combination descriptors
#define ADD 1
#define SUB 2
#define MULT 3
#define DIV 4
#define MOD 5
#define OP_END 255

#endif // __CODE_INC
