#include "LuASMAssembler.h"

static HashMap argTreatments;

void initArgTreatments() {
	argTreatments = hm_Create(sizeof(InstArgTreament), false);

	InstArgTreament treatment;

	treatment = NO_ARGS;
	{
		hm_Push(&argTreatments, "NOP", &treatment);
		hm_Push(&argTreatments, "RET", &treatment);
		hm_Push(&argTreatments, "RETURN", &treatment);
	}

	treatment = MAYBE_ONE_ARG;
	{
		// Place Holder
		hm_Push(&argTreatments, "WAIT", &treatment);
	}

	treatment = DEST_SRC;
	{
		// Place Holder
		hm_Push(&argTreatments, "STR", &treatment);
	}

	treatment = ONEBYTE_VALUE;
	{
		// Place Holder
		hm_Push(&argTreatments, "SCF", &treatment);
	}

	treatment = DEST;
	{
		hm_Push(&argTreatments, "STZ", &treatment);
		hm_Push(&argTreatments, "INC", &treatment);
		hm_Push(&argTreatments, "DEC", &treatment);
		hm_Push(&argTreatments, "NOT", &treatment);
	}

	treatment = DEST_OPA_OPB;
	{
		hm_Push(&argTreatments, "AND", &treatment);
		hm_Push(&argTreatments, "NAND", &treatment);
		hm_Push(&argTreatments, "OR", &treatment);
		hm_Push(&argTreatments, "NOR", &treatment);
		hm_Push(&argTreatments, "XOR", &treatment);
		hm_Push(&argTreatments, "XNOR", &treatment);
		hm_Push(&argTreatments, "ADD", &treatment);
		hm_Push(&argTreatments, "ADC", &treatment);
		hm_Push(&argTreatments, "SUB", &treatment);
		hm_Push(&argTreatments, "SBC", &treatment);
		hm_Push(&argTreatments, "MUL", &treatment);
		hm_Push(&argTreatments, "DIV", &treatment);
		hm_Push(&argTreatments, "MOD", &treatment);
	}

	treatment = OPA_OPB;
	{
		// Place Holder
		hm_Push(&argTreatments, "CMP", &treatment);
	}

	treatment = ADDR_ABS_REL;
	{
		hm_Push(&argTreatments, "BR", &treatment);
		hm_Push(&argTreatments, "JMP", &treatment);
		hm_Push(&argTreatments, "GOTO", &treatment);

		hm_Push(&argTreatments, "BRZ", &treatment);
		hm_Push(&argTreatments, "JMPZ", &treatment);
		hm_Push(&argTreatments, "GOTOZ", &treatment);

		hm_Push(&argTreatments, "BRE", &treatment);
		hm_Push(&argTreatments, "JMPE", &treatment);
		hm_Push(&argTreatments, "GOTOE", &treatment);

		hm_Push(&argTreatments, "BRNZ", &treatment);
		hm_Push(&argTreatments, "JMPNZ", &treatment);
		hm_Push(&argTreatments, "GOTONZ", &treatment);

		hm_Push(&argTreatments, "BRNE", &treatment);
		hm_Push(&argTreatments, "JMPNE", &treatment);
		hm_Push(&argTreatments, "GOTONE", &treatment);

		hm_Push(&argTreatments, "BRC", &treatment);
		hm_Push(&argTreatments, "JMPC", &treatment);
		hm_Push(&argTreatments, "GOTOC", &treatment);

		hm_Push(&argTreatments, "BRLT", &treatment);
		hm_Push(&argTreatments, "JMPLT", &treatment);
		hm_Push(&argTreatments, "GOTOLT", &treatment);

		hm_Push(&argTreatments, "BRCZ", &treatment);
		hm_Push(&argTreatments, "JMPCZ", &treatment);
		hm_Push(&argTreatments, "GOTOCZ", &treatment);

		hm_Push(&argTreatments, "BRLTE", &treatment);
		hm_Push(&argTreatments, "JMPLTE", &treatment);
		hm_Push(&argTreatments, "GOTOLTE", &treatment);

		hm_Push(&argTreatments, "BRNCNZ", &treatment);
		hm_Push(&argTreatments, "JMPNCNZ", &treatment);
		hm_Push(&argTreatments, "GOTONCNZ", &treatment);

		hm_Push(&argTreatments, "BRGT", &treatment);
		hm_Push(&argTreatments, "JMPGT", &treatment);
		hm_Push(&argTreatments, "GOTOGT", &treatment);

		hm_Push(&argTreatments, "BRNCZ", &treatment);
		hm_Push(&argTreatments, "JMPNCZ", &treatment);
		hm_Push(&argTreatments, "GOTONCZ", &treatment);

		hm_Push(&argTreatments, "BRGTE", &treatment);
		hm_Push(&argTreatments, "JMPGTE", &treatment);
		hm_Push(&argTreatments, "GOTOGTE", &treatment);

		hm_Push(&argTreatments, "BRNC", &treatment);
		hm_Push(&argTreatments, "JMPNC", &treatment);
		hm_Push(&argTreatments, "GOTONC", &treatment);

		hm_Push(&argTreatments, "BRCNZ", &treatment);
		hm_Push(&argTreatments, "JMPCNZ", &treatment);
		hm_Push(&argTreatments, "GOTOCNZ", &treatment);
	}

	treatment = ADDR_ABS;
	{
		// Place Holder
		hm_Push(&argTreatments, "CALL", &treatment);
	}

	treatment = REGISTER;
	{
		hm_Push(&argTreatments, "PUSH", &treatment);
		hm_Push(&argTreatments, "POP", &treatment);
	}
}

