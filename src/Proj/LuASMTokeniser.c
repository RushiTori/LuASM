#include "LuASMTokeniser.h"

typedef enum LangLabelType {
	INST_WHOLE,
	INST_PARTIAL,
	INST_COND,
	IS_LABEL,
} LangLabelType;

typedef struct LangLabel {
	const string value;
	TokenType tokType;
	LangLabelType type;
} LangLabel;

void getLangLabel(Token* tok, uint tokStartIdx, uint* actualIdx) {
	static const LangLabel betterLangLabels[] = {
		{"NOP", TOK_INST_NO_ARGS, INST_WHOLE},
		{"RET", TOK_INST_NO_ARGS, INST_WHOLE},
		{"RETURN", TOK_INST_NO_ARGS, INST_WHOLE},

		{"WAIT", TOK_INST_MAYBE_ONE_ARG, INST_WHOLE},

		{"STR", TOK_INST_DEST_SRC, INST_WHOLE},
		{"LSL", TOK_INST_DEST_SRC, INST_WHOLE},
		{"LSR", TOK_INST_DEST_SRC, INST_WHOLE},
		{"ASL", TOK_INST_DEST_SRC, INST_WHOLE},
		{"ASR", TOK_INST_DEST_SRC, INST_WHOLE},
		{"BSL", TOK_INST_DEST_SRC, INST_WHOLE},
		{"BSR", TOK_INST_DEST_SRC, INST_WHOLE},

		{"SCF", TOK_INST_ONEBYTE_VALUE, INST_WHOLE},

		{"STZ", TOK_INST_DEST, INST_WHOLE},
		{"INC", TOK_INST_DEST, INST_WHOLE},
		{"DEC", TOK_INST_DEST, INST_WHOLE},

		{"NOT", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"AND", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"NAND", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"OR", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"NOR", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"XOR", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"XNOR", TOK_INST_DEST_OPA_OPB, INST_WHOLE},

		{"ADD", TOK_INST_DEST_OPA_OPB, INST_PARTIAL},
		{"ADC", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"SUB", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"SBC", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"MUL", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"DIV", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"MOD", TOK_INST_DEST_OPA_OPB, INST_WHOLE},
		{"CMP", TOK_INST_DEST_OPA_OPB, INST_WHOLE},

		{"BR", TOK_INST_ADDR_ABS_REL, INST_PARTIAL},
		{"JMP", TOK_INST_ADDR_ABS_REL, INST_PARTIAL},
		{"GOTO", TOK_INST_ADDR_ABS_REL, INST_PARTIAL},

		{"CALL", TOK_INST_ADDR_ABS, INST_WHOLE},

		{"PUSH", TOK_INST_REGISTER, INST_WHOLE},
		{"POP", TOK_INST_REGISTER, INST_WHOLE},

		{"E", TOK_COND_E, INST_COND},
		{"EQ", TOK_COND_E, INST_COND},
		{"ZS", TOK_COND_E, INST_COND},

		{"NE", TOK_COND_NE, INST_COND},
		{"NEQ", TOK_COND_NE, INST_COND},
		{"ZC", TOK_COND_NE, INST_COND},

		{"UGTE", TOK_COND_UGTE, INST_COND},
		{"CS", TOK_COND_UGTE, INST_COND},

		{"ULT", TOK_COND_ULT, INST_COND},
		{"CC", TOK_COND_ULT, INST_COND},

		{"MI", TOK_COND_NEG, INST_COND},
		{"NEG", TOK_COND_NEG, INST_COND},
		{"NS", TOK_COND_NEG, INST_COND},

		{"PL", TOK_COND_POS, INST_COND},
		{"POS", TOK_COND_POS, INST_COND},
		{"NC", TOK_COND_POS, INST_COND},

		{"VS", TOK_COND_VS, INST_COND},

		{"VC", TOK_COND_VC, INST_COND},

		{"UGT", TOK_COND_UGT, INST_COND},

		{"ULTE", TOK_COND_ULTE, INST_COND},

		{"SGTE", TOK_COND_SGTE, INST_COND},
		{"GTE", TOK_COND_SGTE, INST_COND},

		{"SLT", TOK_COND_SLT, INST_COND},
		{"LT", TOK_COND_SLT, INST_COND},

		{"SGT", TOK_COND_SGT, INST_COND},
		{"GT", TOK_COND_SGT, INST_COND},

		{"SLTE", TOK_COND_SLE, INST_COND},
		{"LTE", TOK_COND_SLE, INST_COND},

		{"X", TOK_REG_X, IS_LABEL},

		{"Y", TOK_REG_Y, IS_LABEL},

		{"Z", TOK_REG_Z, IS_LABEL},

		{"S", TOK_STACKPTR, IS_LABEL},
		{"SPTR", TOK_STACKPTR, IS_LABEL},
		{"STACKPTR", TOK_STACKPTR, IS_LABEL},
		{"STACK", TOK_STACKPTR, IS_LABEL},

		{"RAM", TOK_RAM, IS_LABEL},
	};

	static const uint beterLangLabelsCount = sizeof(betterLangLabels) / sizeof(LangLabel);

	bool foundLangLabel = false;
	uint langLabelIdx = 0;

	for (uint i = 0; i < beterLangLabelsCount; i++) {
		if (betterLangLabels[i].type == INST_PARTIAL) {
			uint range = strlen(betterLangLabels[i].value);
			foundLangLabel = str_InsensitiveEqualsRange(tok->value, betterLangLabels[i].value, range);
		} else {
			foundLangLabel = str_InsensitiveEquals(tok->value, betterLangLabels[i].value);
		}

		if (foundLangLabel) {
			langLabelIdx = i;
			break;
		}
	}

	if (!foundLangLabel) return;

	if (betterLangLabels[langLabelIdx].type != INST_PARTIAL) {
		tok->type = betterLangLabels[langLabelIdx].tokType;
		return;
	}

	foundLangLabel = false;
	uint partialLen = strlen(betterLangLabels[langLabelIdx].value);
	for (uint i = 0; i < beterLangLabelsCount; i++) {
		if (betterLangLabels[i].type != INST_COND) continue;
		foundLangLabel = str_InsensitiveEquals(tok->value + partialLen, betterLangLabels[i].value);
		if (foundLangLabel) break;
	}

	if (!foundLangLabel) return;

	*actualIdx = tokStartIdx + partialLen;
	tok->type = betterLangLabels[langLabelIdx].tokType;
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
	Array tokens = arr_Create(sizeof(Token), 2720001, false, false);
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

			if (actualToken.type == TOK_INT_LIT) i--;

			arr_Push(&tokens, &actualToken, 1);
		}

		{
			Token newLineToken;
			newLineToken.type = TOK_NEWLINE;
			newLineToken.value = NULL;
			arr_Push(&tokens, &newLineToken, 1);
		}
	}

	return tokens;
}

