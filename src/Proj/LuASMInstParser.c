#include "LuASMInstParser.h"

typedef struct OpCodeInfo {
	OpCode opCode;
	InstArgTreament treatment;
	ConditionType condType;
} OpCodeInfo;

static HashMap opCodeInfos;

void initOpCodeInfos() {
	opCodeInfos = hm_Create(sizeof(OpCodeInfo), false);

	OpCodeInfo info;
	info.condType = COND_None;

	info.treatment = NO_ARGS;
	{
		info.opCode = OP_NOP;
		hm_Push(&opCodeInfos, "NOP", &info);
		info.opCode = OP_RET;
		hm_Push(&opCodeInfos, "RET", &info);
		info.opCode = OP_RET;
		hm_Push(&opCodeInfos, "RETURN", &info);
	}

	info.treatment = MAYBE_ONE_ARG;
	{
		info.opCode = OP_WAIT;
		hm_Push(&opCodeInfos, "WAIT", &info);
	}

	info.treatment = DEST_SRC;
	{
		info.opCode = OP_STR;
		hm_Push(&opCodeInfos, "STR", &info);
		info.opCode = OP_LSL;
		hm_Push(&opCodeInfos, "LSL", &info);
		info.opCode = OP_LSR;
		hm_Push(&opCodeInfos, "LSR", &info);
		info.opCode = OP_ASL;
		hm_Push(&opCodeInfos, "ASL", &info);
		info.opCode = OP_ASR;
		hm_Push(&opCodeInfos, "ASR", &info);
		info.opCode = OP_BSL;
		hm_Push(&opCodeInfos, "BSL", &info);
		info.opCode = OP_BSR;
		hm_Push(&opCodeInfos, "BSR", &info);
	}

	info.treatment = ONEBYTE_VALUE;
	{
		info.opCode = OP_SCF;
		hm_Push(&opCodeInfos, "SCF", &info);
	}

	info.treatment = DEST;
	{
		info.opCode = OP_STZ;
		hm_Push(&opCodeInfos, "STZ", &info);
		info.opCode = OP_INC;
		hm_Push(&opCodeInfos, "INC", &info);
		info.opCode = OP_DEC;
		hm_Push(&opCodeInfos, "DEC", &info);
		info.opCode = OP_NOT;
		hm_Push(&opCodeInfos, "NOT", &info);
	}

	info.treatment = DEST_OPA_OPB;
	{
		info.opCode = OP_AND;
		hm_Push(&opCodeInfos, "AND", &info);
		info.opCode = OP_NAND;
		hm_Push(&opCodeInfos, "NAND", &info);
		info.opCode = OP_OR;
		hm_Push(&opCodeInfos, "OR", &info);
		info.opCode = OP_NOR;
		hm_Push(&opCodeInfos, "NOR", &info);
		info.opCode = OP_XOR;
		hm_Push(&opCodeInfos, "XOR", &info);
		info.opCode = OP_XNOR;
		hm_Push(&opCodeInfos, "XNOR", &info);
		info.opCode = OP_ADD;
		hm_Push(&opCodeInfos, "ADD", &info);
		info.opCode = OP_ADC;
		hm_Push(&opCodeInfos, "ADC", &info);
		info.opCode = OP_SUB;
		hm_Push(&opCodeInfos, "SUB", &info);
		info.opCode = OP_SBC;
		hm_Push(&opCodeInfos, "SBC", &info);
		info.opCode = OP_MUL;
		hm_Push(&opCodeInfos, "MUL", &info);
		info.opCode = OP_DIV;
		hm_Push(&opCodeInfos, "DIV", &info);
		info.opCode = OP_MOD;
		hm_Push(&opCodeInfos, "MOD", &info);
	}

	info.treatment = OPA_OPB;
	{
		info.opCode = OP_CMP;
		hm_Push(&opCodeInfos, "CMP", &info);
	}

	info.treatment = ADDR_ABS_REL;
	{
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BR", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMP", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTO", &info);

		info.condType = COND_Zero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOZ", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOE", &info);

		info.condType = COND_NotZero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTONZ", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRNE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPNE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTONE", &info);

		info.condType = COND_Carry;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRC", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPC", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOC", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRLT", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPLT", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOLT", &info);

		info.condType = COND_CarryZero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRCZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPCZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOCZ", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRLTE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPLTE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOLTE", &info);

		info.condType = COND_NotCarryNotZero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRNCNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPNCNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTONCNZ", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRGT", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPGT", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOGT", &info);

		info.condType = COND_NotCarryZero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRNCZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPNCZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTONCZ", &info);

		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRGTE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPGTE", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOGTE", &info);

		info.condType = COND_NotCarry;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRNC", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPNC", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTONC", &info);

		info.condType = COND_CarryNotZero;
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "BRCNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "JMPCNZ", &info);
		info.opCode = OP_BADOP;
		hm_Push(&opCodeInfos, "GOTOCNZ", &info);
	}

	info.condType = COND_None;

	info.treatment = ADDR_ABS;
	{
		info.opCode = OP_CALL;
		hm_Push(&opCodeInfos, "CALL", &info);
	}

	info.treatment = REGISTER;
	{
		info.opCode = OP_PUSH;
		hm_Push(&opCodeInfos, "PUSH", &info);
		info.opCode = OP_POP;
		hm_Push(&opCodeInfos, "POP", &info);
	}
}