InstArgTreament getArgTreatment(const string instName) { return *((InstArgTreament*)hm_Get(&argTreatments, instName)); }

string* cutLine(const string str) {
	bool charMode = false;
	bool stringMode = false;
	bool backslashMode = false;

	uint subCount = 1;
	string* subStrings = calloc(subCount, sizeof(string));

	for (uint i = 0; str[i]; i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == ',') {
			continue;
		}
		uint j = i + 1;
		while (str[j]) {
			if (str[j] == ' ' || str[j] == ',') {
				if (!charMode && !stringMode) break;
			}

			if (str[j] == '\t' || str[j] == '\n') {
				if (charMode || stringMode) {
					printf("Syntax Error : can't have a tab nor a newline inside a string/character\n");
				}
			}

			if (str[j] == '\\') {
				backslashMode = !backslashMode;
			} else {
				if (str[j] == '\"' && !backslashMode) stringMode = !stringMode;
				if (str[j] == '\'' && !backslashMode) charMode = !charMode;
				backslashMode = false;
			}

			if (charMode && stringMode) {
				printf("Syntax Error : entering both character and string mode at the same time\n");
			}

			j++;
		}
		// printf("took a cut at %d of length %d\n", i, j - i);

		subStrings = realloc(subStrings, (subCount + 1) * sizeof(string));
		subStrings[subCount] = NULL;
		subStrings[subCount - 1] = calloc(j - i + 1, sizeof(char));
		memcpy(subStrings[subCount - 1], str + i, j - i);
		subCount++;

		i = j;
	}

	return subStrings;
}

