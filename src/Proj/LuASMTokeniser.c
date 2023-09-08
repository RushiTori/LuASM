#include "LuASMTokeniser.h"

#include <stdarg.h>

typedef enum LangLabelType {
	INST_WHOLE,
	INST_PARTIAL,
	INST_COND,
	IS_LABEL,
} LangLabelType;

typedef struct LangLabel {
	const string value;
	OpCode opCode;
	TokenType tokType;
	LangLabelType type;
} LangLabel;

HashMap codeLabels = {
	NULL,			 // BinaryTree* data;
	sizeof(ushort),	 // uint dataSize;
	0,				 // uint elementCount;
	false			 // bool containsPointers;
};

static const LangLabel langLabels[] = {
	{"NOP", OP_NOP, TOK_INST_NO_ARGS, INST_WHOLE},
	{"RET", OP_RET, TOK_INST_NO_ARGS, INST_WHOLE},
	{"RETURN", OP_RET, TOK_INST_NO_ARGS, INST_WHOLE},

	{"WAIT", OP_WAIT, TOK_INST_MAYBE_ONE_ARG, INST_WHOLE},

	{"STR", OP_STR, TOK_INST_DEST_SRC, INST_WHOLE},
	{"LSL", OP_LSL, TOK_INST_DEST_SRC, INST_WHOLE},
	{"LSR", OP_LSR, TOK_INST_DEST_SRC, INST_WHOLE},
	{"ASL", OP_ASL, TOK_INST_DEST_SRC, INST_WHOLE},
	{"ASR", OP_ASR, TOK_INST_DEST_SRC, INST_WHOLE},
	{"BSL", OP_BSL, TOK_INST_DEST_SRC, INST_WHOLE},
	{"BSR", OP_BSR, TOK_INST_DEST_SRC, INST_WHOLE},

	{"SCF", OP_SCF, TOK_INST_ONEBYTE_VALUE, INST_WHOLE},

	{"STZ", OP_STZ, TOK_INST_DEST, INST_WHOLE},
	{"INC", OP_INC, TOK_INST_DEST, INST_WHOLE},
	{"DEC", OP_DEC, TOK_INST_DEST, INST_WHOLE},

	{"NOT", OP_NOT, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"AND", OP_AND, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"NAND", OP_NAND, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"OR", OP_OR, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"NOR", OP_NOR, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"XOR", OP_XOR, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"XNOR", OP_XNOR, TOK_INST_DEST_OPA_OPB, INST_WHOLE},

	{"ADD", OP_ADD, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"ADC", OP_ADC, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"SUB", OP_SUB, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"SBC", OP_SBC, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"MUL", OP_MUL, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"DIV", OP_DIV, TOK_INST_DEST_OPA_OPB, INST_WHOLE},
	{"MOD", OP_MOD, TOK_INST_DEST_OPA_OPB, INST_WHOLE},

	{"CMP", OP_CMP, TOK_INST_OPA_OPB, INST_WHOLE},

	{"BR", OP_JMPA, TOK_INST_ADDR_ABS_REL, INST_PARTIAL},
	{"JMP", OP_JMPA, TOK_INST_ADDR_ABS_REL, INST_PARTIAL},
	{"GOTO", OP_JMPA, TOK_INST_ADDR_ABS_REL, INST_PARTIAL},

	{"CALL", OP_CALL, TOK_INST_ADDR_ABS, INST_WHOLE},

	{"PUSH", OP_PUSH, TOK_INST_REGISTER, INST_WHOLE},
	{"POP", OP_POP, TOK_INST_REGISTER, INST_WHOLE},

	{"E", OP_BADOP, TOK_COND_E, INST_COND},
	{"EQ", OP_BADOP, TOK_COND_E, INST_COND},
	{"ZS", OP_BADOP, TOK_COND_E, INST_COND},

	{"NE", OP_BADOP, TOK_COND_NE, INST_COND},
	{"NEQ", OP_BADOP, TOK_COND_NE, INST_COND},
	{"ZC", OP_BADOP, TOK_COND_NE, INST_COND},

	{"UGTE", OP_BADOP, TOK_COND_UGTE, INST_COND},
	{"CS", OP_BADOP, TOK_COND_UGTE, INST_COND},

	{"ULT", OP_BADOP, TOK_COND_ULT, INST_COND},
	{"CC", OP_BADOP, TOK_COND_ULT, INST_COND},

	{"MI", OP_BADOP, TOK_COND_NEG, INST_COND},
	{"NEG", OP_BADOP, TOK_COND_NEG, INST_COND},
	{"NS", OP_BADOP, TOK_COND_NEG, INST_COND},

	{"PL", OP_BADOP, TOK_COND_POS, INST_COND},
	{"POS", OP_BADOP, TOK_COND_POS, INST_COND},
	{"NC", OP_BADOP, TOK_COND_POS, INST_COND},

	{"VS", OP_BADOP, TOK_COND_VS, INST_COND},

	{"VC", OP_BADOP, TOK_COND_VC, INST_COND},

	{"UGT", OP_BADOP, TOK_COND_UGT, INST_COND},

	{"ULTE", OP_BADOP, TOK_COND_ULTE, INST_COND},

	{"SGTE", OP_BADOP, TOK_COND_SGTE, INST_COND},
	{"GTE", OP_BADOP, TOK_COND_SGTE, INST_COND},

	{"SLT", OP_BADOP, TOK_COND_SLT, INST_COND},
	{"LT", OP_BADOP, TOK_COND_SLT, INST_COND},

	{"SGT", OP_BADOP, TOK_COND_SGT, INST_COND},
	{"GT", OP_BADOP, TOK_COND_SGT, INST_COND},

	{"SLTE", OP_BADOP, TOK_COND_SLE, INST_COND},
	{"LTE", OP_BADOP, TOK_COND_SLE, INST_COND},

	{"X", OP_BADOP, TOK_REG_X, IS_LABEL},

	{"Y", OP_BADOP, TOK_REG_Y, IS_LABEL},

	{"Z", OP_BADOP, TOK_REG_Z, IS_LABEL},

	{"S", OP_BADOP, TOK_STACKPTR, IS_LABEL},
	{"SPTR", OP_BADOP, TOK_STACKPTR, IS_LABEL},
	{"STACKPTR", OP_BADOP, TOK_STACKPTR, IS_LABEL},
	{"STACK", OP_BADOP, TOK_STACKPTR, IS_LABEL},

	{"RAM", OP_BADOP, TOK_RAM, IS_LABEL},
};

