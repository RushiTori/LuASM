#ifndef LU_ASM_DEFINES_H
#define LU_ASM_DEFINES_H

#include <LuLib/LuTypes.h>

#define FLAG_Zero           ((ushort)0b1000000000000000) // check if zero
#define FLAG_Carry          ((ushort)0b0100000000000000) // check if greater than MAX
#define FLAG_Negative       ((ushort)0b0010000000000000) // check most significant bit
#define FLAG_Overflow       ((ushort)0b0001000000000000) // check if greater than MAX (check for signedness)

#define FLAG_8BitsMode      ((ushort)0b0000000010000000)
#define FLAG_EndianMode     ((ushort)0b0000000001000000)
#define FLAG_UnsignedMode   ((ushort)0b0000000000100000)
#define FLAG_FrameClockMode ((ushort)0b0000000000010000)

#define RAM_UnknownTerritoryBegin ((ushort)0x0000)
#define RAM_UnknownTerritoryEnd   ((ushort)0x00FF)

#define RAM_StackBegin            ((ushort)0x0100)
#define RAM_StackEnd              ((ushort)0x02FF)

#define RAM_UserVarBegin          ((ushort)0x0300)
#define RAM_UserVarEnd            ((ushort)0x7FFF)

#define RAM_UserConstBegin        ((ushort)0x8000)
#define RAM_UserConstEnd          ((ushort)0xEFFF)

#define RAM_GameVarBegin          ((ushort)0xF000)
#define RAM_GameVarEnd            ((ushort)0xFFFF)

#define extractModeLow(mode)                  (((mode) & 0b00001111))
#define extractModeHigh(mode)                 (((mode) & 0b11110000) >> 4)
#define combineModeLowHigh(modeLow, modeHigh) (((modeHigh) << 4) | (modeLow))

typedef enum InstArgMode {
  MODE_REG_X     = 0b0000,
  MODE_REG_Y     = 0b0001,
  MODE_REG_Z     = 0b0010,
  MODE_SPTR      = 0b0011,
  MODE_VALUE     = 0b0100,
  MODE_RAM_REG_X = 0b1000,
  MODE_RAM_REG_Y = 0b1001,
  MODE_RAM_REG_Z = 0b1010,
  MODE_RAM_SPTR  = 0b1011,
  MODE_RAM_VALUE = 0b1100,
} InstArgMode;

#define JMP_CheckZ 0b1000
#define JMP_NotZ   0b0100
#define JMP_CheckC 0b0010
#define JMP_NotC   0b0001

#define evalCond(notC, checkC, notZ, checkZ) ((!(checkC) || (isFlagSet(FLAG_Carry) ^ (bool)(notC))) && \
                                              (!(checkZ) || (isFlagSet(FLAG_Zero)  ^ (bool)(notZ))))

typedef enum ConditionType {
	COND_None            = (!JMP_CheckZ) | (!JMP_NotZ) | (!JMP_CheckC) | (!JMP_NotC),

	COND_Zero            = ( JMP_CheckZ) | (!JMP_NotZ) | (!JMP_CheckC) | (!JMP_NotC),
	COND_NotZero         = ( JMP_CheckZ) | ( JMP_NotZ) | (!JMP_CheckC) | (!JMP_NotC),

	COND_Carry           = (!JMP_CheckZ) | (!JMP_NotZ) | ( JMP_CheckC) | (!JMP_NotC),
	COND_NotCarry        = (!JMP_CheckZ) | (!JMP_NotZ) | ( JMP_CheckC) | ( JMP_NotC),

	COND_CarryZero       = ( JMP_CheckZ) | (!JMP_NotZ) | ( JMP_CheckC) | (!JMP_NotC),
	COND_NotCarryZero    = ( JMP_CheckZ) | (!JMP_NotZ) | ( JMP_CheckC) | ( JMP_NotC),

	COND_CarryNotZero    = ( JMP_CheckZ) | ( JMP_NotZ) | ( JMP_CheckC) | (!JMP_NotC),
	COND_NotCarryNotZero = ( JMP_CheckZ) | ( JMP_NotZ) | ( JMP_CheckC) | ( JMP_NotC),
} ConditionType;

typedef enum InstArgTreament {
	NO_ARGS,
	MAYBE_ONE_ARG,
	DEST_SRC,
	ONEBYTE_VALUE,
	DEST,
	DEST_OPA_OPB,
	OPA_OPB,
	ADDR_ABS_REL,
	ADDR_ABS,
	REGISTER
} InstArgTreament;

typedef enum OpCode {
  OP_BADOP = 0x00,
  OP_NOP   = 0x01,
  OP_WAIT  = 0x02,
  OP_STR   = 0x03,
  OP_STZ   = 0x04,
  OP_SCF   = 0x05,
  OP_INC   = 0x10,
  OP_DEC   = 0x11,
  OP_AND   = 0x12,
  OP_NAND  = 0x13,
  OP_OR    = 0x14,
  OP_NOR   = 0x15,
  OP_XOR   = 0x16,
  OP_XNOR  = 0x17,
  OP_NOT   = 0x18,
  OP_ADD   = 0x19,
  OP_ADC   = 0x1A,
  OP_SUB   = 0x1B,
  OP_SBC   = 0x1C,
  OP_MUL   = 0x1D,
  OP_DIV   = 0x1E,
  OP_MOD   = 0x1F,
  OP_CMP   = 0x20,
  OP_JMPA  = 0x21,
  OP_JMPR  = 0x22,
  OP_JMPAC = 0x23,
  OP_JMPRC = 0x24,
  OP_CALL  = 0x25,
  OP_RET   = 0x26,
  OP_PUSH  = 0x27,
  OP_POP   = 0x28
} OpCode;

#endif	// LU_ASM_DEFINES_H