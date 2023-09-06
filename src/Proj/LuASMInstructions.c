#include "LuASMInstructions.h"

void NOP() {
	// printf("Registers :\n");
	printf("\tX      : %d\n", (ushort)reg_x);
	// printf("\tY      : %d\n", reg_y);
	// printf("\tZ      : %d\n", reg_z);
	// printf("\tsPtr   : %d\n", stackPtr);
	// printf("\t[sPtr] : %d\n", readMiniRAM(stackPtr - 2));
	// printf("\tpc     : %d\n", codePtr - 1);

	// printf("State Flags :\n");
	// printf("\tZero           : %1d\n", isFlagSet(FLAG_Zero));
	// printf("\tCarry          : %1d\n", isFlagSet(FLAG_Carry));
	// printf("\tNegative       : %1d\n", isFlagSet(FLAG_Negative));
	// printf("\tOverflow       : %1d\n", isFlagSet(FLAG_Overflow));

	// printf("Control Flags :\n");
	// printf("\t8BitsMode      : %1d\n", isFlagSet(FLAG_8BitsMode));
	// printf("\tEndianMode     : %1d\n", isFlagSet(FLAG_EndianMode));
	// printf("\tFrameClockMode : %1d\n", isFlagSet(FLAG_FrameClockMode));
}

void WAIT() {
	uchar mode = readCodeRAM(true);
	sleepCycle = modeAsSrc(mode);
}

void STR() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);

	writeBytes(dest, srcValue);
}

void STZ() {
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);
	writeBytes(dest, 0);
}

void SCF() {
	uchar controlFlags = readCodeRAM(true);
	flags = (flags & 0xFF00) | controlFlags;
}

void LSL() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= (maxShift + 1);

	ushort result = readBytes(dest);
	result <<= srcValue;
	writeBytes(dest, result);

	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void LSR() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= (maxShift + 1);

	ushort result = readBytes(dest);
	result >>= srcValue;
	writeBytes(dest, result);

	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void ASL() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= maxShift;

	if (isFlagSet(FLAG_8BitsMode)) {
		uchar result = readBytes(dest);
		bool signBit = result & 0x80;
		result <<= srcValue;
		if (signBit) result |= 0x80;
		writeBytes(dest, result);
	} else {
		ushort result = readBytes(dest);
		bool signBit = result & 0x8000;
		result <<= srcValue;
		if (signBit) result |= 0x8000;
		writeBytes(dest, result);
	}
}

void ASR() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= maxShift;

	if (isFlagSet(FLAG_8BitsMode)) {
		uchar result = readBytes(dest);
		bool signBit = result & 0x80;
		uchar signMask = ~(0x7F >> srcValue);
		result >>= srcValue;
		if (signBit) result |= signMask;
		writeBytes(dest, result);
	} else {
		ushort result = readBytes(dest);
		bool signBit = result & 0x8000;
		ushort signMask = ~(0x7FFF >> srcValue);
		result >>= srcValue;
		if (signBit) result |= signMask;
		writeBytes(dest, result);
	}
}

void BSL() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= (maxShift + 1);

	if (isFlagSet(FLAG_8BitsMode)) {
		*((uchar*)dest) = (*((uchar*)dest) << srcValue) | (*((uchar*)dest) >> (maxShift - srcValue));
	} else {
		ushort result = readBytes(dest);
		result = (result << srcValue) | (result >> (maxShift - srcValue));
		writeBytes(dest, result);
	}
}

void BSR() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	void* dest = modeAsDest(modeHigh);
	ushort srcValue = modeAsSrc(modeLow);
	ushort maxShift = CHAR_BIT * ((isFlagSet(FLAG_8BitsMode)) ? sizeof(uchar) : sizeof(ushort));
	srcValue %= (maxShift + 1);

	if (isFlagSet(FLAG_8BitsMode)) {
		*((uchar*)dest) = (*((uchar*)dest) >> srcValue) | (*((uchar*)dest) << (maxShift - srcValue));
	} else {
		ushort result = readBytes(dest);
		result = (result >> srcValue) | (result << (maxShift - srcValue));
		writeBytes(dest, result);
	}
}

void INC() {
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);

	int result = readBytes(dest);
	writeBytes(dest, ++result);

	setStateFlags(false, result);
}

void DEC() {
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);

	int result = readBytes(dest);
	writeBytes(dest, --result);

	setStateFlags(true, result);
}

void AND() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = opA & opB;
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void NAND() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = ~(opA & opB);
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void OR() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = opA | opB;
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void NOR() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = ~(opA | opB);
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void XOR() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = opA ^ opB;
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void XNOR() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);

	ushort result = ~(opA ^ opB);
	writeBytes(dest, result);
	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void NOT() {
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);

	if (isFlagSet(FLAG_8BitsMode)) {
		(*((uchar*)dest)) ^= UINT8_MAX;
		setFlag(FLAG_Zero, (*((uchar*)dest)) == 0);
	} else {
		(*((ushort*)dest)) ^= UINT16_MAX;
		setFlag(FLAG_Zero, (*((ushort*)dest)) == 0);
	}
}

void ADD() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	int result = opA + opB;

	writeBytes(dest, result);

	setStateFlags(false, result);
}

void ADC() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	int result = opA + opB + isFlagSet(FLAG_Carry);

	writeBytes(dest, result);

	setStateFlags(false, result);
}

void SUB() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	int result = opA - opB;

	writeBytes(dest, result);

	setStateFlags(true, result);
}

void SBC() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	int result = opA - opB - isFlagSet(FLAG_Carry);

	writeBytes(dest, result);

	setStateFlags(true, result);
}

void MUL() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	int result = opA * opB;

	writeBytes(dest, result & UINT16_MAX);
	writeBytes(miniRAM, (result & 0xFFFF0000) / 0x00010000);

	setStateFlags(false, result);
}

void DIV() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA / opB;

	writeBytes(dest, result);

	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void MOD() {
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA % opB;

	writeBytes(dest, result);

	if (isFlagSet(FLAG_8BitsMode)) result &= 0xFF;
	setFlag(FLAG_Zero, result == 0);
}

void CMP() {
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	ushort opA = modeAsSrc(modeHigh);
	ushort opB = modeAsSrc(modeLow);
	int result = opA - opB;

	setStateFlags(true, result);
}

void JMPA() {
	ushort jmpAddr = readCodeRAM(false);
	codePtr = jmpAddr;
}

void JMPR() {
	char jmpAddr = readCodeRAM(true) & 0xFF;
	codePtr += jmpAddr;
}

void JMPAC() {
	uchar mode = readCodeRAM(true);
	ushort jmpAddr = readCodeRAM(false);
	if (evalCond(mode)) codePtr = jmpAddr;
}

void JMPRC() {
	uchar mode = readCodeRAM(true);
	char jmpAddr = (uchar)readCodeRAM(true);
	if (evalCond(mode)) codePtr += jmpAddr;
}

void CALL() {
	ushort jmpAddr = readCodeRAM(false);

	pushStack(codePtr, false);
	codePtr = jmpAddr;
}

void RET() { codePtr = popStack(false); }

void PUSH() {
	uchar mode = readCodeRAM(true);
	ushort srcValue = modeAsSrc(mode);
	pushStack(srcValue, false);
}

void POP() {
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);
	writeBytes(dest, popStack(false));
}