static const uint langLabelsCount = sizeof(langLabels) / sizeof(LangLabel);

void getLangLabel(Token* tok, uint tokStartIdx, uint* actualIdx) {
	bool foundLangLabel = false;
	uint langLabelIdx = 0;

	for (uint i = 0; i < langLabelsCount; i++) {
		if (langLabels[i].type == INST_PARTIAL) {
			uint range = strlen(langLabels[i].value);
			foundLangLabel = str_InsensitiveEqualsRange(tok->value, langLabels[i].value, range);
		} else {
			foundLangLabel = str_InsensitiveEquals(tok->value, langLabels[i].value);
		}

		if (foundLangLabel) {
			langLabelIdx = i;
			break;
		}
	}

	if (!foundLangLabel) return;

	if (langLabels[langLabelIdx].type != INST_PARTIAL) {
		tok->type = langLabels[langLabelIdx].tokType;
		return;
	}

	uint partialLen = strlen(langLabels[langLabelIdx].value);
	if (partialLen == strlen(tok->value)) {
		tok->type = langLabels[langLabelIdx].tokType;
		return;
	}

	foundLangLabel = false;
	for (uint i = 0; i < langLabelsCount; i++) {
		if (langLabels[i].type != INST_COND) continue;
		foundLangLabel = str_InsensitiveEquals(tok->value + partialLen, langLabels[i].value);
		if (foundLangLabel) break;
	}

	if (!foundLangLabel) return;

	*actualIdx = tokStartIdx + partialLen;
	tok->type = langLabels[langLabelIdx].tokType;
}

