#include "common.h"
#include "timer.h"
#include "mocks.h"
#include "interrupts.h"
#include "context.h"
#include "process-manager.h"

#define _TIMER_FREQUENCY_HZ 1193180ul
#define _TIME_QUANTUM_MS 1ul
#define _TIMER_COUNTER_START_VALUE \
    ( _TIME_QUANTUM_MS * _TIMER_FREQUENCY_HZ / 1000ul )

#define _TIMER1_CONTROL_PORT 0x43
#define _TIMER1_CHANNEL0_PORT 0x40
#define _TIMER1_CHANNEL1_PORT 0x41
#define _TIMER1_CHANNEL2_PORT 0x42
#define _TIMER_CHOOSE_CHANNEL0 0x00
#define _TIMER_CHOOSE_CHANNEL1 0x40
#define _TIMER_CHOOSE_CHANNEL2 0x80
#define _TIMER_CHANNEL_MODE 0x36

static void _HandleTimerInterrupt()
{
    ActivateProcess( ChooseProcessToActivate() );
}

static void naked _TimerInterruptHandler()
{
    asm {
        call SwitchContextToKernel
        call _HandleTimerInterrupt
        call SwitchContextToProcess
    }
    EndInterrupt( PIC1 )
}

extern void InitTimer()
{
    OutByteToPort( _TIMER1_CONTROL_PORT,
        _TIMER_CHOOSE_CHANNEL0 | _TIMER_CHANNEL_MODE );
    OutByteToPort( _TIMER1_CHANNEL0_PORT, _TIMER_COUNTER_START_VALUE & 0xFF );
    OutByteToPort( _TIMER1_CHANNEL0_PORT, _TIMER_COUNTER_START_VALUE >> 8 );
    SetInterruptHandler( INTERRUPT_TIMER, &_TimerInterruptHandler );
}
