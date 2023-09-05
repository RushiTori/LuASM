#include "LuASMInstructions.h"

void NOP() { return; }

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
	((uchar*)&flags)[!systemEndianness] = controlFlags;
}

void INC() {
	// WIP
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);
	ushort prevValue, newValue;

	if (isFlagSet(FLAG_8BitsMode)) {
		prevValue = (*((uchar*)dest));
		(*((uchar*)dest))++;
		newValue = (*((uchar*)dest));
	} else {
		prevValue = (*((ushort*)dest));
		(*((ushort*)dest))++;
		newValue = (*((ushort*)dest));
	}

	setFlag(FLAG_Overflow | FLAG_Carry | FLAG_Zero | FLAG_Negative, newValue < prevValue);
}

void DEC() {
	// WIP
	uchar mode = readCodeRAM(true);
	void* dest = modeAsDest(mode);
	ushort prevValue, newValue;

	if (isFlagSet(FLAG_8BitsMode)) {
		prevValue = (*((uchar*)dest));
		(*((uchar*)dest))--;
		newValue = (*((uchar*)dest));
	} else {
		prevValue = (*((ushort*)dest));
		(*((ushort*)dest))--;
		newValue = (*((ushort*)dest));
	}

	setFlag(FLAG_Overflow | FLAG_Carry | FLAG_Zero | FLAG_Carry, newValue > prevValue);
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
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	uint result = opA + opB;
	//if(result > (isFlagSet(FLAG_8BitsMode) ? UINT8_MAX : UINT16_MAX)) printf("%d\n", result);

	writeBytes(dest, result & UINT16_MAX);

	setFlag(FLAG_Zero, result == 0);
	setFlag(FLAG_Carry | FLAG_Overflow, result > (isFlagSet(FLAG_8BitsMode) ? UINT8_MAX : UINT16_MAX));
	//if(result > (isFlagSet(FLAG_8BitsMode) ? UINT8_MAX : UINT16_MAX)) printf("%d\n", isFlagSet(FLAG_Carry));
	
}

void ADC() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	uint result = opA + opB + isFlagSet(FLAG_Carry);

	writeBytes(dest, result & UINT16_MAX);

	setFlag(FLAG_Zero, result == 0);
	setFlag(FLAG_Carry | FLAG_Overflow, result > UINT16_MAX);
}

void SUB() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA - opB;

	writeBytes(dest, result);

	setFlag(FLAG_Zero, result == 0);
	setFlag(FLAG_Carry | FLAG_Overflow, opB > opA);
}

void SBC() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA - opB - isFlagSet(FLAG_Carry);

	writeBytes(dest, result);

	setFlag(FLAG_Zero, result == 0);
	setFlag(FLAG_Carry | FLAG_Overflow, opB > opA);
}

void MUL() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	uint result = opA * opB;

	writeBytes(dest, result & UINT16_MAX);
	writeBytes(miniRAM, result >> 16);

	setFlag(FLAG_Zero, result == 0);
	setFlag(FLAG_Carry | FLAG_Overflow, result > UINT16_MAX);
}

void DIV() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA / opB;

	writeBytes(dest, result);

	setFlag(FLAG_Zero, result == 0);
}

void MOD() {
	// WIP
	uchar modeA = readCodeRAM(true);
	uchar modeB = readCodeRAM(true);
	uchar modeBHigh = extractModeHigh(modeB);
	uchar modeBLow = extractModeLow(modeB);

	void* dest = modeAsDest(modeA);
	ushort opA = modeAsSrc(modeBHigh);
	ushort opB = modeAsSrc(modeBLow);
	ushort result = opA % opB;

	writeBytes(dest, result);

	setFlag(FLAG_Zero, result == 0);
}

void CMP() {
	// WIP
	uchar mode = readCodeRAM(true);
	uchar modeHigh = extractModeHigh(mode);
	uchar modeLow = extractModeLow(mode);

	ushort srcA = modeAsSrc(modeHigh);
	ushort srcB = modeAsSrc(modeHigh);

	setFlag(FLAG_Zero, srcA == srcB);
	setFlag(FLAG_Carry | FLAG_Overflow, srcB > srcA);
}

void JMPA() {
	bool byteMode = isFlagSet(FLAG_8BitsMode);
	setFlag(FLAG_8BitsMode, false);

	ushort jmpAddr = readCodeRAM(true);
	codePtr = jmpAddr;

	setFlag(FLAG_8BitsMode, byteMode);
}

void JMPR() {
	// WIP
	char jmpAddr = readCodeRAM(true) & 0xFF;
	codePtr += jmpAddr;
}

void JMPAC() {
	// WIP
	uchar mode = readCodeRAM(true);

	ushort jmpAddr = readCodeRAM(false);

	if (evalCond(mode & JMP_NotC, mode & JMP_CheckC, mode & JMP_NotZ, mode & JMP_CheckZ)) {
		codePtr = jmpAddr;
	}
}

void JMPRC() {
	// WIP
	uchar mode = readCodeRAM(true);
	char jmpAddr = (uchar)readCodeRAM(true);

	if (evalCond(mode & JMP_NotC, mode & JMP_CheckC, mode & JMP_NotZ, mode & JMP_CheckZ)) {
		codePtr += jmpAddr;
	}
}

void CALL() {
	ushort jmpAddr = readCodeRAM(false);

	pushStack(codePtr, false);
	codePtr = jmpAddr;
}

void RET() { codePtr = popStack(false); }

void PUSH() {
	uchar mode = readCodeRAM(true);
	ushort* regSrc = modeAsReg(mode);
	pushStack(*regSrc, false);
}

void POP() {
	uchar mode = readCodeRAM(true);
	ushort* regDest = modeAsReg(mode);
	*regDest = popStack(false);
}
