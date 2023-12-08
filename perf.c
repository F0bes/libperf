#include "perf.h"
#include <kernel.h> // EI, DI

// Pointer to the instruction where we can insert our `jal` instruction
extern uiptr handler_jal_area;

// The actual layout of the pccr register
typedef union
{
	struct
	{
		u32 : 1;
		u32 EXL0 : 1;
		u32 K0 : 1;
		u32 S0 : 1;
		u32 U0 : 1;
		u32 EVENT0 : 5;
		u32 : 1;
		u32 EXL1 : 1;
		u32 K1 : 1;
		u32 S1 : 1;
		u32 U1 : 1;
		u32 EVENT1 : 5;
		u32 : 10;
		u32 CTE;
	};
	u32 _u32;
} PCCR_REG;

#define MIPS_J(func) (u32)((0x02 << 26) | (((u32)func) / 4))
#define MIPS_JAL(func) (u32)((0x03 << 26) | (((u32)func) / 4))
void perf_default_handler(PCR counter0, PCR counter1)
{
	if (counter0.OVFL)
		perf_clear_counter(PERF_COUNTER_0);
	if (counter1.OVFL)
		perf_clear_counter(PERF_COUNTER_1);
}

extern void level_2_handler();

void perf_install_handler2(perf_handler handler, u8 override)
{
	DI();
	ee_kmode_enter();
	*(vu32 *)UNCACHED_SEG(0x80000080) = MIPS_J(UNCACHED_SEG(level_2_handler));
	*(vu32 *)UNCACHED_SEG(0x80000084) = 0;

	asm volatile(
		"mtps $zero, 0\n"
		"mtpc $zero, 0\n"
		"mtpc $zero, 1\n"
		"sync.p\n");
	ee_kmode_exit();
	EI();

	// What a stupid, stupid, stupid hack
	// There is some sort of issue with the assembler or compiler which makes it impossible
	// to reliably pass a global from here to the interrupt handler.
	// So instead of having a global function pointer, we write the jump directly in the assembly.
	if (override)
	{
		*(uiptr *)UNCACHED_SEG(&handler_jal_area) = 0x00;
	}
	else
	{
		if (handler == NULL)
		{
			sio_puts("Hey, it's perf here. I hope you know what you're doing not setting a handler.\n");
			sio_puts("Assuming that you don't, I'll clear the counters for you when they overflow.\n");
			sio_puts("To override this behaviour, pass -1 as the handler.\n");
			*(uiptr *)UNCACHED_SEG(&handler_jal_area) = MIPS_JAL(UNCACHED_SEG(perf_default_handler));
		}
		else
		{
			*(uiptr *)UNCACHED_SEG(&handler_jal_area) = MIPS_JAL(UNCACHED_SEG(handler));
		}
	}

	FlushCache(INVALIDATE_ICACHE);
}

PCR perf_get_counter(perf_counter counter)
{
	DI();
	PCR pcr;
	if (counter == PERF_COUNTER_0)
		asm(
			"mfpc %0, 0\n"
			: "=r"(pcr._u32));
	else if (counter == PERF_COUNTER_1)
		asm(
			"mfpc %0, 1\n"
			: "=r"(pcr._u32));

	EI();
	return pcr;
}

void perf_set_counter(perf_counter counter, PCR value)
{
	DI();
	if (counter == PERF_COUNTER_0)
		asm(
			"mtpc %0, 0\n"
			:
			: "r"(value._u32));
	else if (counter == PERF_COUNTER_1)
		asm(
			"mtpc %0, 1\n"
			:
			: "r"(value._u32));
	EI();
}

void perf_clear_counter(perf_counter counter)
{
	DI();
	if (counter == PERF_COUNTER_0)
		asm("mtpc $zero, 0\n");
	else if (counter == PERF_COUNTER_1)
		asm("mtpc $zero, 1\n");
	EI();
}

void perf_set_counter_config(perf_counter counter, PCCR config)
{
	DI();
	PCCR_REG pccr;
	asm(
		"mfps %0, 0\n"
		: "=r"(pccr._u32));

	if (counter == PERF_COUNTER_0)
	{
		pccr.EXL0 = config.EL1Exception;
		pccr.K0 = config.EKernel;
		pccr.S0 = config.ESuperviser;
		pccr.U0 = config.EUser;
		pccr.EVENT0 = config.Event;
	}
	else if (counter == PERF_COUNTER_1)
	{
		pccr.EXL1 = config.EL1Exception;
		pccr.K1 = config.EKernel;
		pccr.S1 = config.ESuperviser;
		pccr.U1 = config.EUser;
		pccr.EVENT1 = config.Event;
	}

	asm(
		"mtps %0, 0\n"
		:
		: "r"(pccr._u32));

	//	printf("perf_set_counter_config: %08X\n", pccr._u32);
	if (counter == PERF_COUNTER_0)
		asm("mtpc $zero, 0\n");
	else if (counter == PERF_COUNTER_1)
		asm("mtpc $zero, 1\n");
	EI();
}

void perf_get_counter_config(perf_counter counter, PCCR *config)
{
	DI();
	PCCR_REG pccr;
	asm(
		"mfps %0, 0\n"
		: "=r"(pccr._u32));

	if (counter == PERF_COUNTER_0)
	{
		config->EL1Exception = pccr.EXL0;
		config->EKernel = pccr.K0;
		config->ESuperviser = pccr.S0;
		config->EUser = pccr.U0;
		config->Event = pccr.EVENT0;
	}
	else if (counter == PERF_COUNTER_1)
	{
		config->EL1Exception = pccr.EXL1;
		config->EKernel = pccr.K1;
		config->ESuperviser = pccr.S1;
		config->EUser = pccr.U1;
		config->Event = pccr.EVENT1;
	}
	EI();
}

void perf_toggle_counters(u8 enable)
{
	DI();
	asm(
		"mfps $t0, 0\n"
		"li $t1, 0x7FFFFFFF\n"
		"and $t0, $t0, $t1\n"
		"sll $t1, %0, 31\n"
		"or $t0, $t0, $t1\n"
		"mtps $t0, 0\n"
		"sync.p\n" ::"r"(enable));
	EI();
}
