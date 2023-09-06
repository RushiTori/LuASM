#include "LuASMCommon.h"

uchar codeRAM[UINT16_MAX + 1];
uchar miniRAM[UINT16_MAX + 1];

ushort sleepCycle;
ushort fetched;
ushort codePtr;

ushort stackPtr;
ushort reg_x;
ushort reg_y;
ushort reg_z;
ushort flags;

bool systemEndianness;

void resetLuASM() {
	systemEndianness = checkSystemEndianness();
	memset(codeRAM, 0, sizeof(uchar) * (UINT16_MAX + 1));
	memset(miniRAM, 0, sizeof(uchar) * (UINT16_MAX + 1));

	sleepCycle = 0;
	fetched = 0;
	codePtr = 0;

	stackPtr = 0x0100;
	reg_x = 0;
	reg_y = 0;
	reg_z = 0;
	flags = FLAG_UnsignedMode | FLAG_FrameClockMode;
	setFlag(FLAG_EndianMode, systemEndianness);
}

bool checkSystemEndianness() {
	uint temp = 1;
	return *((char*)&temp);
}

// Zero : if result of operation is zero
// Carry : if result of operation is greater than UMAX
// Overflow : if result of operation is greater than SMAX (when inc, add and mul) or less than SMIN (when dec and sub)
// Negative : if the result is negative
void setStateFlags(bool isFromSubDec, int result) {
	setFlag(FLAG_Zero, result == 0);

	setFlag(FLAG_Carry, result > getCPUMaxUValue());

	if (isFromSubDec) {
		setFlag(FLAG_Overflow, result < getCPUMinSValue());
	} else {
		setFlag(FLAG_Overflow, result > getCPUMaxSValue());
	}

	setFlag(FLAG_Negative, result < 0);
}

bool isFlagSet(ushort flag) { return (flags & flag); }

void setFlag(ushort flag, bool val) {
	if (val) {
		flags |= flag;
	} else {
		flags ^= flags & flag;
	}
}

int getCPUMaxUValue() { return ((isFlagSet(FLAG_8BitsMode)) ? UINT8_MAX : UINT16_MAX); }

int getCPUMaxSValue() { return ((isFlagSet(FLAG_8BitsMode)) ? INT8_MAX : INT16_MAX); }

int getCPUMinSValue() { return ((isFlagSet(FLAG_8BitsMode)) ? INT8_MIN : INT16_MIN); }

ushort swapEndian(ushort baseValue) {
	uchar* baseBytes = (uchar*)&baseValue;

	ushort tempValue = 0;

	uchar* tempBytes = (uchar*)&tempValue;
	tempBytes[1] = baseBytes[0];
	tempBytes[0] = baseBytes[1];

	return tempValue;
}

ushort readMiniRAM(ushort addr) {
	bool systemEndian = systemEndianness;
	bool endianMode = isFlagSet(FLAG_EndianMode);
	bool byteMode = isFlagSet(FLAG_8BitsMode);

	if (byteMode) {
		fetched = miniRAM[addr];
	} else {
		fetched = *((ushort*)(miniRAM + addr));
		if (endianMode != systemEndian) {
			fetched = swapEndian(fetched);
		}
	}

	return fetched;
}

ushort readCodeRAM(bool readOneByte) {
	if (readOneByte) {
		fetched = codeRAM[codePtr++];
	} else {
		fetched = *(ushort*)(codeRAM + codePtr);
		codePtr += 2;
	}
	return fetched;
}

void pushStack(ushort val, bool checkByteMode) {
	bool byteMode = isFlagSet(FLAG_8BitsMode);

	setFlag(FLAG_8BitsMode, checkByteMode && byteMode);

	writeBytes(miniRAM + stackPtr, val);

	setFlag(FLAG_8BitsMode, byteMode);

	stackPtr += 2 - (checkByteMode && byteMode);
}

ushort popStack(bool checkByteMode) {
	bool byteMode = isFlagSet(FLAG_8BitsMode);
	setFlag(FLAG_8BitsMode, checkByteMode && byteMode);

	stackPtr -= 2 - (checkByteMode && byteMode);
	ushort stackValue = readMiniRAM(stackPtr);

	setFlag(FLAG_8BitsMode, byteMode);

	return stackValue;
}

bool isReg(void* addr) { return (addr == &reg_x) || (addr == &reg_y) || (addr == &reg_z) || (addr == &stackPtr); }

void writeBytes(void* dest, ushort val) {
	bool endianMode = isFlagSet(FLAG_EndianMode);
	bool byteMode = isFlagSet(FLAG_8BitsMode);

	if ((endianMode != systemEndianness) && !isReg(dest)) {
		val = swapEndian(val);
	}

	if (byteMode) {
		*((uchar*)dest) = val & 0xFF;
	} else {
		*((ushort*)dest) = val;
	}
}

void* modeAsDest(uchar mode) {
	void* destAddr = NULL;

	switch (mode) {
		default:
		case MODE_REG_X: {
			destAddr = &reg_x;
		} break;
		case MODE_REG_Y: {
			destAddr = &reg_y;
		} break;
		case MODE_REG_Z: {
			destAddr = &reg_z;
		} break;
		case MODE_SPTR: {
			destAddr = &stackPtr;
		} break;

		case MODE_RAM_REG_X: {
			destAddr = miniRAM + reg_x;
		} break;
		case MODE_RAM_REG_Y: {
			destAddr = miniRAM + reg_y;
		} break;
		case MODE_RAM_REG_Z: {
			destAddr = miniRAM + reg_z;
		} break;
		case MODE_RAM_SPTR: {
			destAddr = miniRAM + stackPtr;
		} break;

		case MODE_VALUE:
		case MODE_RAM_VALUE: {
			destAddr = miniRAM + readCodeRAM(false);
		} break;
	}

	return destAddr;
}

ushort modeAsSrc(uchar mode) {
	ushort srcValue = 0;

	switch (mode) {
		default:
		case MODE_REG_X: {
			srcValue = reg_x;
		} break;
		case MODE_REG_Y: {
			srcValue = reg_y;
		} break;
		case MODE_REG_Z: {
			srcValue = reg_z;
		} break;
		case MODE_SPTR: {
			srcValue = stackPtr;
		} break;
		case MODE_VALUE: {
			srcValue = readCodeRAM(false);
		} break;

		case MODE_RAM_REG_X: {
			srcValue = readMiniRAM(reg_x);
		} break;
		case MODE_RAM_REG_Y: {
			srcValue = readMiniRAM(reg_y);
		} break;
		case MODE_RAM_REG_Z: {
			srcValue = readMiniRAM(reg_z);
		} break;
		case MODE_RAM_SPTR: {
			srcValue = readMiniRAM(stackPtr);
		} break;
		case MODE_RAM_VALUE: {
			srcValue = readMiniRAM(readCodeRAM(false));
		} break;
	}

	return srcValue;
}

ushort* modeAsReg(uchar mode) {
	switch (mode) {
		default:
		case 0b0000:
			return &reg_x;
			break;

		case 0b0001:
			return &reg_y;
			break;

		case 0b0010:
			return &reg_z;
			break;

		case 0b0011:
			return &stackPtr;
			break;

		case 0b0100:
			return &flags;
			break;
	}
}
