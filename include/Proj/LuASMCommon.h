#ifndef LU_ASM_COMMON
#define LU_ASM_COMMON

#include <LuLib.h>
#include <LuASMDefines.h>
#include "leak_detector_c.h"

extern uchar codeRAM[UINT16_MAX + 1];
extern uchar miniRAM[UINT16_MAX + 1];

extern ushort sleepCycle;
extern ushort fetched;
extern ushort codePtr;

extern ushort reg_x;
extern ushort reg_y;
extern ushort reg_z;
extern ushort stackPtr;
extern ushort flags;

extern bool systemEndianness;

bool checkSystemEndianness();
void resetLuASM();

bool isFlagSet(ushort flag);
void setFlag(ushort flag, bool val);

ushort readMiniRAM(ushort addr);
ushort readCodeRAM(bool readOneByte);

void pushStack(ushort val, bool checkByteMode);
ushort popStack(bool checkByteMode);

void writeBytes(void* dest, ushort val);

void* modeAsDest(uchar mode);
ushort modeAsSrc(uchar mode);
ushort* modeAsReg(uchar mode);

#endif  // LU_ASM_COMMON
