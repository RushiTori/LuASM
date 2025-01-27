#include "main.h"

void printLuASMStates(FILE* outFile) {
	fprintf(outFile, "Registers :\n");
	fprintf(outFile, "\tX : %d\n", reg_x);
	fprintf(outFile, "\tY : %d\n", reg_y);
	fprintf(outFile, "\tZ : %d\n", reg_z);
	fprintf(outFile, "\tsPtr : %d\n", stackPtr);

	fprintf(outFile, "State Flags :\n");
	fprintf(outFile, "\tZero           : %d\n", isFlagSet(FLAG_Zero));
	fprintf(outFile, "\tCarry          : %d\n", isFlagSet(FLAG_Carry));
	fprintf(outFile, "\tNegative       : %d\n", isFlagSet(FLAG_Negative));
	fprintf(outFile, "\tOverflow       : %d\n", isFlagSet(FLAG_Overflow));
	fprintf(outFile, "\t8BitsMode      : %d\n", isFlagSet(FLAG_8BitsMode));
	fprintf(outFile, "\tEndianMode     : %d\n", isFlagSet(FLAG_EndianMode));
	fprintf(outFile, "\tFrameClockMode : %d\n", isFlagSet(FLAG_FrameClockMode));
}

int main(int argc, string* argv) {
	atexit(report_mem_leak);
	setup("Lu ASM Project", argv, argc);

	resetLuASM();

	assembleTokens("ressources/scripts/LuASMTesting.luasm");
	// initOpCodeInfos();
	// assembleParser("ressources/scripts/LuASMTesting.luasm");
	for (uint i = 0; i < 21; i++) {
		// printf("[%d] : %X\n", i, codeRAM[i]);
	}

	bool interupt = false;
	while (!interupt) {
		if (sleepCycle > 0) {
			sleepCycle--;
			continue;
		}
		uchar opCode = readCodeRAM(true);
		//printf("codeRAM[%d] : %X\n", codePtr - 1, opCode);

		switch (opCode) {
			default:
			case OP_BADOP:
				interupt = true;
				break;

			case OP_NOP:
				NOP();
				break;
			case OP_WAIT:
				WAIT();
				break;
			case OP_STR:
				STR();
				break;
			case OP_STZ:
				STZ();
				break;
			case OP_SCF:
				SCF();
				break;
			case OP_LSL:
				LSL();
				break;
			case OP_LSR:
				LSR();
				break;
			case OP_ASL:
				ASL();
				break;
			case OP_ASR:
				ASR();
				break;
			case OP_BSL:
				BSL();
				break;
			case OP_BSR:
				BSR();
				break;
			case OP_INC:
				INC();
				break;
			case OP_DEC:
				DEC();
				break;
			case OP_AND:
				AND();
				break;
			case OP_NAND:
				NAND();
				break;
			case OP_OR:
				OR();
				break;
			case OP_NOR:
				NOR();
				break;
			case OP_XOR:
				XOR();
				break;
			case OP_XNOR:
				XNOR();
				break;
			case OP_NOT:
				NOT();
				break;
			case OP_ADD:
				ADD();
				break;
			case OP_ADC:
				ADC();
				break;
			case OP_SUB:
				SUB();
				break;
			case OP_SBC:
				SBC();
				break;
			case OP_MUL:
				MUL();
				break;
			case OP_DIV:
				DIV();
				break;
			case OP_MOD:
				MOD();
				break;
			case OP_CMP:
				CMP();
				break;
			case OP_JMPA:
				JMPA();
				break;
			case OP_JMPR:
				JMPR();
				break;
			case OP_JMPAC:
				JMPAC();
				break;
			case OP_JMPRC:
				JMPRC();
				break;
			case OP_CALL:
				CALL();
				break;
			case OP_RET:
				RET();
				break;
			case OP_PUSH:
				PUSH();
				break;
			case OP_POP:
				POP();
				break;
		}
	}

	printf("\nProgram closed successfully !\n");
	return 0;
}
