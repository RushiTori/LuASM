#ifndef LU_ASM_TOKENISER_H
#define LU_ASM_TOKENISER_H

#include "LuASMCommon.h"

typedef enum TokenType {
	TOK_INVALID = 0,
	TOK_NEWLINE,

	TOK_COMMA,
	TOK_COLON,
	TOK_SEMICOLON,
	TOK_PERIOD,

	TOK_ADDRESS_BEGIN,
	TOK_ADDRESS_END,

	TOK_PLUS,
	TOK_MINUS,
	TOK_STAR,
	TOK_FSLASH,
	TOK_BSLASH,

	TOK_INT_LIT,
	TOK_CHAR_LIT,
	TOK_STRING_LIT,

	TOK_LABEL,

	TOK_INST_NO_ARGS,
	TOK_INST_MAYBE_ONE_ARG,
	TOK_INST_DEST_SRC,
	TOK_INST_ONEBYTE_VALUE,
	TOK_INST_DEST,
	TOK_INST_DEST_OPA_OPB,
	TOK_INST_OPA_OPB,
	TOK_INST_ADDR_ABS_REL,
	TOK_INST_ADDR_ABS,
	TOK_INST_REGISTER,

	TOK_COND_E,
	TOK_COND_NE,
	TOK_COND_UGTE,
	TOK_COND_ULT,
	TOK_COND_NEG,
	TOK_COND_POS,
	TOK_COND_VS,
	TOK_COND_VC,
	TOK_COND_UGT,
	TOK_COND_ULTE,
	TOK_COND_SGTE,
	TOK_COND_SLT,
	TOK_COND_SGT,
	TOK_COND_SLE,

	TOK_REG_X,
	TOK_REG_Y,
	TOK_REG_Z,
	TOK_STACKPTR,
	TOK_RAM,
} TokenType;

typedef struct Token {
	TokenType type;
	string value;
} Token;

Array tokenise(const string path);
void assembleTokens(const string path);
void printToken(const Token* tok);

typedef struct TokenLine {
	/*OpCode*/ uchar opCode;
	/*InstArgMode*/ uchar modes[3];
	uchar modeCount;
	ushort argValues[3];
	uchar valuesCount;
	bool oneByteValues;
	uint lineIdx;
} TokenLine;

void tokenAsOpCode(Token* tok, TokenLine* line);

bool checkStopToken(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);

bool tokenAsValue(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);
bool tokenAsReg(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);
void tokenAsCond(Token** tok, uint* tokIdx, TokenLine* line);
bool tokenAsAddr(Token** tok, uint* tokIdx, TokenLine* line);
bool tokenAsArg(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);
bool tokenAsSrc(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);
bool tokenAsDest(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line);

void writeTokenLine(TokenLine* tokLine);


ushort labelAsCodeValue(Token** tok, uint* tokIdx, TokenLine* line);

#endif	// LU_ASM_TOKENISER_H