bool isValidIntLitChar(char c, uint base) {
	if (c == '\'') return true;
	if (base == 2) return ((c == '0') || (c == '1'));
	if (base == 10) return ((c >= '0') && (c <= '9'));
	c = str_ToLowerChar(c);
	if (base == 16) return (str_IsDigit(c) || ((c >= 'a') && (c <= 'f')));
	return false;
}

bool isValidLabelChar(char c) { return (str_IsLetter(c) || str_IsDigit(c) || (c == '_')); }

Array tokenise(const string path) {
	Array tokens = arr_Create(sizeof(Token), 1024, false, false);
	string* lines = io_LoadStrings(path);

	for (uint lineIdx = 0; lines[lineIdx]; lineIdx++) {
		string line = lines[lineIdx];
		uint lineLen = strlen(line);
		for (uint i = 0; i < lineLen; i++) {
			if (str_IsSpace(line[i])) continue;
			Token actualToken;
			actualToken.type = TOK_INVALID;
			actualToken.value = NULL;

			uint tokenValueStart = i;
			switch (line[i]) {
				case ',':
					actualToken.type = TOK_COMMA;
					break;
				case ':':
					actualToken.type = TOK_COLON;
					break;
				case ';':
					actualToken.type = TOK_SEMICOLON;
					break;
				case '.':
					actualToken.type = TOK_PERIOD;
					break;

				case '[':
					actualToken.type = TOK_ADDRESS_BEGIN;
					break;
				case ']':
					actualToken.type = TOK_ADDRESS_END;
					break;

				case '+':
					actualToken.type = TOK_PLUS;
					break;
				case '-':
					actualToken.type = TOK_MINUS;
					break;
				case '*':
					actualToken.type = TOK_STAR;
					break;
				case '/':
					actualToken.type = TOK_FSLASH;
					break;
				case '\\':
					actualToken.type = TOK_BSLASH;
					break;

				case '\'': {
					while (++i < lineLen) {
						if (line[i] == '\'') break;
						if (line[i] == '\\') i++;
					}
					if (i >= lineLen) break;
					tokenValueStart++;
					actualToken.type = TOK_CHAR_LIT;
				} break;

				case '\"': {
					while (++i < lineLen) {
						if (line[i] == '\"') break;
						if (line[i] == '\\') i++;
					}
					if (i >= lineLen) break;
					tokenValueStart++;
					actualToken.type = TOK_STRING_LIT;
				} break;

				default: {
					if (str_IsDigit(line[i])) {
						actualToken.type = TOK_INT_LIT;
						uint base = 10;
						if (line[i] == '0' && i < (lineLen - 1)) {
							if (str_ToLowerChar(line[i + 1]) == 'b') base = 2;
							if (str_ToLowerChar(line[i + 1]) == 'x') base = 16;
							if (base != 10) i += 2;
						}

						while (i < lineLen) {
							if (!isValidIntLitChar(line[i], base)) break;
							i++;
						}
					} else if (str_IsLetter(line[i])) {
						actualToken.type = TOK_LABEL;
						while (i < lineLen) {
							if (!isValidLabelChar(line[i])) break;
							i++;
						}
					}
				} break;
			}

			if (actualToken.type == TOK_INVALID) {
				printf("Invalid token character at line %d:%d : \"%c\"\n", lineIdx, i, line[i]);
				for (uint j = 0; j < tokens.elementCount; j++) {
					free(((Token*)arr_Get(&tokens, j))->value);
				}
				arr_Clear(&tokens, false);
				return tokens;
			}

			if (actualToken.type == TOK_SEMICOLON) break;

			if (i != tokenValueStart) {
				uint tokenValueLen = i - tokenValueStart;
				actualToken.value = calloc(tokenValueLen + 1, sizeof(char));
				memcpy(actualToken.value, line + tokenValueStart, tokenValueLen);
			}

			if (actualToken.type == TOK_LABEL) {
				getLangLabel(&actualToken, tokenValueStart, &i);
				i--;
			}

			if ((actualToken.type == TOK_CHAR_LIT) || (actualToken.type == TOK_STRING_LIT)) i--;

			arr_Push(&tokens, &actualToken, 1);
		}

		if ((((Token*)tokens.data) + tokens.elementCount - 1)->type != TOK_NEWLINE) {
			Token newLineToken;
			newLineToken.type = TOK_NEWLINE;
			newLineToken.value = NULL;
			arr_Push(&tokens, &newLineToken, 1);
		}
	}

	arr_ShrinkToFit(&tokens);
	return tokens;
}

