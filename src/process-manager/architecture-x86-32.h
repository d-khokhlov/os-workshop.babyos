#pragma once

#define COMMON_REGISTERS_COUNT 8
#define POPA_STACK_POINTER_INDEX 3

typedef unsigned int MachineWord;
typedef unsigned short SegmentSelector;

typedef MachineWord CommonRegister;
typedef MachineWord FlagsRegister;
typedef MachineWord InstructionPointer;
typedef SegmentSelector SegmentRegister;

#define NEAR_POINTER_SIZE sizeof( MachineWord )
#define FAR_POINTER_SIZE ( NEAR_POINTER_SIZE + sizeof( SegmentSelector ) )

// todo: Добавить типы IrqNumber, InterruptNumber, PortNumber и использовать их
// везде, где нужно.
