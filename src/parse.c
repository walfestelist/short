#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "parse.h"
#include "debug.h"
#include "mem.h"
#include "utils.h"

extern uint64_t pc;

static inline int is_endline(char c) {
    return c == '.' || c == '\n' || c == '\0';
}

static inline uint64_t parse_num(const char *code, Status *status) {
    char *endptr;
    uint64_t num = (uint64_t) strtoull(code + pc, &endptr, 10);

    if (endptr == code + pc) {
        *status = STATUS_EXPECTED_NUM;
        return 0;
    }

    pc += endptr - (code + pc);

    *status = STATUS_SUCCESS;
    return num;
}

static inline char parse_op(const char *code) {
    if (code[pc] == '+' || code[pc] == '-' || code[pc] == '*' || code[pc] == '/' || code[pc] == '=' || code[pc] == '%' || code[pc] == '^' || code[pc] == '&' || code[pc] == '|') {
        return code[pc++];
    } else if (code[pc] != '\0') {
        if (code[pc] == '>' && code[pc+1] == '>') {
            pc += 2;
            return '>';
        } else if (code[pc] == '<' && code[pc+1] == '<') {
            pc += 2;
            return '<';
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static inline uint64_t parse_lit(const char *code, Status *status) {
    char *endptr = (char*) code + pc;
    uint64_t lit = 0;

    if (code[pc] == '0' && code[pc+1] == 'x') {
        pc += 2; // '0x' - 2 symbols
        // if it was "0x2A", then now code + pc is pointing to "2A"

        if (!isdigit_in_hex(code[pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + pc, &endptr, 16); // hex
    } else if (code[pc] == '0' && code[pc+1] == 'o') {
        pc += 2;
        // if it was "0o52", then now code + pc is pointing to "52"
        
        if (!isdigit_in_oct(code[pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + pc, &endptr, 8); // octal
    } else if (code[pc] == '0' && code[pc+1] == 'b') {
        pc += 2;
        // if it was "0b101010", then now code + pc is pointing to "101010"

        if (!isdigit_in_bin(code[pc])) {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        lit = (uint64_t) strtoull(code + pc, &endptr, 2); // binary
    } else if (code[pc] == '\'') {
        pc++;
        // If it was "'A'", then now code + pc is pointing to "A'"

        if (code[pc] == '\0') {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }

        if (code[pc] != '\'' && code[pc] != '\\') {
            lit = (uint64_t) code[pc]; // we just take the ASCII code of the current symbol, like 'A'
            pc++;
        } else if (code[pc] == '\'') {
            lit = 0;
        } else if (code[pc] == '\\') {
            // imagining it was "\n'"
            pc++;

            // now code + pc is pointing to "n'"
            // so, code[pc] == 'n'

            switch (code[pc]) {
                case 'n': lit = '\n'; break;
                case '0': lit = '\0'; break;
                case 'e': lit = '\x1b'; break;
                case 't': lit = '\t'; break;
                case 'b': lit = '\b'; break;
                case '\'': lit = '\''; break;
                default: printf_error("Unknown symbol after '\\': '%c'", code[pc]);
            }

            pc++;

            // now it's pointing to "'"
        }

        // We must check if the quote is closed

        if (code[pc] != '\'') {
            *status = STATUS_EXPECTED_LIT_CONT;
            return 0;
        }
        
        pc++;

        *status = STATUS_SUCCESS;
        return lit;
    } else if (isdigit(code[pc])) {
        lit = (uint64_t) strtoull(code + pc, &endptr, 10);
    } else {
        *status = STATUS_UNKNOWN_LIT;
        return 0;
    }
     
    pc += endptr - (code + pc); // if we didn't use strtoull(), endptr is equal to code, so it's basically pc += 0
    
    *status = STATUS_SUCCESS;
    return lit;
}

static inline uint64_t parse_val(const char *code, Status *status) {
    if (code[pc] == 'v' || code[pc] == 'b') {
        char type = code[pc];
        pc++;

        uint64_t num = parse_num(code, status);
        if (*status != STATUS_SUCCESS) {
            return 0;
        }

        if (type == 'v') return getvar_mem(num);
        else return getbyte_mem(num);
    } else {
        uint64_t lit = parse_lit(code, status);

        if (*status != STATUS_SUCCESS) {
            return 0;
        }

        return lit;
    }
}

void parse_labels(const char *code, uint64_t *label_memory, Status *status) {
    for (pc = 0; code[pc] != '\0'; pc++) {
        if (code[pc] == 'j') {
            pc++;
            
            while (pc == ' ') pc++;

            if (code[pc] == 'l') pc++;
        } else if (code[pc] == 'l') {
            pc++;
            uint64_t num = parse_num(code, status);
            if (*status != STATUS_SUCCESS) {
                break;
            }

            while (code[pc] == ' ') pc++;

            if (!is_endline(code[pc])) {
                *status = STATUS_EXPECTED_ENDLINE;
                break;
            }

            if (code[pc] != '\0') pc++;

            set_label_addr(num, pc);
            printf("Label %ld was set at %zu", num, pc);
        }
    }
    
    pc = 0;
    return;
}

Node parse(const char *code, Status *status) {
    Node node;

    while (code[pc] == ' ' || code[pc] == '\n' || code[pc] == '\t') pc++;

    if (code[pc] == 'v' || code[pc] == 'b') {
        char type = code[pc];

        pc++;

        uint64_t var = parse_num(code, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[pc] == ' ') pc++;

        char op = parse_op(code);

        if (!op) {
            *status = STATUS_EXPECTED_OP;
            return node;
        }

        while (code[pc] == ' ') pc++;

        uint64_t val = parse_val(code, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[pc] == ' ') pc++;

        if (!is_endline(code[pc])) {
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
        if (code[pc] != '\0') pc++;
    } else if (code[pc] == 'w' || code[pc] == 'r') {
        char cmd = code[pc];

        pc++;

        while (code[pc] == ' ') pc++;

        uint64_t arg = parse_val(code, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[pc] == ' ') pc++;

        if (!is_endline(code[pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[pc] != '\0') pc++;

        node.type = NODE_COMMAND;
        node.value.command.cmd = cmd;
        node.value.command.arg = arg;
    } else if (code[pc] == 'j') {
        // imagining the line is "j l42"
        pc++;

        // now it's " l42"

        while (code[pc] == ' ') pc++;

        // now just "l42"

        if (code[pc] != 'l') {
            *status = STATUS_EXPECTED_LABEL;
            return node;
        }

        pc++;

        // now only a number left - "42"

        uint64_t num = parse_num(code, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        if (!is_endline(code[pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[pc] != '\0') pc++;

        node.type = NODE_COMMAND;
        node.value.command.cmd = 'j';
        node.value.command.arg = num;
    } else if (code[pc] == '\0') {
        *status = STATUS_CONTINUE;
        return node;
    } else if (code[pc] == 'l') {
        pc++;
        uint64_t num = parse_num(code, status);

        if (*status != STATUS_SUCCESS) {
            return node;
        }

        while (code[pc] == ' ') pc++;

        if (!is_endline(code[pc])) {
            *status = STATUS_EXPECTED_ENDLINE;
            return node;
        }

        if (code[pc] != '\0') pc++;
        
        node.type = NODE_LABEL;
    }

    else {
        *status = STATUS_UNEXPECTED_SYMBOL;
        // printf("The wrong symbol is: %c\n", code[pc]);
        return node;
    }

    *status = STATUS_SUCCESS;
    return node;
}