OpCodeInfo getOpCodeInfo(const string instName) { return *((OpCodeInfo*)hm_Get(&opCodeInfos, instName)); }

static InstLine GetEmptyInstLine() {
	InstLine instLine;
	instLine.opCode = OP_BADOP;
	instLine.modeCount = 0;
	instLine.valuesCount = 0;
	instLine.oneByteValues = false;
	return instLine;
}

string* CutInstLineParts(const string lineStr, uint lineNum) {
	if (!lineStr) {
		printf("Parsing Error : line passed is NULL #%d\n", lineNum);
		return NULL;
	}

	bool charMode = false;
	bool stringMode = false;
	bool backslashMode = false;

	uint subCount = 1;
	string* subStrings = calloc(subCount, sizeof(string));

	for (uint i = 0; lineStr[i]; i++) {
		if (lineStr[i] == ' ' || lineStr[i] == '\t' || lineStr[i] == '\n' || lineStr[i] == ',') {
			continue;
		}

		uint j = i + 1;
		while (lineStr[j]) {
			bool done = false;
			switch (lineStr[j]) {
				case ' ':
				case ',':
					if (!(charMode || stringMode)) done = true;
					break;

				case '\t':
				case '\n':
					if (charMode || stringMode) {
						printf("Syntax Error : can't have a tab nor a newline inside a string/character\n");
					}
					break;

				case '\\':
					backslashMode = !backslashMode;
					break;

				default:
					if ((lineStr[j] == '\"') && !backslashMode) stringMode = !stringMode;
					if ((lineStr[j] == '\'') && !backslashMode) charMode = !charMode;
					backslashMode = false;
					break;
			}
			if (done) break;

			if (charMode && stringMode) {
				printf("Syntax Error : entering both character and string mode at the same time\n");
			}

			j++;
		}

		uint subStrLen = j - i;

		subStrings = realloc(subStrings, (subCount + 1) * sizeof(string));
		subStrings[subCount - 1] = calloc(subStrLen + 1, sizeof(char));
		memcpy(subStrings[subCount - 1], lineStr + i, subStrLen);
		//("took a cut at %d of length %d : \"%s\"\n", i, subStrLen, subStrings[subCount - 1]);

		subStrings[subCount++] = NULL;

		i = j - 1;
	}

	return subStrings;
}