InstArgMode getArgMode(const string arg) {
	uint argLen = strlen(arg);

	if (argLen == 1) {
		if (arg[0] < '0' || arg[0] >= '9') {
			switch (arg[0]) {
				case 'x':
				case 'X':
					return MODE_REG_X;
					break;

				case 'y':
				case 'Y':
					return MODE_REG_Y;
					break;

				case 'z':
				case 'Z':
					return MODE_REG_Z;
					break;

				default:
					printf("Syntax Error : illegal symbol as arg, defaulting to reg X\n");
					return MODE_REG_X;
					break;
			}
		}
	}

	if ((strncmp(arg, "0x", 2) == 0) || (strncmp(arg, "0b", 2) == 0) || (strncmp(arg, "\'", 1) == 0) ||
		((arg[0] >= '0') && (arg[0] <= '9'))) {
		return MODE_VALUE;
	}

	if (!str_Contains(arg, '[')) {
		printf("Syntax Error : couldn't understand arg, defaulting to reg X\n");
		return MODE_REG_X;
	}

	if (arg[argLen - 1] != ']') {
		printf("Syntax Error : missing matching square bracket\n");
		return MODE_REG_X;
	}

	bool foundBrackets = false;

	uint i = 0;
	while (i < argLen) {
		char charAtI = arg[i];
		if ((charAtI >= 'a') && (charAtI <= 'z')) {
			charAtI += 'A' - 'a';
		}

		if (charAtI == 'R' || charAtI == 'A' || charAtI == 'M') {
			i++;
		} else if (charAtI == '[') {
			foundBrackets = true;
			i++;
		} else {
			if (!foundBrackets) {
				printf("Syntax Error : couldn't undersand RAM addr, default to reg X as the addr\n");
				return MODE_RAM_REG_X;
			}
			if (charAtI == ' ' || charAtI == '\t') {
				i++;
			} else {
				break;
			}
		}
	}

	foundBrackets = false;
	uint j = argLen - 2;
	while (!foundBrackets && j < argLen) {
		if (arg[j] == ' ' || arg[j] == '\t') {
			j--;
		} else {
			foundBrackets = true;
		}
	}

	uint subArgLen = j - i + 1;
	const string subArg = arg + i;

	if (subArgLen == 1) {
		switch (subArg[0]) {
			case 'x':
			case 'X':
				return MODE_RAM_REG_X;
				break;

			case 'y':
			case 'Y':
				return MODE_RAM_REG_Y;
				break;

			case 'z':
			case 'Z':
				return MODE_RAM_REG_Z;
				break;

			default:
				printf("Syntax Error : illegal symbol as RAM addr, defaulting to reg X as the addr\n");
				return MODE_RAM_REG_X;
				break;
		}
	}

	if ((strncmp(subArg, "0x", 2) == 0) || (strncmp(subArg, "0b", 2) == 0) || (strncmp(subArg, "\'", 1) == 0) ||
		((subArg[0] >= '0') && (subArg[0] <= '9'))) {
		return MODE_RAM_VALUE;
	}

	printf("Syntax Error : unknown error when determining the arg type, defaulting to reg X\n");
	return MODE_REG_X;
}

ushort strToUShort(const string numStr, uint numStrLen, uint base) {
	uint numValue = 0;
	for (uint i = 0; numStr[i]; i++) {
		if (numStrLen) {
			if (i >= numStrLen) break;
		}

		char charAtI = numStr[i];
		if ((charAtI >= 'a') && (charAtI <= 'z')) {
			charAtI += 'A' - 'a';
		}

		if (charAtI == '\'') continue;

		uint charValue = 0;

		if ((charAtI >= '0') && (charAtI <= '9')) {
			charValue = charAtI - '0';
		} else if ((charAtI >= 'a') && (charAtI <= 'z')) {
			charValue = 10 + charAtI - 'A';
		} else {
			printf("Syntax Error : illegal character in numerical expression\n");
			return 0;
		}

		if (charValue >= base) {
			printf("Syntax Error : character outside of the base's range in numerical expression\n");
			return 0;
		}
		numValue *= base;
		numValue += charValue;

		if (numValue > UINT16_MAX) {
			printf("Syntax Error : numerical expression evaluates to a larger number than UINT16_MAX\n");
			return UINT16_MAX;
		}
	}
	return numValue;
}

