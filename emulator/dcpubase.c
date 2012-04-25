/**

	File:			dcpubase.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes
					Aaron Miller

	Description:	Handles core functionality of
					the virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdarg.h>
#include <stdio.h>
#include "dcpubase.h"
#include "dcpuops.h"
#include "dcpuhook.h"

void vm_halt(vm_t* vm, const char* message, ...)
{
    va_list argptr;
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
	vm->halted = true;
	return;
}

void vm_interrupt(vm_t* vm, uint16_t msgid)
{
	if (vm->ia == 0)
		return;
	vm->ram[vm->sp++] = vm->pc;
	vm->ram[vm->sp++] = vm->registers[REG_A];
	vm->pc = vm->ia;
	vm->registers[REG_A] = msgid;
}

uint16_t vm_consume_word(vm_t* vm)
{
	uint16_t v = vm->ram[vm->pc];
	vm->pc++;
	return v;
}

uint16_t vm_resolve_value(vm_t* vm, uint16_t val, uint8_t pos)
{
	uint16_t t;
	switch (val)
	{
	case REG_A:
	case REG_B:
	case REG_C:
	case REG_X:
	case REG_Y:
	case REG_Z:
	case REG_I:
	case REG_J:
		return vm->registers[val];
	case VAL_A:
	case VAL_B:
	case VAL_C:
	case VAL_X:
	case VAL_Y:
	case VAL_Z:
	case VAL_I:
	case VAL_J:
		return vm->ram[vm->registers[val - VAL_A]];
	case NXT_VAL_A:
	case NXT_VAL_B:
	case NXT_VAL_C:
	case NXT_VAL_X:
	case NXT_VAL_Y:
	case NXT_VAL_Z:
	case NXT_VAL_I:
	case NXT_VAL_J:
		return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm_consume_word(vm))];
	case PUSH_POP:
		if (vm->skip) return vm->dummy;
		if (pos == POS_A)
			t = vm->sp++;
		else if (pos == POS_B)
			t = --vm->sp;
		else
			// Probably should handle if pos is neither
			// A nor B to catch internal errors.
			t = vm->sp;
		return vm->ram[t];
	case PEEK:
		t = vm->sp;
		return vm->ram[t];
	case PICK:
		t = vm->sp;
		return vm->ram[(uint16_t)(t + vm_consume_word(vm))];
	case IA:
		return vm->ia;
	case SP:
		return vm->sp;
	case PC:
		return vm->pc;
	case EX:
		return vm->ex;
	case NXT:
		return vm->ram[vm_consume_word(vm)];
	case NXT_LIT:
		return vm_consume_word(vm);
	default:
		return val - 0x20;
	}
}

void vm_print_op(const char* opname, vm_t* vm, uint16_t a, uint16_t b)
{
	if (!vm->debug)
		return;
	if (vm->skip)
		printf("(skipped) %s 0x%04X 0x%04X", opname, a, b);
	else
		printf("%s 0x%04X 0x%04X", opname, a, b);
}

void vm_print_op_nonbasic(const char* opname, vm_t* vm, uint16_t a)
{
	if (!vm->debug)
		return;
	if (vm->skip)
		printf("(skipped) %s 0x%04X", opname, a);
	else
		printf("%s 0x%04X", opname, a);
}

void vm_cycle(vm_t* vm)
{
	uint16_t instruction = vm_consume_word(vm);
	uint16_t op = INSTRUCTION_GET_OP(instruction);
	uint16_t b = INSTRUCTION_GET_B(instruction);
	uint16_t a = INSTRUCTION_GET_A(instruction);

	switch (op)
	{
	case OP_SET:
		vm_print_op("SET", vm, b, a);
		vm_op_set(vm, b, a);
		break;
	case OP_ADD:
		vm_print_op("ADD", vm, b, a);
		vm_op_add(vm, b, a);
		break;
	case OP_SUB:
		vm_print_op("SUB", vm, b, a);
		vm_op_sub(vm, b, a);
		break;
	case OP_MUL:
		vm_print_op("MUL", vm, b, a);
		vm_op_mul(vm, b, a);
		break;
	case OP_DIV:
		vm_print_op("DIV", vm, a, b);
		vm_op_div(vm, b, a);
		break;
	case OP_MOD:
		vm_print_op("MOD", vm, b, a);
		vm_op_mod(vm, b, a);
		break;
	case OP_SHL:
		vm_print_op("SHL", vm, b, a);
		vm_op_shl(vm, b, a);
		break;
	case OP_SHR:
		vm_print_op("SHR", vm, b, a);
		vm_op_shr(vm, b, a);
		break;
	case OP_AND:
		vm_print_op("AND", vm, b, a);
		vm_op_and(vm, b, a);
		break;
	case OP_BOR:
		vm_print_op("BOR", vm, b, a);
		vm_op_bor(vm, b, a);
		break;
	case OP_XOR:
		vm_print_op("XOR", vm, b, a);
		vm_op_xor(vm, b, a);
		break;
	case OP_IFE:
		vm_print_op("IFE", vm, b, a);
		vm_op_ife(vm, b, a);
		break;
	case OP_IFN:
		vm_print_op("IFN", vm, b, a);
		vm_op_ifn(vm, b, a);
		break;
	case OP_IFG:
		vm_print_op("IFG", vm, b, a);
		vm_op_ifg(vm, b, a);
		break;
	case OP_IFB:
		vm_print_op("IFB", vm, b, a);
		vm_op_ifb(vm, b, a);
		break;
	case OP_NONBASIC:
		switch (b)
		{
		case NBOP_JSR:
			vm_print_op_nonbasic("JSR", vm, a);
			vm_op_jsr(vm, a);
			break;
		case NBOP_INT:
			vm_print_op_nonbasic("INT", vm, a);
			vm_op_int(vm, a);
			break;
		case NBOP_ING:
			vm_print_op_nonbasic("ING", vm, a);
			vm_op_ing(vm, a);
			break;
		case NBOP_INS:
			vm_print_op_nonbasic("INS", vm, a);
			vm_op_ins(vm, a);
			break;
		case NBOP_HWN:
			vm_print_op_nonbasic("HWN", vm, a);
			vm_op_hwn(vm, a);
			break;
		case NBOP_HWQ:
			vm_print_op_nonbasic("HWQ", vm, a);
			vm_op_hwq(vm, a);
			break;
		case NBOP_HWI:
			vm_print_op_nonbasic("HWI", vm, a);
			vm_op_hwi(vm, a);
			break;
		default:
			vm_halt(vm, "Invalid non-basic opcode %u.", b);
			break;
		}
		break;
	default:
		vm_halt(vm, "Invalid opcode %u.", op);
		break;
	}
	if (vm->debug)
		printf("\n");
	vm_hook_fire(vm, 0, HOOK_ON_CYCLE);
}