bool ParseArgValue(InstLine* instLine, const string arg, uint lineNum, uint argNum) {
	if (!arg) {
		printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
		printf("arg passed is NULL\n");
		return false;
	}

	uint argLen = strlen(arg);

	if (!argLen) {
		printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
		printf("arg passed is empty\n");
		return false;
	}

	if (str_InsensitiveEqualsRange(arg, "0b", 2) && (argLen > 2)) {
		instLine->argValues[instLine->valuesCount++] = str_StrToUInt(arg + 2, 2, UINT16_MAX);
		instLine->modes[instLine->modeCount++] = MODE_VALUE;
		return true;
	} else if (str_InsensitiveEqualsRange(arg, "0x", 2) && (argLen > 2)) {
		instLine->argValues[instLine->valuesCount++] = str_StrToUInt(arg + 2, 16, UINT16_MAX);
		instLine->modes[instLine->modeCount++] = MODE_VALUE;
		return true;
	} else if (str_IsDigit(arg[0])) {
		instLine->argValues[instLine->valuesCount++] = str_StrToUInt(arg, 10, UINT16_MAX);
		instLine->modes[instLine->modeCount++] = MODE_VALUE;
		return true;
	} else if ((arg[0] == '+') || (arg[0] == '-') && (argLen > 1)) {
		if (str_IsDigit(arg[1])) {
			ulong maxValue = UINT16_MAX;
			if (arg[0] == '-') maxValue = INT16_MAX + 1;
			instLine->argValues[instLine->valuesCount++] = str_StrToUInt(arg + 1, 10, maxValue);

			if (arg[0] == '-') {
				int16_t temp = instLine->argValues[instLine->valuesCount - 1];
				temp *= -1;
				instLine->argValues[instLine->valuesCount - 1] = temp;
			}
			instLine->modes[instLine->modeCount++] = MODE_VALUE;
			return true;
		} else {
			printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
			printf("couldn't understand the %c sign\n", arg[0]);
			return false;
		}
	} else if (arg[0] == '\'') {
		uint charModeStart = str_IndexOf(arg, '\'');
		uint charModeEnd = str_LastIndexOf(arg, '\'');

		if (argLen != (3 + (arg[1] == '\\'))) {
			// if (charModeEnd - (charModeStart + 1) != (2 + arg[1] == '\\')) {
			printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
			printf("multi-character expression aren't supported, if you meant a string please use \"\n");
			return false;
		}

		if (arg[1] == '\\') {
			switch (arg[2]) {
				case 't':
					instLine->argValues[instLine->valuesCount++] = '\t';
					break;
				case 'n':
					instLine->argValues[instLine->valuesCount++] = '\n';
					break;
				case '\"':
					instLine->argValues[instLine->valuesCount++] = '\"';
					break;
				case '\'':
					instLine->argValues[instLine->valuesCount++] = '\'';
					break;
				case '\\':
					instLine->argValues[instLine->valuesCount++] = '\\';
					break;
				default:
					printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
					printf("the backslash character \\%c is unknown or unsupported\n", arg[2]);
					return false;
			}
		} else {
			instLine->argValues[instLine->valuesCount++] = arg[1];
		}
		instLine->modes[instLine->modeCount++] = MODE_VALUE;
		return true;
	} else if (argLen == 1) {
		switch (str_ToUpperChar(arg[0])) {
			case 'X':
				instLine->modes[instLine->modeCount++] = MODE_REG_X;
				break;
			case 'Y':
				instLine->modes[instLine->modeCount++] = MODE_REG_Y;
				break;
			case 'Z':
				instLine->modes[instLine->modeCount++] = MODE_REG_Z;
				break;
			case 'S':
				instLine->modes[instLine->modeCount++] = MODE_SPTR;
				break;

			default:
				printf("Syntax Error : illegal symbol as arg, defaulting to reg X\n");
				return false;
		}
		return true;
	} else if (str_InsensitiveEquals(arg, "stackPtr") || str_InsensitiveEquals(arg, "sPtr")) {
		instLine->modes[instLine->modeCount++] = MODE_SPTR;
		return true;
	}

	printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
	printf("couldn't parse the value of \"%s\"\n", arg);
	return false;
}

bool ParseInstArg(InstLine* instLine, const string arg, uint lineNum, uint argNum) {
	if (!arg) {
		printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
		printf("arg passed is NULL\n");
		return false;
	}

	uint argLen = strlen(arg);

	if (!argLen) {
		printf("Parsing Error at line %d arg %d : ", lineNum, argNum);
		printf("arg passed is empty\n");
		return false;
	}

	if (str_InsensitiveEqualsRange(arg, "ram", 3) || (arg[0] == '[')) {
		uint indexingStart = str_IndexOf(arg, '[');
		uint indexingEnd = str_LastIndexOf(arg, ']');
		if ((indexingStart == 0 || indexingStart == 3) && (indexingEnd == (argLen - 1))) {
			bool done = false;
			indexingStart++;
			while (!done) {
				done = true;
				if (str_IsSpace(arg[indexingStart])) {
					indexingStart++;
					done = false;
				}

				if (str_IsSpace(arg[indexingEnd])) {
					indexingEnd--;
					done = false;
				}
			}
			indexingEnd;

			uint indexingLen = indexingEnd - indexingStart;
			char indexPart[indexingLen + 1];
			memcpy(indexPart, arg + indexingStart, indexingLen);
			indexPart[indexingLen] = '\0';

			if (!ParseArgValue(instLine, indexPart, lineNum, argNum)) {
				// Unknown Parsing Error, probs caused by the poor algo
				// Needs to switch to a tokenised version asap
				return false;
			}
			instLine->modes[instLine->modeCount - 1] |= MODE_RAM_REG_X;
			return true;
		}
	}

	return ParseArgValue(instLine, arg, lineNum, argNum);
}

void CleanInstParts(string** parts) {
	for (uint i = 0; (*parts)[i]; i++) {
		free((*parts)[i]);
	}
	free((*parts));
	*parts = NULL;
}

