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

ushort readMiniRAM(ushort addr) {
	bool systemEndian = systemEndianness;
	bool endianMode = isFlagSet(FLAG_EndianMode);
	bool byteMode = isFlagSet(FLAG_8BitsMode);

	if (byteMode) {
		fetched = miniRAM[addr];
	} else {
		if (endianMode == systemEndian) {
			fetched = *((ushort*)(miniRAM + addr));
		} else {
			((char*)&fetched)[0] = miniRAM[addr + 1];
			((char*)&fetched)[1] = miniRAM[addr];
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

void writeBytes(void* dest, ushort val) {
	bool systemEndian = systemEndianness;
	bool endianMode = isFlagSet(FLAG_EndianMode);
	bool byteMode = isFlagSet(FLAG_8BitsMode);

	if (byteMode) {
		*((uchar*)dest) = val & 0xFF;
	} else {
		if (endianMode == systemEndian) {
			*((ushort*)dest) = val;
		} else {
			((uchar*)dest)[1] = val & 0x00FF;
			((uchar*)dest)[0] = (val & 0xFF00) / 0x0100;
		}
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

/*ushort modeAsSrc(uchar mode, void** srcAddr) {
	ushort srcValue = 0;

	if (mode == 0b1111) {
		srcValue = readCodeRAM(true);
		if (srcAddr) *srcAddr = NULL;
	} else if (mode & 0b1000) {
		ushort tempAddr = readCodeRAM(true);
		srcValue = readMiniRAM(tempAddr);
		if (srcAddr) *srcAddr = miniRAM + tempAddr;
	} else if (mode & 0b0100) {
		switch (mode & 0b0011) {
			case 0b00:
				srcValue = readMiniRAM(reg_x);
				if (srcAddr) *srcAddr = miniRAM + reg_x;
				break;
			case 0b01:
				srcValue = readMiniRAM(reg_y);
				if (srcAddr) *srcAddr = miniRAM + reg_y;
				break;
			case 0b10:
				srcValue = readMiniRAM(reg_z);
				if (srcAddr) *srcAddr = miniRAM + reg_z;
				break;
			case 0b11:
				srcValue = readMiniRAM(stackPtr);
				if (srcAddr) *srcAddr = miniRAM + stackPtr;
				break;
		}
	} else {
		switch (mode & 0b0011) {
			case 0b00:
				srcValue = reg_x;
				if (srcAddr) *srcAddr = &reg_x;
				break;
			case 0b01:
				srcValue = reg_y;
				if (srcAddr) *srcAddr = &reg_y;
				break;
			case 0b10:
				srcValue = reg_z;
				if (srcAddr) *srcAddr = &reg_z;
				break;
			case 0b11:
				srcValue = stackPtr;
				if (srcAddr) *srcAddr = &stackPtr;
				break;
		}
	}

	return srcValue;
}*/

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