void printExpectedTokens(const Token* unexpected, uint lineIdx, uint expectedCount, ...) {
	printf("Unexpected token ");
	printToken(unexpected);
	printf(" at line %d\n", lineIdx + 1);
	printf("Expected any of the following : \n");

	va_list argList;
	va_start(argList, expectedCount);

	Token tok;
	tok.value = NULL;
	for (uint i = 0; i < expectedCount; i++) {
		tok.type = va_arg(argList, TokenType);
		printf("\t-");
		printToken(&tok);
		printf("\n");
	}
	va_end(argList);
}

void tokenAsOpCode(Token* tok, TokenLine* line) {
	for (uint i = 0; i < langLabelsCount; i++) {
		if (str_InsensitiveEquals(tok->value, langLabels[i].value)) {
			line->opCode = langLabels[i].opCode;
			return;
		}
	}
}

char parseCharLit(const string charLit) {
	if (!charLit || !charLit[0]) return '\0';
	if (charLit[0] != '\\') return charLit[0];

	switch (charLit[1]) {
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
			break;
	}

	return '\0';
}

bool checkStopToken(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	(*tok)++;
	(*tokIdx)++;
	if ((*tok)->type != stopTok) {
		printExpectedTokens(*tok, line->lineIdx, 1, stopTok);
		return false;
	}
	return true;
}

bool tokenAsValue(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	if ((*tok)->type == TOK_CHAR_LIT) {
		if (!(*tok)->value) {
			printf("Tokeniser Error at line %d : CHAR_LIT without value\n", line->lineIdx);
		}

		char val = parseCharLit((*tok)->value);
		if (val == '\0') {
			printf("Tokeniser Error at line %d : CHAR_LIT without value\n", line->lineIdx);
		}
		line->argValues[line->valuesCount++] = val;
	} else if ((*tok)->type == TOK_INT_LIT) {
		if (!(*tok)->value) {
			printf("Tokeniser Error at line %d : INT_LIT without value\n", line->lineIdx);
		}

		str_RemoveChars((*tok)->value, "\'");

		string numLit = (*tok)->value;
		uint base = 10;

		if (str_InsensitiveEqualsRange((*tok)->value, "0b", 2)) {
			numLit += 2;
			base = 2;
		} else if (str_InsensitiveEqualsRange((*tok)->value, "0x", 2)) {
			numLit += 2;
			base = 16;
		}
		if (!(*numLit)) printf("Tokeniser Error at line %d : INT_LIT without value\n", line->lineIdx);

		line->argValues[line->valuesCount++] = str_StrToUInt(numLit, base, UINT16_MAX);
	} else if ((*tok)->type == TOK_LABEL) {
		line->argValues[line->valuesCount++] = labelAsCodeValue(tok, tokIdx, line);
	} else {
		printExpectedTokens(*tok, line->lineIdx, 2, TOK_CHAR_LIT, TOK_INT_LIT);
		return false;
	}
	line->modes[line->modeCount++] = MODE_VALUE;
	return checkStopToken(tok, tokIdx, stopTok, line);
}