ushort readArgValue(const string arg) {
	uint argLen = strlen(arg);

	if ((arg[0] >= '0') && (arg[0] <= '9')) {
		return strToUShort(arg, 0, 10);
	}

	if (!strncmp(arg, "0b", 2)) {
		return strToUShort(arg + 2, 0, 2);
	}

	if (!strncmp(arg, "0x", 2)) {
		return strToUShort(arg + 2, 0, 16);
	}

	if (!strncmp(arg, "\'", 1)) {
		if (argLen != (3 + (arg[1] == '\\'))) {
			printf("Syntax Error : multi-character expression aren't supported, maybe you wanted a string ?\n");
			return 0;
		}

		if (arg[argLen - 1] != '\'') {
			printf("Syntax Error : missing matching end of character quote \'\n");
			return 0;
		}

		if (arg[1] == '\\') {
			switch (arg[2]) {
				case 't':
					return '\t';
				case 'n':
					return '\n';
				case '\"':
					return '\"';
				case '\'':
					return '\'';
				case '\\':
					return '\\';
				default:
					printf("Syntax Error : the backslash character \\%c is unknown or unsupported\n", arg[2]);
					return 0;
			}
		} else {
			if ((arg[1] == '\'') || (arg[1] == '\"')) {
				printf("Syntax Error : missing backslash character before a non-ending quote\n");
			}
			return arg[1];
		}
	}

	bool foundBrackets = false;

	uint i = 0;
	while (i < argLen) {
		char charAtI = arg[i];
		if ((charAtI >= 'a') && (charAtI <= 'z')) {
			charAtI += 'A' - 'a';
		}

		if (charAtI == 'R' || charAtI == 'A' || charAtI == 'M') {
			i++;
		} else if (charAtI == '[') {
			foundBrackets = true;
			i++;
		} else {
			if (!foundBrackets) {
				printf("Syntax Error : couldn't undersand RAM addr, default to reg X as the addr\n");
				return MODE_RAM_REG_X;
			}
			if (charAtI == ' ' || charAtI == '\t') {
				i++;
			} else {
				break;
			}
		}
	}

	foundBrackets = false;
	uint j = argLen - 2;
	while (!foundBrackets && j < argLen) {
		if (arg[j] == ' ' || arg[j] == '\t') {
			j--;
		} else {
			foundBrackets = true;
		}
	}

	uint subArgLen = j - i + 1;
	const string subArg = arg + i;

	if ((subArg[0] >= '0') && (subArg[0] <= '9')) {
		return strToUShort(subArg, 0, 10);
	}

	if (strncmp(subArg, "0b", 2) == 0) {
		return strToUShort(subArg + 2, 0, 2);
	}

	if (strncmp(subArg, "0x", 2) == 0) {
		return strToUShort(subArg + 2, 0, 16);
	}

	if (strncmp(subArg, "\'", 1) == 0) {
		if (subArgLen != (3 + (subArg[1] == '\\'))) {
			printf(
				"Syntax Error : as RAM addr, multi-character expression aren't supported, maybe you wanted a string "
				"?\n");
			return 0;
		}

		if (subArg[subArgLen - 1] != '\'') {
			printf("Syntax Error : as RAM addr, missing matching end of character quote \'\n");
			return 0;
		}

		if (subArg[1] == '\\') {
			switch (subArg[2]) {
				case '\t':
					return '\t';
				case '\n':
					return '\n';
				case '\"':
					return '\"';
				case '\'':
					return '\'';
				case '\\':
					return '\\';
				default:
					printf("Syntax Error : as RAM addr, the backslash character \\%c is unknown or unsupported\n",
						   arg[2]);
					return 0;
			}
		} else {
			if ((subArg[1] == '\'') || (subArg[1] == '\"')) {
				printf("Syntax Error : as RAM addr, missing backslash character before a non-ending quote\n");
			}
			return subArg[1];
		}
	}

	return 0;
}

bool isModeDest(InstArgMode mode) {
	if (!(mode & MODE_VALUE)) return true;
	if (!(mode & MODE_RAM_REG_X)) return false;
	return true;
}

ConditionType getConditionType(const string inst) {
	uint instLen = strlen(inst);
	string cond;

	if (!strncmp(inst, "BR", 2)) {
		cond = inst + 2;
	} else if (!strncmp(inst, "JMP", 3)) {
		cond = inst + 3;
	} else if (!strncmp(inst, "GOTO", 4)) {
		cond = inst + 4;
	} else {
		printf("Syntax Error : unknown error when reading jump instruction\n");
		return COND_None;
	}

	if (!(*cond)) return COND_None;

	if (str_Equals(cond, "Z") || str_Equals(cond, "E")) {
		return COND_Zero;
	} else if (str_Equals(cond, "NZ") || str_Equals(cond, "NE")) {
		return COND_NotZero;
	} else if (str_Equals(cond, "C") || str_Equals(cond, "LT")) {
		return COND_Carry;
	} else if (str_Equals(cond, "CZ") || str_Equals(cond, "LTE")) {
		return COND_CarryZero;
	} else if (str_Equals(cond, "NCNZ") || str_Equals(cond, "GT")) {
		return COND_NotCarryNotZero;
	} else if (str_Equals(cond, "NCZ") || str_Equals(cond, "GTE")) {
		return COND_NotCarryZero;
	} else if (str_Equals(cond, "NC")) {
		return COND_NotCarry;
	} else if (str_Equals(cond, "CNZ")) {
		return COND_CarryNotZero;
	} else {
		printf("Syntax Error : the condition specifier \"%s\" doesn't exist\n", cond);
		return COND_None;
	}
}

