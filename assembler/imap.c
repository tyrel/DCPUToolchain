//
// DCPU Assembler by James Rhodes
//
// Instruction map.
//

#include <stdlib.h>
#include <string.h>
#include "imap.h"
#include "dcpu.h"

#define OPCODE(op) { #op, OP_ ## op, 0 }
#define NBOPCODE(op) { #op, OP_NONBASIC, NBOP_ ## op }
#define REGISTER(v) { #v, REG_ ## v }
#define REGISTER_VALUE(v) { #v, VAL_ ## v }
#define REGISTER_VALUE_NEXT(v) { #v, NXT_VAL_ ## v }

struct instruction_mapping instruction_name_map[] =
{
	OPCODE(SET),
	OPCODE(ADD),
	OPCODE(SUB),
	OPCODE(MUL),
	OPCODE(DIV),
	OPCODE(MOD),
	OPCODE(SHL),
	OPCODE(SHR),
	OPCODE(AND),
	OPCODE(BOR),
	OPCODE(XOR),
	OPCODE(IFE),
	OPCODE(IFN),
	OPCODE(IFG),
	OPCODE(IFB),
	NBOPCODE(JSR),
	NULL
};

struct register_mapping register_name_map[] =
{
	REGISTER(A),
	REGISTER(B),
	REGISTER(C),
	REGISTER(X),
	REGISTER(Y),
	REGISTER(Z),
	REGISTER(I),
	REGISTER(J),
	{ "POP", POP },
	{ "PEEK", PEEK },
	{ "PUSH", PUSH },
	{ "SP", SP },
	{ "PC", PC },
	{ "O", O },
	NULL
};

struct register_mapping register_value_name_map[] =
{
	REGISTER_VALUE(A),
	REGISTER_VALUE(B),
	REGISTER_VALUE(C),
	REGISTER_VALUE(X),
	REGISTER_VALUE(Y),
	REGISTER_VALUE(Z),
	REGISTER_VALUE(I),
	REGISTER_VALUE(J),
	NULL
};

struct register_mapping register_value_next_map[] =
{
	REGISTER_VALUE_NEXT(A),
	REGISTER_VALUE_NEXT(B),
	REGISTER_VALUE_NEXT(C),
	REGISTER_VALUE_NEXT(X),
	REGISTER_VALUE_NEXT(Y),
	REGISTER_VALUE_NEXT(Z),
	REGISTER_VALUE_NEXT(I),
	REGISTER_VALUE_NEXT(J),
	NULL
};

struct instruction_mapping* get_instruction_by_name(char* name)
{
	uint16_t i = 0;
	while (instruction_name_map[i].name != NULL)
	{
		if (strcmp(instruction_name_map[i].name, name) == 0)
			return &instruction_name_map[i];
		i += 1;
	}
	return NULL;
}

struct register_mapping* get_register_by_name(char* name, bool bracketed)
{
	uint16_t i = 0;
	if (bracketed)
	{
		while (register_value_name_map[i].name != NULL)
		{
			if (strcmp(register_value_name_map[i].name, name) == 0)
				return &register_value_name_map[i];
			i += 1;
		}
	}
	else
	{
		while (register_name_map[i].name != NULL)
		{
			if (strcmp(register_name_map[i].name, name) == 0)
				return &register_name_map[i];
			i += 1;
		}
	}
	return NULL;
}

struct register_mapping* get_register_by_name_next(char* name)
{
	uint16_t i = 0;
	while (register_value_next_map[i].name != NULL)
	{
		if (strcmp(register_value_next_map[i].name, name) == 0)
			return &register_value_next_map[i];
		i += 1;
	}
	return NULL;
}