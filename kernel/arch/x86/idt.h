#pragma once

#include "types.h"

#define NUMBER_OF_IDT_ENTRIES         255
#define NUMBER_OF_IDT_DEFINED_ENTRIES 21
#define NUMBER_OF_IDT_EXCEPTIONS      32

enum IDT_ENTRY_FLAGS {
	PRESENT = 0x80,
	RING3 = 0x60,
	RING0 = 0x00,
	TRAP_GATE = 0x7,
	INT_GATE = 0x6,
	TASK_GATE = 0x5,
	GATE_32 = 0x8,
	GATE_16 = 0
};

enum EXCEPTION_NUMBER {
	DE = 0,
	DB = 1,
	NMI = 2,
	BP = 3,
	OF = 4,
	BR = 5,
	UD = 6,
	NM = 7,
	DF = 8,
	TS = 10,
	NP = 11,
	SS = 12,
	GP = 13,
	PF = 14,
	MF = 16,
	AC = 17,
	MC = 18,
	XM = 19,
	VE = 20
};

#pragma pack(1)
struct IDT {
	uint16_t limit;
	uint32_t base;
};
struct IDTEntry {
	uint16_t offset0_15;
	uint16_t segment;
	uint8_t zero;
	uint8_t type;
	uint16_t offset16_31;
};
#pragma pack()

void setup_idt();
static void fill_idt(volatile IDT* idt, uint32_t base, uint16_t limit);
static void fill_idt_entry(volatile IDTEntry* idt_entry, uint32_t address, uint16_t segment, uint8_t type);
static void load_idt(volatile IDT* idt);