bool tokenAsReg(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	InstArgMode tempMode;
	if ((*tok)->type == TOK_REG_X) {
		tempMode = MODE_REG_X;
	} else if ((*tok)->type == TOK_REG_Y) {
		tempMode = MODE_REG_Y;
	} else if ((*tok)->type == TOK_REG_Z) {
		tempMode = MODE_REG_Z;
	} else if ((*tok)->type == TOK_STACKPTR) {
		tempMode = MODE_SPTR;
	} else {
		printExpectedTokens(*tok, line->lineIdx, 4, TOK_REG_X, TOK_REG_Y, TOK_REG_Z, TOK_STACKPTR);
		return false;
	}

	line->modes[line->modeCount++] = tempMode;
	return checkStopToken(tok, tokIdx, stopTok, line);
}

void tokenAsCond(Token** tok, uint* tokIdx, TokenLine* line) {
	uint prevModeCount = line->modeCount;
	if ((*tok)->type == TOK_COND_E) line->modes[line->modeCount++] = COND_Equal;
	if ((*tok)->type == TOK_COND_NE) line->modes[line->modeCount++] = COND_NotEqual;
	if ((*tok)->type == TOK_COND_UGTE) line->modes[line->modeCount++] = COND_UnsignedGreaterThanOrEqual;
	if ((*tok)->type == TOK_COND_ULT) line->modes[line->modeCount++] = COND_UnsignedLowerThan;
	if ((*tok)->type == TOK_COND_NEG) line->modes[line->modeCount++] = COND_Negative;
	if ((*tok)->type == TOK_COND_POS) line->modes[line->modeCount++] = COND_Positive;
	if ((*tok)->type == TOK_COND_VS) line->modes[line->modeCount++] = COND_Overflow;
	if ((*tok)->type == TOK_COND_VC) line->modes[line->modeCount++] = COND_NotOverflow;
	if ((*tok)->type == TOK_COND_UGT) line->modes[line->modeCount++] = COND_UnsignedGreaterThan;
	if ((*tok)->type == TOK_COND_ULTE) line->modes[line->modeCount++] = COND_UnsignedLowerThanOrEqual;
	if ((*tok)->type == TOK_COND_SGTE) line->modes[line->modeCount++] = COND_SignedGreaterThanOrEqual;
	if ((*tok)->type == TOK_COND_SLT) line->modes[line->modeCount++] = COND_SignedLowerThan;
	if ((*tok)->type == TOK_COND_SGT) line->modes[line->modeCount++] = COND_SignedGreaterThan;
	if ((*tok)->type == TOK_COND_SLE) line->modes[line->modeCount++] = COND_SignedLowerThanOrEqual;
	if (prevModeCount == line->modeCount) return;
	(*tok)++;
	(*tokIdx)++;
}

bool tokenAsAddr(Token** tok, uint* tokIdx, TokenLine* line) {
	switch ((*tok)->type) {
		case TOK_REG_X:
		case TOK_REG_Y:
		case TOK_REG_Z:
		case TOK_STACKPTR:
		case TOK_CHAR_LIT:
		case TOK_INT_LIT:
			if (tokenAsArg(tok, tokIdx, TOK_ADDRESS_END, line)) {
				line->modes[line->modeCount - 1] |= MODE_RAM_REG_X;
				return true;
			}
			return false;

		default:
			break;
	}

	printExpectedTokens(*tok, line->lineIdx, 6, TOK_REG_X, TOK_REG_Y, TOK_REG_Z, TOK_STACKPTR, TOK_CHAR_LIT,
						TOK_INT_LIT);
	return false;
}