void assemble(const string path) {
	// WIP
	codePtr = 0;
	string* lines = io_LoadStrings(path);

	for (uint i = 0; lines[i]; i++) {
		// printf("\n");
		string* parts = cutLine(lines[i]);
		uint partCount = 0;
		while (parts[partCount]) {
			partCount++;
			continue;
		}

		if (!partCount) {
			printf("Line #%d doesn't contain parts\n", i);
			free(parts);
			continue;
		}

		for (uint j = 0; parts[0][j]; j++) {
			if (parts[0][j] >= 'a' && parts[0][j] <= 'z') {
				parts[0][j] += 'A' - 'a';
			}
		}

		OpCode opCode = OP_BADOP;

		uchar modes[2] = {MODE_REG_X};
		uint modeCount = 0;

		ushort values[3] = {0};
		uint valueCount = 0;
		uint valueSize = 2;
		InstArgTreament treatment = getArgTreatment(parts[0]);

		/*printf("line %d has %d parts\n", i, partCount);
		for (uint j = 0; j < partCount; j++) {
			printf("\t[%d] : %s\n", j, parts[j]);
		}
		printf("the treatment for this line is : %d\n", treatment);*/

		switch (treatment) {
			case NO_ARGS: {
				if (partCount > 1) {
					printf("Line #%d has too many parts\n", i);
					break;
				}
				if (parts[0][0] == 'N') opCode = OP_NOP;
				if (parts[0][0] == 'R') opCode = OP_RET;
			} break;

			case MAYBE_ONE_ARG: {
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}
				if (partCount == 1) {
					opCode = OP_NOP;
					break;
				}
				opCode = OP_WAIT;
				modes[modeCount++] = getArgMode(parts[1]);
				if (modes[0] & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[1]);
				}
			} break;

			case DEST_SRC: {
				if (partCount < 3) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 3) {
					printf("Line #%d has too many parts\n", i);
					break;
				}

				InstArgMode modeHigh = getArgMode(parts[1]);
				InstArgMode modeLow = getArgMode(parts[2]);

				if (!isModeDest(modeHigh)) {
					printf("Value used as \"dest\" part at line %d\n", i);
					printf("\t(maybe missing \"RAM[{value}]\")\n");
					break;
				}

				if (modeHigh & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[1]);
				}

				if (modeLow & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[2]);
				}

				modes[modeCount++] = combineModeLowHigh(modeLow, modeHigh);

				opCode = OP_STR;
			} break;

			case ONEBYTE_VALUE: {
				if (partCount < 2) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}
				modes[modeCount++] = getArgMode(parts[1]);
				if (!(modes[0] & MODE_VALUE)) {
					modeCount = 0;
					printf("A constant 1 byte value must be provided at line %d\n", i);
					break;
				}

				values[valueCount++] = readArgValue(parts[1]);
				valueSize = 1;

				opCode = OP_SCF;

			} break;

			case DEST: {
				if (partCount < 2) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}
				modes[modeCount++] = getArgMode(parts[1]);

				if (!isModeDest(modes[0])) {
					modeCount = 0;
					printf("Value used as \"dest\" part at line %d\n", i);
					printf("\t(maybe missing \"RAM[{value}]\")\n");
					break;
				}

				if (modes[0] & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[1]);
				}

				switch (parts[0][0]) {
					case 'S':
						opCode = OP_STZ;
						break;
					case 'I':
						opCode = OP_INC;
						break;
					case 'D':
						opCode = OP_DEC;
						break;
					case 'N':
						opCode = OP_NOT;
						break;
					default:
						opCode = OP_BADOP;
				}

			} break;

			case DEST_OPA_OPB: {
				if (partCount < 4) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 4) {
					printf("Line #%d has too many parts\n", i);
					break;
				}

				InstArgMode modeAHigh = getArgMode(parts[1]);
				InstArgMode modeALow = getArgMode(parts[2]);
				InstArgMode modeBLow = getArgMode(parts[3]);

				if (!isModeDest(modeAHigh)) {
					printf("Value used as \"dest\" part at line %d\n", i);
					printf("\t(maybe missing \"RAM[{value}]\")\n");
					break;
				}

				if (modeAHigh & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[1]);
				}

				if (modeALow & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[2]);
				}

				if (modeBLow & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[3]);
				}

				modes[modeCount++] = combineModeLowHigh(modeALow, modeAHigh);
				modes[modeCount++] = modeBLow;

				if (str_Equals(parts[0], "AND")) {
					opCode = OP_AND;
				} else if (str_Equals(parts[0], "NAND")) {
					opCode = OP_NAND;
				} else if (str_Equals(parts[0], "OR")) {
					opCode = OP_OR;
				} else if (str_Equals(parts[0], "NOR")) {
					opCode = OP_NOR;
				} else if (str_Equals(parts[0], "XOR")) {
					opCode = OP_XOR;
				} else if (str_Equals(parts[0], "XNOR")) {
					opCode = OP_XNOR;
				} else if (str_Equals(parts[0], "ADD")) {
					opCode = OP_ADD;
				} else if (str_Equals(parts[0], "ADC")) {
					opCode = OP_ADC;
				} else if (str_Equals(parts[0], "SUB")) {
					opCode = OP_SUB;
				} else if (str_Equals(parts[0], "SBC")) {
					opCode = OP_SBC;
				} else if (str_Equals(parts[0], "MUL")) {
					opCode = OP_MUL;
				} else if (str_Equals(parts[0], "DIV")) {
					opCode = OP_DIV;
				} else if (str_Equals(parts[0], "MOD")) {
					opCode = OP_MOD;
				}

			} break;

			case OPA_OPB: {
				if (partCount < 3) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 3) {
					printf("Line #%d has too many parts\n", i);
					break;
				}
				InstArgMode modeHigh = getArgMode(parts[1]);
				InstArgMode modeLow = getArgMode(parts[2]);

				if (modeHigh & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[1]);
				}

				if (modeLow & MODE_VALUE) {
					values[valueCount++] = readArgValue(parts[2]);
				}

				modes[modeCount++] = combineModeLowHigh(modeLow, modeHigh);

				opCode = OP_CMP;
			} break;

			case ADDR_ABS_REL: {
				if (partCount < 2) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}

				if (getArgMode(parts[1]) != MODE_VALUE) {
					printf("Attempt to jump on a non-constant expression at line %d\n", i);
					break;
				}

				ConditionType condType = getConditionType(parts[0]);
				if (condType != COND_None) modes[modeCount++] = condType;
				ushort jmpAddr = readArgValue(parts[1]);
				ushort tempCodePtr = codePtr + 1 + (condType != COND_None) + 1;
				int diff = (int)(jmpAddr)-tempCodePtr;
				if (diff <= 127 && diff >= -128) {
					valueSize = 1;
					values[valueCount++] = diff;
					if (condType == COND_None) {
						opCode = OP_JMPR;
					} else {
						opCode = OP_JMPRC;
					}
				} else {
					values[valueCount++] = jmpAddr;
					if (condType == COND_None) {
						opCode = OP_JMPA;
					} else {
						opCode = OP_JMPAC;
					}
				}

			} break;

			case ADDR_ABS: {
				if (partCount < 2) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}

				if (getArgMode(parts[1]) != MODE_VALUE) {
					printf("Attempt to jump on a non-constant expression at line %d\n", i);
					break;
				}

				values[valueCount++] = readArgValue(parts[1]);

				opCode = OP_CALL;
			} break;

			case REGISTER: {
				if (partCount < 2) {
					printf("Line #%d is missing parts\n", i);
					break;
				}
				if (partCount > 2) {
					printf("Line #%d has too many parts\n", i);
					break;
				}

				InstArgMode tempMode = getArgMode(parts[1]);

				if ((tempMode & MODE_RAM_REG_X) || (tempMode & MODE_VALUE)) {
					printf("Attempt to push/pop a non-register expression at line %d\n", i);
					break;
				}

				modes[modeCount++] = tempMode;

				if (parts[0][1] == 'U') {
					opCode = OP_PUSH;
				} else {
					opCode = OP_POP;
				}
			} break;
		}

		codeRAM[codePtr++] = opCode;
		for (uint j = 0; j < modeCount; j++) {
			codeRAM[codePtr++] = modes[j];
		}

		for (uint j = 0; j < valueCount; j++) {
			if (valueSize == 1) {
				codeRAM[codePtr++] = values[j];
			} else {
				*(ushort*)(codeRAM + codePtr) = values[j];
				codePtr += 2;
			}
		}

		for (uint j = 0; j < partCount; j++) {
			free(parts[j]);
		}
		free(parts);
	}

	codePtr = 0;
	for (uint i = 0; lines[i]; i++) {
		free(lines[i]);
	}
	free(lines);
}
