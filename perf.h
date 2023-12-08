#pragma once

#include <kernel.h>
#include <sio.h>

// Define the events for perf_counter0
typedef enum {
    PC0_Cycle = 1,
    PC0_InstructionIssue,
    PC0_BranchIssued,
    PC0_BTACMiss,
    PC0_ITLBMiss,
    PC0_ICacheMiss,
    PC0_AccessToDTLB,
    PC0_NonBlockingLoad,
    PC0_WBBSingleRequest,
    PC0_WBBBurstRequest,
    PC0_AddressBusBusy,
    PC0_InstructionCompleted,
    PC0_NonBDSInstructionCompleted,
    PC0_COP2InstructionCompleted,
    PC0_LoadCompleted,
    PC0_NoEvent
} perf_counter0_event;

// Define the events for perf_counter1
typedef enum {
    PC1_LowOrderBranchIssue,
    PC1_ProcessorCycle,
    PC1_DualInstructionIssue,
    PC1_BranchMispredicted,
    PC1_TLBMiss,
    PC1_DTLBMiss,
    PC1_DCacheMiss,
    PC1_WBBSingleReqUnavailable,
    PC1_WBBBurstReqUnavailable,
    PC1_WBBBurstReqAlmostFull,
    PC1_WBBBurstReqFull,
    PC1_DataBusBusy,
    PC1_InstructionCompleted,
    PC1_NonBDSInstructionCompleted,
    PC1_COP1InstructionCompleted,
    PC1_StoreCompleted,
    PC1_NoEvent
} perf_counter1_event;


/// Performance counter register
/// Same for both counters
typedef union
{
	struct
	{
		u32 VALUE : 31;
		u32 OVFL : 1;
	};
	u32 _u32;
} PCR;

/// Performance counter control register
typedef union
{
	struct
	{
		u32 EL1Exception : 1;
		u32 EKernel : 1;
		u32 ESuperviser : 1;
		u32 EUser : 1;
		u32 Event : 5;
	};
	u32 _u32;
} PCCR;

typedef u8 perf_counter;

typedef void(*perf_handler)(PCR counter0, PCR counter1);

static const perf_counter PERF_COUNTER_0 = 0;
static const perf_counter PERF_COUNTER_1 = 1;

/// Installs a handler to the EL2 V_COUNT vector
/// Clears the counters and their configs
/// handler can be NULL. If NULL, the counters will be cleared when they overflow.
#define perf_install_handler(handler) perf_install_handler2(handler, 0)

/// Installs a handler to the EL2 V_COUNT vector
/// Clears the counters and their configs
/// If override is 1, the handler value is ignored and the counters will not be cleared when they overflow.
void perf_install_handler2(perf_handler handler, u8 override);

/// Get a specific counter value
PCR perf_get_counter(perf_counter counter);

/// Set a specific counter value
void perf_set_counter(perf_counter counter, PCR value);

/// Clear the counter value
/// (Same as perf_set_counter(counter, (PCR){.VALUE = 0}))
void perf_clear_counter(perf_counter counter);

/// Set the counter config
void perf_set_counter_config(perf_counter counter, PCCR config);

/// Get the counter config
void perf_get_counter_config(perf_counter counter, PCCR *config);

void perf_toggle_counters(u8 enable);