void assembleTokens(const string path) {
	// WIP
	clock_t startTimer = clock();
	Array tokens = tokenise(path);
	clock_t endTimer = clock();

	for (uint i = 0; i < tokens.elementCount; i++) {
		Token* actualToken = arr_Get(&tokens, i);

		if (actualToken->type == TOK_LABEL) {
			if (i > 0) {
				Token* prevToken = arr_Get(&tokens, i - 1);
				if (prevToken->type == TOK_NEWLINE) {
					printf("\n");
				}
			}
		}

		switch (actualToken->type) {
			case TOK_INVALID:
				printf("INVALID ");
				break;
			case TOK_NEWLINE:
				printf("\n");
				break;

			case TOK_COMMA:
				printf("COMMA ");
				break;
			case TOK_COLON:
				printf("COLON ");
				break;
			case TOK_SEMICOLON:
				printf("SEMICOLON ");
				break;
			case TOK_PERIOD:
				printf("PERIOD ");
				break;

			case TOK_ADDRESS_BEGIN:
				printf("ADDRESS_BEGIN ");
				break;
			case TOK_ADDRESS_END:
				printf("ADDRESS_END ");
				break;

			case TOK_PLUS:
				printf("PLUS ");
				break;
			case TOK_MINUS:
				printf("MINUS ");
				break;
			case TOK_STAR:
				printf("STAR ");
				break;
			case TOK_FSLASH:
				printf("FSLASH ");
				break;
			case TOK_BSLASH:
				printf("BSLASH ");
				break;

			case TOK_INT_LIT:
				printf("INT_LIT(\"%s\") ", actualToken->value);
				break;
			case TOK_CHAR_LIT:
				printf("CHAR_LIT(\"%s\") ", actualToken->value);
				break;
			case TOK_STRING_LIT:
				printf("STRING_LIT(\"%s\") ", actualToken->value);
				break;

			case TOK_LABEL:
				printf("LABEL(\"%s\") ", actualToken->value);
				break;

			case TOK_INST_NO_ARGS:
				printf("INST_NO_ARGS ");
				break;
			case TOK_INST_MAYBE_ONE_ARG:
				printf("INST_MAYBE_ONE_ARG ");
				break;
			case TOK_INST_DEST_SRC:
				printf("INST_DEST_SRC ");
				break;
			case TOK_INST_ONEBYTE_VALUE:
				printf("INST_ONEBYTE_VALUE ");
				break;
			case TOK_INST_DEST:
				printf("INST_DEST ");
				break;
			case TOK_INST_DEST_OPA_OPB:
				printf("INST_DEST_OPA_OPB ");
				break;
			case TOK_INST_OPA_OPB:
				printf("INST_OPA_OPB ");
				break;
			case TOK_INST_ADDR_ABS_REL:
				printf("INST_ADDR_ABS_REL ");
				break;
			case TOK_INST_ADDR_ABS:
				printf("INST_ADDR_ABS ");
				break;
			case TOK_INST_REGISTER:
				printf("INST_REGISTER ");
				break;

			case TOK_COND_E:
				printf("COND_E ");
				break;
			case TOK_COND_NE:
				printf("COND_NE ");
				break;
			case TOK_COND_UGTE:
				printf("COND_UGTE ");
				break;
			case TOK_COND_ULT:
				printf("COND_ULT ");
				break;
			case TOK_COND_NEG:
				printf("COND_NEG ");
				break;
			case TOK_COND_POS:
				printf("COND_POS ");
				break;
			case TOK_COND_VS:
				printf("COND_VS ");
				break;
			case TOK_COND_VC:
				printf("COND_VC ");
				break;
			case TOK_COND_UGT:
				printf("COND_UGT ");
				break;
			case TOK_COND_ULTE:
				printf("COND_ULTE ");
				break;
			case TOK_COND_SGTE:
				printf("COND_SGTE ");
				break;
			case TOK_COND_SLT:
				printf("COND_SLT ");
				break;
			case TOK_COND_SGT:
				printf("COND_SGT ");
				break;
			case TOK_COND_SLE:
				printf("COND_SLE ");
				break;

			case TOK_REG_X:
				printf("REG_X ");
				break;
			case TOK_REG_Y:
				printf("REG_Y ");
				break;
			case TOK_REG_Z:
				printf("REG_Z ");
				break;
			case TOK_STACKPTR:
				printf("STACKPTR ");
				break;
			case TOK_RAM:
				printf("RAM ");
				break;
		}
	}

	printf("took %ld microS to create %d tokens !\n", endTimer - startTimer, tokens.elementCount);

	startTimer = clock();
	for (uint i = 0; i < tokens.elementCount; i++) {
		// free(((Token*)arr_Get(&tokens, i))->value);
		Token* tok = ((Token*)tokens.data) + i;
		if (tok->value) free(tok->value);
	}
	endTimer = clock();
	arr_Clear(&tokens, false);
	printf("took %ld microS to clean the tokens !\n", endTimer - startTimer);
}
