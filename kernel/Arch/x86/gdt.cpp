#include "gdt.h"
#include "Lib/stdlib.h"
#include "paging.h"

volatile GDT_DESCRIPTOR GDT::gdt __attribute__((aligned(8)));
volatile GDT_ENTRY GDT::gdt_entries[GDT_NUMBER_OF_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
volatile TSS_ENTRY GDT::tss_entry __attribute__((aligned(PAGE_SIZE)));
void GDT::setup()
{
	memset((void*)&tss_entry, 0, sizeof(TSS_ENTRY));
	memset((void*)&gdt_entries, 0, sizeof(GDT_ENTRY) * GDT_NUMBER_OF_ENTRIES);

	fill_gdt((uint32_t)&gdt_entries, GDT_NUMBER_OF_ENTRIES * sizeof(GDT_ENTRY));

	// Empty Entry
	fill_gdt_entry(0, 0, 0, 0, 0);
	// Kernel Segments
	fill_gdt_entry(SEGMENT_INDEX(KCS_SELECTOR), 0, 0xFFFFF, GDT_CODE_PL0, 0x0D);
	fill_gdt_entry(SEGMENT_INDEX(KDS_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL0, 0x0D);
	// User Entries
	fill_gdt_entry(SEGMENT_INDEX(UCS_SELECTOR), 0, 0xFFFFF, GDT_CODE_PL3, 0x0D);
	fill_gdt_entry(SEGMENT_INDEX(UDS_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL3, 0x0D);
	// TSS
	fill_gdt_entry(SEGMENT_INDEX(TSS_SELECTOR), (uint32_t)&tss_entry, sizeof(TSS_ENTRY), GDT_TSS_PL3, 0);

	load_gdt();

	load_segments(KCS_SELECTOR, KDS_SELECTOR);
	setup_tss(0);
}

void GDT::set_tss_stack(uint32_t kernel_stack)
{
	tss_entry.esp0 = kernel_stack;
	tss_entry.ss0 = KDS_SELECTOR;
	//--------------------------
	// tss_entry.cs = KCS_SELECTOR;
	// tss_entry.es = tss_entry.ss = tss_entry.fs = tss_entry.ds = tss_entry.gs = KDS_SELECTOR;
	// load_tss(TSS_SELECTOR);
}

void GDT::setup_tss(uint32_t kernel_stack)
{
	set_tss_stack(kernel_stack);
	load_tss(TSS_SELECTOR);
}

void GDT::fill_gdt_entry(uint32_t gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entries[gdt_entry].base0_15 = base & 0x0000FFFF;
	gdt_entries[gdt_entry].base16_23 = (base & 0x00FF0000) >> 16;
	gdt_entries[gdt_entry].base24_31 = (base & 0xFF000000) >> 24;
	gdt_entries[gdt_entry].lim0_15 = limit & 0xFFFF;
	gdt_entries[gdt_entry].lim16_19 = (limit & 0x0F0000) >> 16;
	gdt_entries[gdt_entry].access = access;
	gdt_entries[gdt_entry].flags = flags;
}

void GDT::fill_gdt(uint32_t base, uint16_t limit)
{
	gdt.base = base;
	gdt.limit = limit;
}

void GDT::load_gdt()
{
	asm volatile("LGDT (%0)" : : "r"(&gdt));
}

void GDT::load_tss(uint16_t tss)
{
	asm volatile("LTR %0" : : "a"(tss));
}

void GDT::load_segments(uint16_t code_segment, uint16_t data_segment)
{
	asm volatile(" MOVW %0,%%ds   \t\n\
				   MOVW %0,%%es   \t\n\
				   MOVW %0,%%ss   \t\n\
				   MOVW %0,%%fs   \t\n\
				   MOVW %0,%%gs"
	             :
	             : "r"(data_segment));
	asm volatile("PUSHL %0\n      \t\n\
				  PUSHL $far_jmp%=   \t\n\
				  RETF            \t\n\
				  far_jmp%=:         \t\n\
				  nop"
	             :
	             : "r"((uint32_t)code_segment));
}
