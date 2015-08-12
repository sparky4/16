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

// We never use zero as a tag. This makes it easier to track down misbehaving
// VC code, since zero is commonly come-by when going off into uncharted
// depths of memory.

#define ERROR                 0

// Opcode values.

#define opEXEC_STDLIB         1
#define opEXEC_LOCALFUNC      2
#define opEXEC_EXTERNFUNC     3
#define opIF                  4
#define opELSE                5
#define opGOTO                6
#define opSWITCH              7
#define opCASE                8
#define opRETURN              9
#define opASSIGN              10
#define opSETRETVAL           11
#define opSETLOCALSTACK       12

// Operand types.

#define op_IMMEDIATE          1
#define op_HVAR0              2
#define op_HVAR1              3
#define op_UVAR               4
#define op_UVARRAY            5
#define op_LVAR               6
#define op_BFUNC              7
#define op_UFUNC              8
#define op_GROUP              9
#define op_STRING             10
#define op_SARRAY             11
#define op_SLOCAL             12

// Variable types.

#define v_IMMEDIATE           1  // is this necessary?
#define v_CHAR                2
#define v_PTR                 3
#define v_INT                 4

// IF relational operators.

#define i_ZERO                1
#define i_NONZERO             2
#define i_EQUALTO             3
#define i_NOTEQUAL            4
#define i_GREATERTHAN         5
#define i_LESSTHAN            6
#define i_GREATERTHANOREQUAL  7
#define i_LESSTHANOREQUAL     8
#define i_GROUP               9
#define i_UNGROUP             10
#define i_AND                 11
#define i_OR                  12

// Assignment operators.

#define a_SET                 1
#define a_INC                 2
#define a_DEC                 3
#define a_INCSET              4
#define a_DECSET              5
#define a_MULTSET             6
#define a_DIVSET              7

// Operand combination operators.

#define op_ADD                1
#define op_SUB                2
#define op_MULT               3
#define op_DIV                4
#define op_MOD                5
#define op_SHL                6
#define op_SHR                7
#define op_AND                8
#define op_OR                 9
#define op_XOR                10
#define op_END                11

// String components

#define s_IMMEDIATE           1
#define s_GLOBAL              2
#define s_ARRAY               3
#define s_NUMSTR              4
#define s_LEFT                5
#define s_RIGHT               6
#define s_MID                 7
#define s_LOCAL               8
#define s_ADD                 9
#define s_CHR                 10
#define s_END                 11
