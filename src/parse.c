#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "parse.h"
#include "debug.h"
#include "mem.h"
#include "utils.h"

static inline int is_endline(char c) {
    return c == '.' || c == '\n' || c == '\0';
}

static uint64_t parse_num(ShortVM *vm, Status *status) {
    char *endptr;
    uint64_t num = (uint64_t) strtoull(vm->code + vm->pc, &endptr, 10);

    if (endptr == vm->code + vm->pc) {
        *status = STATUS_EXPECTED_NUM;
        return 0;
    }

    vm->pc += endptr - (vm->code + vm->pc);

    *status = STATUS_SUCCESS;
    return num;
}

static char parse_op(ShortVM *vm) {
    char c = vm->code[vm->pc];
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '%' || c == '^' || c == '&' || c == '|' || c == '?') {
        return vm->code[vm->pc++];
    } else if (c != '\0') {
        if (vm->code[vm->pc] == '>' && vm->code[vm->pc+1] == '>') {
            vm->pc += 2;
            return '>';
        } else if (vm->code[vm->pc] == '<' && vm->code[vm->pc+1] == '<') {
            vm->pc += 2;
            return '<';
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static char* parse_cond_op(ShortVM *vm) {
    if (vm->code[vm->pc] == '\0') return 0;

    char *op = "";

    if (vm->code[vm->pc+1] == '=') {
        if (vm->code[vm->pc] == '=') op = "==";
        else if (vm->code[vm->pc] == '>') op = ">=";
        else if (vm->code[vm->pc] == '<') op = "<=";
    } else {
        if (vm->code[vm->pc] == '!') op = "!";
        else if (vm->code[vm->pc] == '>') op = ">";
        else if (vm->code[vm->pc] == '<') op = "<";
    }

    vm->pc += strlen(op);
    return op;
}

static uint64_t parse_lit(ShortVM *vm, Status *status) {
    char *endptr = (char*) vm->code + vm->pc;
    uint64_t lit = 0;

    const char *code = vm->code;

    char c = code[vm->pc];
    char n = code[vm->pc+1];

    if (c == '0' && n == 'x') {
        vm->pc += 2; // '0x' - 2 symbols
        // if it was "0x2A", then now code + pc is pointing to "2A"

        if (!isdigit_in_hex(code[vm->pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + vm->pc, &endptr, 16); // hex
    } else if (c == '0' && n == 'o') {
        vm->pc += 2;
        // if it was "0o52", then now code + pc is pointing to "52"
        
        if (!isdigit_in_oct(code[vm->pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + vm->pc, &endptr, 8); // octal
    } else if (c == '0' && n == 'b') {
        vm->pc += 2;
        // if it was "0b101010", then now code + pc is pointing to "101010"

        if (!isdigit_in_bin(code[vm->pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + vm->pc, &endptr, 2); // binary
    } else if (c == '\'') {
        vm->pc++;
        // If it was "'A'", then now code + pc is pointing to "A'"

        if (code[vm->pc] == '\0') {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }

        if (code[vm->pc] != '\'' && code[vm->pc] != '\\') {
            uint8_t first_byte = (uint8_t) code[vm->pc];

            int utf8_len = utf8_char_len(first_byte);
            if (utf8_len == 0) {
                *status = STATUS_INVALID_SYMBOL;
                return 0;
            }

            for (int i = 0; i < utf8_len; i++) {
                if (code[vm->pc+i] == '\0') {
                    *status = STATUS_INVALID_SYMBOL;
                    return 0;
                }
                if (i > 0) {
                    uint8_t byte = (uint8_t)code[vm->pc + i];
                    if ((byte & 0xC0) != 0x80) {
                        *status = STATUS_INVALID_SYMBOL;
                        return 0;
                    }
                }
            }

            if (utf8_len == 1) {
                lit = (uint64_t) code[vm->pc];
            } else if (utf8_len == 2) {
                uint8_t bytes[2] = {
                    (uint8_t) code[vm->pc],
                    (uint8_t) code[vm->pc+1]
                };

                lit = (uint64_t) uint8_2_to_uint16(bytes);
            } else if (utf8_len == 3) {
                uint8_t bytes[3] = {
                    (uint8_t) code[vm->pc],
                    (uint8_t) code[vm->pc+1],
                    (uint8_t) code[vm->pc+2]
                };

                lit = (uint64_t) uint8_3_to_uint24(bytes);
            } else if (utf8_len == 4) {
                uint8_t bytes[4] = {
                    (uint8_t) code[vm->pc],
                    (uint8_t) code[vm->pc+1],
                    (uint8_t) code[vm->pc+2],
                    (uint8_t) code[vm->pc+3]
                };

                lit = (uint64_t) uint8_4_to_uint32(bytes);
            }

            vm->pc += utf8_len;
        } else if (code[vm->pc] == '\'') {
            lit = 0;
        } else if (code[vm->pc] == '\\') {
            // imagining it was "\n'"
            vm->pc++;

            // now code + pc is pointing to "n'"
            // so, code[pc] == 'n'

            switch (code[vm->pc]) {
                case 'n': lit = '\n'; break;
                case '0': lit = '\0'; break;
                case 'e': lit = '\x1b'; break;
                case 't': lit = '\t'; break;
                case 'b': lit = '\b'; break;
                case '\'': lit = '\''; break;
                default: printf_error_vm(vm, "Unknown symbol after '\\': '%c'", code[vm->pc]);
            }

            vm->pc++;

            // now it's pointing to "'"
        }

        // We must check if the quote is closed

        if (code[vm->pc] != '\'') {
            *status = STATUS_EXPECTED_QUOTE_CLOSE;
            return 0;
        }
        
        vm->pc++;

        *status = STATUS_SUCCESS;
        return lit;
    } else if (isdigit(code[vm->pc])) {
        lit = (uint64_t) strtoull(code + vm->pc, &endptr, 10);
    } else {
        *status = STATUS_UNKNOWN_LIT;
        return 0;
    }
     
    vm->pc += endptr - (code + vm->pc); // if we didn't use strtoull(), endptr is equal to code + pc, so it's basically pc += 0
    
    *status = STATUS_SUCCESS;
    return lit;
}

static uint64_t parse_val(ShortVM *vm, Status *status) {
    const char *code = vm->code;
    if (code[vm->pc] == 'v' || code[vm->pc] == 'b') {
        char type = code[vm->pc];
        vm->pc++;

        uint64_t num = parse_num(vm, status);
        if (*status != STATUS_SUCCESS) return 0;

        if (type == 'v') return getvar_mem(num, vm);
        else return getbyte_mem(num, vm);
    } else if (code[vm->pc] == '[' || code[vm->pc] == '{' || code[vm->pc] == '<') {
        char type = code[vm->pc];
        uint64_t addr;

        vm->pc++;

        if (type != '<') {
            addr = parse_val(vm, status);
            if (*status != STATUS_SUCCESS) return 0;
            
            if ((type == '[' && code[vm->pc] != ']') ||
                (type == '{' && code[vm->pc] != '}')) {
                *status = STATUS_EXPECTED_CLOSING_BRACKET;
                return 0;
            }

            vm->pc++;
        } else {
            char var_type = code[vm->pc];
            if (code[vm->pc] != 'v' && code[vm->pc] != 'b') {
                *status = STATUS_EXPECTED_VAR;
                return 0;
            }

            vm->pc++;

            uint64_t num = parse_num(vm, status);

            if (*status != STATUS_SUCCESS) return 0;

            if (code[vm->pc] != '>') {
                *status = STATUS_EXPECTED_CLOSING_BRACKET;
                return 0;
            }

            vm->pc++;

            if (var_type == 'v') return num * 8;
            else return num;
        }

        if (type == '[') return getbyte_mem(addr, vm);
        else if (type == '{') return getvar_mem(addr, vm);
        else return 0;
    } else if (code[vm->pc] == '\'' || isdigit(code[vm->pc])) {
        uint64_t lit = parse_lit(vm, status);

        if (*status != STATUS_SUCCESS) {
            return 0;
        }

        return lit;
    } else {
        *status = STATUS_EXPECTED_VAL;
        return 0;
    }
}

void parse_labels(ShortVM *vm, Status *status) {
    const char *code = vm->code;
    for (vm->pc = 0; code[vm->pc] != '\0';) {
        if (code[vm->pc] == 'j') {
            vm->pc++;
            
            while (code[vm->pc] == ' ') vm->pc++;

            // printf("Now code[pc] is: %c\n", code[pc]);
            if (code[vm->pc] == 'l') vm->pc++;
        } else if (code[vm->pc] == 'i') {
            vm->pc++;

            while (code[vm->pc] == ' ' || code[vm->pc] == '=' || code[vm->pc] == '!' || code[vm->pc] == '>' || code[vm->pc] == '<') {
                vm->pc++;
            }

            if (code[vm->pc] == 'l') vm->pc++;
        }
        else if (code[vm->pc] == 'l') {
            vm->pc++;
            // printf("After 'l' PC is %zu at '%c'\n", vm->pc, code[vm->pc]);
            uint64_t num = parse_num(vm, status);
            if (*status != STATUS_SUCCESS) {
                printf("Error!\n");
                break;
            }

            while (code[vm->pc] == ' ') vm->pc++;
            // printf("After spaces PC is %zu at '%c'\n", vm->pc, code[vm->pc]);

            if (!is_endline(code[vm->pc])) {
                *status = STATUS_EXPECTED_ENDLINE;
                break;
            }

            if (code[vm->pc] != '\0') vm->pc++;
            // printf("After endline PC is %zu at '%c'\n", vm->pc, code[vm->pc]);

            set_label_addr(num, vm->pc, vm);
            printf("l%zu was set at %zu\n", num, vm->pc);
        } else {
            vm->pc++;
        }
    }
    
    vm->pc = 0;
    return;
}

Node parse(ShortVM *vm, Status *status) {
    Node node;
    const char *code = vm->code;

    while (code[vm->pc] == ' ' || code[vm->pc] == '\n' || code[vm->pc] == '\t') vm->pc++;

    if (code[vm->pc] == 'v' || code[vm->pc] == 'b') {
        char type = code[vm->pc];

        vm->pc++;

        uint64_t var = parse_num(vm, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        char op = parse_op(vm);

        if (!op) {
            *status = STATUS_EXPECTED_OP;
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        uint64_t val = parse_val(vm, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        if (!is_endline(code[vm->pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (type == 'v') {
            node.type = NODE_ASSIGN_VAR;
            node.value.assign_var.var = var;
            node.value.assign_var.op = op;
            node.value.assign_var.val = val;
        } else {
            node.type = NODE_ASSIGN_BYTE;
            node.value.assign_byte.var = var;
            node.value.assign_byte.op = op;
            node.value.assign_byte.val = val;
        }
        if (code[vm->pc] != '\0') vm->pc++;
    } else if (code[vm->pc] == 'w' || code[vm->pc] == 'r') {
        char cmd = code[vm->pc];

        vm->pc++;

        while (code[vm->pc] == ' ') vm->pc++;

        uint64_t arg = parse_val(vm, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        if (!is_endline(code[vm->pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[vm->pc] != '\0') vm->pc++;

        node.type = NODE_COMMAND;
        node.value.command.cmd = cmd;
        node.value.command.arg = arg;
    } else if (code[vm->pc] == 'j') {
        // imagining the line is "j l42"
        vm->pc++;

        // now it's " l42"

        while (code[vm->pc] == ' ') vm->pc++;

        // now just "l42"

        if (code[vm->pc] != 'l') {
            *status = STATUS_EXPECTED_LABEL;
            return node;
        }

        vm->pc++;

        // now only a number left - "42"

        uint64_t num = parse_num(vm, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        if (!is_endline(code[vm->pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[vm->pc] != '\0') vm->pc++;

        node.type = NODE_COMMAND;
        node.value.command.cmd = 'j';
        node.value.command.arg = num;
    } else if (code[vm->pc] == 'l') {
        vm->pc++;
        uint64_t num = parse_num(vm, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        if (!is_endline(code[vm->pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[vm->pc] != '\0') vm->pc++;
        
        node.type = NODE_LABEL;
    } else if (code[vm->pc] == 'i') {
        vm->pc++;

        while (code[vm->pc] == ' ') vm->pc++;

        char *op = parse_cond_op(vm);

        if (*op == '\0') {
            *status = STATUS_EXPECTED_OP;
            return node;
        }

        while (code[vm->pc] == ' ') vm->pc++;

        if (code[vm->pc] != 'l') {
            *status = STATUS_EXPECTED_LABEL;
            return node;
        }

        vm->pc++;

        uint64_t num = parse_num(vm, status);

        if (*status != STATUS_SUCCESS) return node;

        while (code[vm->pc] == ' ') vm->pc++;

        if (!is_endline(code[vm->pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        node.type = NODE_CONDJMP;
        node.value.condjmp.op = op;
        node.value.condjmp.label = num;
    } else if (code[vm->pc] == '(') {
        while (code[vm->pc] != ')' && code[vm->pc] != '\0') vm->pc++;

        if (code[vm->pc] == '\0') {
            *status = STATUS_EXPECTED_COMMENT_END;
            return node;
        }

        vm->pc++;
    }
    else if (code[vm->pc] == '\0') {
        *status = STATUS_CONTINUE;
        return node;
    }
    else {
        *status = STATUS_UNEXPECTED_SYMBOL;
        // printf("The wrong symbol is: %c\n", code[pc]);
        return node;
    }

    *status = STATUS_SUCCESS;
    return node;
}
