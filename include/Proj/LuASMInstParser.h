#ifndef LU_ASM_INST_PARSER_H
#define LU_ASM_INST_PARSER_H

#include "LuASMCommon.h"

typedef struct InstLine {
	/*OpCode*/ uchar opCode;

	/*InstArgMode*/ uchar modes[3];
	uchar modeCount;

	ushort argValues[3];
	uchar valuesCount;
	bool oneByteValues;
} InstLine;

void initOpCodeInfos();
void assembleParser(const string path);
InstLine ConstructInstLine(const string lineStr, uint lineNum);
string* CutInstLineParts(const string lineStr, uint lineNum);
bool ParseInstArg(InstLine* instLine, const string arg, uint lineNum, uint argNum);

#endif	// LU_ASM_INST_PARSER_H