bool tokenAsArg(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	switch ((*tok)->type) {
		case TOK_RAM: {
			(*tok)++;
			(*tokIdx)++;
			if ((*tok)->type != TOK_ADDRESS_BEGIN) {
				printExpectedTokens(*tok, line->lineIdx, 1, TOK_ADDRESS_BEGIN);
				return false;
			}
		}
			__attribute__((fallthrough));
		case TOK_ADDRESS_BEGIN: {
			bool failed = !tokenAsAddr(tok, tokIdx, line);
			if (failed) return false;

			return checkStopToken(tok, tokIdx, stopTok, line);
		} break;

		case TOK_REG_X:
		case TOK_REG_Y:
		case TOK_REG_Z:
		case TOK_STACKPTR:
			return tokenAsReg(tok, tokIdx, stopTok, line);

		case TOK_CHAR_LIT:
		case TOK_INT_LIT: {
			return tokenAsValue(tok, tokIdx, stopTok, line);
		} break;

		default:
			break;
	}

	printExpectedTokens(*tok, line->lineIdx, 8, TOK_RAM, TOK_ADDRESS_BEGIN, TOK_REG_X, TOK_REG_Y, TOK_REG_Z,
						TOK_STACKPTR, TOK_CHAR_LIT, TOK_INT_LIT);
	return false;
}

bool tokenAsSrc(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	return tokenAsArg(tok, tokIdx, stopTok, line);
}

bool tokenAsDest(Token** tok, uint* tokIdx, TokenType stopTok, TokenLine* line) {
	switch ((*tok)->type) {
		case TOK_RAM:
		case TOK_ADDRESS_BEGIN:
		case TOK_REG_X:
		case TOK_REG_Y:
		case TOK_REG_Z:
		case TOK_STACKPTR:
			return tokenAsArg(tok, tokIdx, stopTok, line);

		default:
			break;
	}

	printExpectedTokens(*tok, line->lineIdx, 6, TOK_RAM, TOK_ADDRESS_BEGIN, TOK_REG_X, TOK_REG_Y, TOK_REG_Z,
						TOK_STACKPTR);
	return false;
}

void writeTokenLine(TokenLine* tokLine) {
	uint tempCodePtr = codePtr;
	codeRAM[codePtr++] = tokLine->opCode;

	if (tokLine->modeCount == 1) {
		codeRAM[codePtr++] = tokLine->modes[0];
	} else if (tokLine->modeCount == 2) {
		codeRAM[codePtr++] = combineModeLowHigh(tokLine->modes[1], tokLine->modes[0]);
	} else if (tokLine->modeCount == 3) {
		codeRAM[codePtr++] = tokLine->modes[0];
		codeRAM[codePtr++] = combineModeLowHigh(tokLine->modes[2], tokLine->modes[1]);
	}

	for (uint j = 0; j < tokLine->valuesCount; j++) {
		if (tokLine->oneByteValues) {
			codeRAM[codePtr++] = tokLine->argValues[j];
		} else {
			*(ushort*)(codeRAM + codePtr) = tokLine->argValues[j];
			codePtr += 2;
		}
	}

	/*printf("adding %X: %d, %d, %d\n", tokLine->opCode, tokLine->modeCount, tokLine->valuesCount,
		   tokLine->oneByteValues);
	for (; tempCodePtr < codePtr; tempCodePtr++) {
		printf("codeRAM[%d] : %X\n", tempCodePtr, codeRAM[tempCodePtr]);
	}
	printf("\n");*/
}

ushort labelAsCodeValue(Token** tok, uint* tokIdx, TokenLine* line) {
	// WIP
	if (hm_Contains(&codeLabels, (*tok)->value)) return *(ushort*)hm_Get(&codeLabels, (*tok)->value);
	printf("Forward label declaration isn't supported yet !!\n");
	return 0xFFFF;
}