InstLine ConstructInstLine(const string lineStr, uint lineNum) {
	InstLine instLine = GetEmptyInstLine();

	// printf("\n");
	string* parts = CutInstLineParts(lineStr, lineNum);

	uint partCount = 0;
	while (parts[partCount]) {
		partCount++;
	}

	if (!partCount) {
		printf("Line %d doesn't contain parts\n", lineNum);
		CleanInstParts(&parts);
		return GetEmptyInstLine();
	}

	str_ToUpper(parts[0]);

	OpCodeInfo info = getOpCodeInfo(parts[0]);
	instLine.opCode = info.opCode;

	uint expectedPartCount = 0;

	switch (info.treatment) {
		case NO_ARGS:
			expectedPartCount = 1;
			break;

		case MAYBE_ONE_ARG:
			expectedPartCount = 2;
			if (partCount == 1) {
				expectedPartCount = 1;
				instLine.opCode = OP_NOP;
			}
			break;

		case ONEBYTE_VALUE:
			instLine.oneByteValues = true;
			__attribute__((fallthrough));
		case DEST:
		case ADDR_ABS_REL:
		case ADDR_ABS:
		case REGISTER:
			expectedPartCount = 2;
			break;

		case DEST_SRC:
		case OPA_OPB:
			expectedPartCount = 3;
			break;

		case DEST_OPA_OPB:
			expectedPartCount = 4;
			break;
	}

	if (partCount < expectedPartCount) {
		printf("Line #%d is missing parts\n", lineNum);
		CleanInstParts(&parts);
		return GetEmptyInstLine();
	} else if (partCount > expectedPartCount) {
		printf("Line #%d has too many parts\n", lineNum);
		CleanInstParts(&parts);
		return GetEmptyInstLine();
	}

	for (uint i = 1; i < partCount; i++) {
		if (!ParseInstArg(&instLine, parts[i], lineNum, i)) {
			CleanInstParts(&parts);
			return GetEmptyInstLine();
		}
	}

	if ((info.treatment == DEST_SRC) || (info.treatment == DEST) || (info.treatment == DEST_OPA_OPB)) {
		if (instLine.modes[0] == MODE_VALUE) {
			printf("Value used as \"dest\" part at line %d\n", lineNum);
			printf("\t(maybe you meant \"RAM[{value}]\")\n");
			CleanInstParts(&parts);
			return GetEmptyInstLine();
		}
	} else if ((info.treatment == ADDR_ABS_REL) || (info.treatment == ADDR_ABS)) {
		if (instLine.modes[0] != MODE_VALUE) {
			printf("Attempt to jump on a non-constant expression at line %d\n", lineNum);
			CleanInstParts(&parts);
			return GetEmptyInstLine();
		}

		if (info.condType != COND_None) instLine.modes[0] = info.condType;

		ushort jmpAddr = instLine.argValues[0];
		ushort tempCodePtr = codePtr + 1 + (info.condType != COND_None) + 1;

		int diff = jmpAddr;
		diff -= tempCodePtr;

		if ((diff <= INT8_MAX) && (diff >= INT8_MIN)) {
			instLine.oneByteValues = true;
			instLine.argValues[0] = diff;
			if (info.condType == COND_None) {
				instLine.opCode = OP_JMPR;
			} else {
				instLine.opCode = OP_JMPRC;
			}
		} else {
			instLine.argValues[0] = jmpAddr;
			if (info.condType == COND_None) {
				instLine.opCode = OP_JMPA;
			} else {
				instLine.opCode = OP_JMPAC;
			}
		}
	} else if (info.treatment == ONEBYTE_VALUE) {
		instLine.modeCount = 0;
	}

	CleanInstParts(&parts);
	return instLine;
}

void assembleParser(const string path) {
	// WIP
	string* lines = io_LoadStrings(path);

	bool failedParsing = false;
	for (uint i = 0; lines[i]; i++) {
		InstLine instLine = ConstructInstLine(lines[i], i);
		if (instLine.opCode == OP_BADOP) {
			failedParsing = true;
			break;
		}

		codeRAM[codePtr++] = instLine.opCode;

		if (instLine.modeCount == 1) {
			codeRAM[codePtr++] = instLine.modes[0];
		} else if (instLine.modeCount == 2) {
			codeRAM[codePtr++] = combineModeLowHigh(instLine.modes[1], instLine.modes[0]);
		} else if (instLine.modeCount == 3) {
			codeRAM[codePtr++] = combineModeLowHigh(instLine.modes[1], instLine.modes[0]);
			codeRAM[codePtr++] = instLine.modes[2];
		}

		for (uint j = 0; j < instLine.valuesCount; j++) {
			if (instLine.oneByteValues) {
				codeRAM[codePtr++] = instLine.argValues[j];
			} else {
				*(ushort*)(codeRAM + codePtr) = instLine.argValues[j];
				codePtr += 2;
			}
		}
	}

	printf("%s compiled to a %d bytes program\n", path, codePtr);

	codePtr = 0;
}