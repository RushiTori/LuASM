#ifndef LU_ASM_INSTRUCTIONS
#define LU_ASM_INSTRUCTIONS

#include "LuASMCommon.h"

void NOP();
void WAIT();

void STR();
void STZ();
void SCF();

void INC();
void DEC();

void AND();
void NAND();
void OR();
void NOR();
void XOR();
void XNOR();
void NOT();

void ADD();
void ADC();
void SUB();
void SBC();
void MUL();
void DIV();
void MOD();

void CMP();

void JMPA();
void JMPR();
void JMPAC();
void JMPRC();

void CALL();
void RET();
void PUSH();
void POP();

#endif  // LU_ASM_INSTRUCTIONS