void assembleTokens(const string path) {
	// WIP
	Array tokens = tokenise(path);

	codePtr = 0;

	for (uint i = 0, lineIdx = 0; i < tokens.elementCount; i++, lineIdx++) {
		Token* actualToken = arr_Get(&tokens, i++);
		Token* nextToken = arr_Get(&tokens, i);

		TokenLine tokLine;
		tokLine.opCode = OP_BADOP;
		tokLine.modeCount = 0;
		tokLine.valuesCount = 0;
		tokLine.lineIdx = lineIdx;
		tokLine.oneByteValues = false;

		bool badTokens = true;

		switch (actualToken->type) {
			case TOK_LABEL: {
				if (nextToken->type != TOK_COLON) {
					printExpectedTokens(nextToken, lineIdx, 1, TOK_COLON);
					break;
				}

				if (hm_Contains(&codeLabels, actualToken->value)) {
					printToken(actualToken);
					printf(" was previoulsy as %d\n", *((ushort*)hm_Get(&codeLabels, actualToken->value)));
					break;
				}
				i++;
				hm_Push(&codeLabels, actualToken->value, &codePtr);
				badTokens = false;
				printf("added code-label \"%s\" at %d\n", actualToken->value, codePtr);
			} break;

			case TOK_INST_NO_ARGS: {
				if (nextToken->type != TOK_NEWLINE) break;
				tokLine.opCode = OP_NOP;
			} break;

			case TOK_INST_MAYBE_ONE_ARG: {
				if (nextToken->type == TOK_NEWLINE) {
					tokLine.opCode = OP_NOP;
					break;
				}
				if (!tokenAsSrc(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokLine.opCode = OP_WAIT;
			} break;

			case TOK_INST_DEST_SRC: {
				if (!tokenAsDest(&nextToken, &i, TOK_COMMA, &tokLine)) break;
				i++;
				nextToken++;
				if (!tokenAsSrc(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_ONEBYTE_VALUE: {
				if (!tokenAsSrc(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_DEST: {
				if (!tokenAsDest(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_DEST_OPA_OPB: {
				if (!tokenAsDest(&nextToken, &i, TOK_COMMA, &tokLine)) break;
				i++;
				nextToken++;
				if (!tokenAsSrc(&nextToken, &i, TOK_COMMA, &tokLine)) break;
				i++;
				nextToken++;
				if (!tokenAsSrc(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_OPA_OPB: {
				if (!tokenAsSrc(&nextToken, &i, TOK_COMMA, &tokLine)) break;
				i++;
				nextToken++;
				if (!tokenAsSrc(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_ADDR_ABS_REL: {
				tokenAsCond(&nextToken, &i, &tokLine);
				if (!tokenAsValue(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				int diff = tokLine.argValues[0] - (codePtr + 1 + tokLine.modeCount);
				if (tokLine.modes[tokLine.modeCount - 1] == MODE_VALUE) tokLine.modeCount--;

				if ((diff <= INT8_MAX) && (diff >= INT8_MIN)) {
					tokLine.oneByteValues = true;
					tokLine.argValues[0] = diff;
					if (!tokLine.modeCount) {
						tokLine.opCode = OP_JMPR;
					} else {
						tokLine.opCode = OP_JMPRC;
					}
				} else {
					if (!tokLine.modeCount) {
						tokLine.opCode = OP_JMPA;
					} else {
						tokLine.opCode = OP_JMPAC;
					}
				}

			} break;

			case TOK_INST_ADDR_ABS: {
				if (!tokenAsValue(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			case TOK_INST_REGISTER: {
				if (!tokenAsReg(&nextToken, &i, TOK_NEWLINE, &tokLine)) break;
				tokenAsOpCode(actualToken, &tokLine);
			} break;

			default:
				printf("Token misalignment at line %d : ", lineIdx);
				printToken(actualToken);
				printf("\n");
				break;
		}
		if (!badTokens) continue;

		if (tokLine.opCode == OP_BADOP) {
			printf("broke out for some reason :/ at : line %d:%d\n", lineIdx, i);
			break;
		}

		writeTokenLine(&tokLine);
	}

	printf("%s assembled into a %d bytes program\n", path, codePtr);
	codePtr = 0;

	for (uint i = 0; i < tokens.elementCount; i++) {
		free(((Token*)arr_Get(&tokens, i))->value);
	}
	arr_Clear(&tokens, false);
	hm_Clear(&codeLabels, false);
}

void printToken(const Token* tok) {
	switch (tok->type) {
		case TOK_INVALID:
			printf("INVALID");
			break;
		case TOK_NEWLINE:
			printf("NEWLINE");
			break;

		case TOK_COMMA:
			printf("COMMA");
			break;
		case TOK_COLON:
			printf("COLON");
			break;
		case TOK_SEMICOLON:
			printf("SEMICOLON");
			break;
		case TOK_PERIOD:
			printf("PERIOD");
			break;

		case TOK_ADDRESS_BEGIN:
			printf("ADDRESS_BEGIN");
			break;
		case TOK_ADDRESS_END:
			printf("ADDRESS_END");
			break;

		case TOK_PLUS:
			printf("PLUS");
			break;
		case TOK_MINUS:
			printf("MINUS");
			break;
		case TOK_STAR:
			printf("STAR");
			break;
		case TOK_FSLASH:
			printf("FSLASH");
			break;
		case TOK_BSLASH:
			printf("BSLASH");
			break;

		case TOK_INT_LIT:
			printf("INT_LIT");
			break;
		case TOK_CHAR_LIT:
			printf("CHAR_LIT");
			break;
		case TOK_STRING_LIT:
			printf("STRING_LIT");
			break;

		case TOK_LABEL:
			printf("LABEL");
			break;

		case TOK_INST_NO_ARGS:
			printf("INST_NO_ARGS");
			break;
		case TOK_INST_MAYBE_ONE_ARG:
			printf("INST_MAYBE_ONE_ARG");
			break;
		case TOK_INST_DEST_SRC:
			printf("INST_DEST_SRC");
			break;
		case TOK_INST_ONEBYTE_VALUE:
			printf("INST_ONEBYTE_VALUE");
			break;
		case TOK_INST_DEST:
			printf("INST_DEST");
			break;
		case TOK_INST_DEST_OPA_OPB:
			printf("INST_DEST_OPA_OPB");
			break;
		case TOK_INST_OPA_OPB:
			printf("INST_OPA_OPB");
			break;
		case TOK_INST_ADDR_ABS_REL:
			printf("INST_ADDR_ABS_REL");
			break;
		case TOK_INST_ADDR_ABS:
			printf("INST_ADDR_ABS");
			break;
		case TOK_INST_REGISTER:
			printf("INST_REGISTER");
			break;

		case TOK_COND_E:
			printf("COND_E");
			break;
		case TOK_COND_NE:
			printf("COND_NE");
			break;
		case TOK_COND_UGTE:
			printf("COND_UGTE");
			break;
		case TOK_COND_ULT:
			printf("COND_ULT");
			break;
		case TOK_COND_NEG:
			printf("COND_NEG");
			break;
		case TOK_COND_POS:
			printf("COND_POS");
			break;
		case TOK_COND_VS:
			printf("COND_VS");
			break;
		case TOK_COND_VC:
			printf("COND_VC");
			break;
		case TOK_COND_UGT:
			printf("COND_UGT");
			break;
		case TOK_COND_ULTE:
			printf("COND_ULTE");
			break;
		case TOK_COND_SGTE:
			printf("COND_SGTE");
			break;
		case TOK_COND_SLT:
			printf("COND_SLT");
			break;
		case TOK_COND_SGT:
			printf("COND_SGT");
			break;
		case TOK_COND_SLE:
			printf("COND_SLE");
			break;

		case TOK_REG_X:
			printf("REG_X");
			break;
		case TOK_REG_Y:
			printf("REG_Y");
			break;
		case TOK_REG_Z:
			printf("REG_Z");
			break;
		case TOK_STACKPTR:
			printf("STACKPTR");
			break;
		case TOK_RAM:
			printf("RAM");
			break;
	}
	if (tok->value || tok->type == TOK_INT_LIT || tok->type == TOK_CHAR_LIT || tok->type == TOK_STRING_LIT ||
		tok->type == TOK_LABEL) {
		printf("(\"%s\")", tok->value);
	}

	if (tok->type != TOK_NEWLINE) printf(" ");
}