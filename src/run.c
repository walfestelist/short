#include <stdio.h>
#include <string.h>

#include "run.h"
#include "debug.h"
#include "parse.h"
#include "mem.h"

uint64_t pc = 0;
extern uint64_t *label_memory;

char
    flag_equal = 0,
    flag_noteq = 0,
    flag_bigger = 0,
    flag_less = 0,
    flag_eq_bigger = 0,
    flag_eq_less = 0
;

static void print_flags() {
    printf("Equal: %d\n", flag_equal);
    printf("Not equal: %d\n", flag_noteq);
    printf("Bigger: %d\n", flag_bigger);
    printf("Less: %d\n", flag_bigger);
    printf("Equals/Bigger: %d\n", flag_eq_bigger);
    printf("Equals/Less: %d\n", flag_eq_less);
}

static char* status_to_str(Status status) {
    switch (status) {
        case STATUS_SUCCESS: return "";
        case STATUS_EXPECTED_NUM: return "Expected variable number";
        case STATUS_EXPECTED_OP: return "Expected op";
        case STATUS_EXPECTED_VAL: return "Expected value";
        case STATUS_EXPECTED_LIT_CONT: return "Expected literal to continue";
        case STATUS_EXPECTED_QUOTE: return "Expected a quote";
        case STATUS_EXPECTED_ENDLINE: return "Expected the end of line";

        case STATUS_UNEXPECTED_SYMBOL: return "Unexpected symbol";

        case STATUS_UNKNOWN_LIT: return "Unknown literal";
        
        case STATUS_DIVISION_BY_ZERO: return "Division by zero";
        default: return "[Unknown error]";
    }
}

static void run_node(Node *node) {
    if (node->type == NODE_ASSIGN_VAR) {
        uint64_t var = node->value.assign_var.var;
        uint64_t var_val = getvar_mem(node->value.assign_var.var);
        uint64_t val = node->value.assign_var.val;
        switch (node->value.assign_var.op) {
            case '=': setvar_mem(var, val); break;
            case '+': addvar_mem(var, val); break;
            case '-': subvar_mem(var, val); break;
            case '*': mulvar_mem(var, val); break;
            case '/': divvar_mem(var, val); break;
            case '%': remvar_mem(var, val); break;
            case '^': xorvar_mem(var, val); break;
            case '&': andvar_mem(var, val); break;
            case '|': orvar_mem(var, val); break;
            case '>': rightvar_mem(var, val); break;
            case '<': leftvar_mem(var, val); break;
            case '?':
                flag_equal = var_val == val;
                flag_noteq = !flag_equal;
                flag_bigger = var_val > val;
                flag_less = var_val < val;
                flag_eq_bigger = var_val >= val;
                flag_eq_less = var_val <= val;
                break;
        }
    } else if (node->type == NODE_ASSIGN_BYTE) {
        uint8_t var = node->value.assign_byte.var;
        uint8_t var_val = getbyte_mem(node->value.assign_byte.var);
        uint8_t val = node->value.assign_byte.val;
        switch (node->value.assign_byte.op) {
            case '=': setbyte_mem(var, val); break;
            case '+': addbyte_mem(var, val); break;
            case '-': subbyte_mem(var, val); break;
            case '*': mulbyte_mem(var, val); break;
            case '/': divbyte_mem(var, val); break;
            case '%': rembyte_mem(var, val); break;
            case '^': xorbyte_mem(var, val); break;
            case '&': andbyte_mem(var, val); break;
            case '|': orbyte_mem(var, val); break;
            case '>': rightbyte_mem(var, val); break;
            case '<': leftbyte_mem(var, val); break;
            case '?':
                flag_equal = var_val == val;
                flag_noteq = !flag_equal;
                flag_bigger = var_val > val;
                flag_less = var_val < val;
                flag_eq_bigger = var_val >= val;
                flag_eq_less = var_val <= val;
                break;
        }
    } else if (node->type == NODE_COMMAND) {
        char c;
        switch (node->value.command.cmd) {
            case 'w':
                putchar(node->value.command.arg);
                break;
            case 'r':
                c = getchar();
                setbyte_mem(node->value.command.arg, c);
                break;
            case 'j':
                pc = get_label_addr(node->value.command.arg);
                // printf("Jumping to %zu from %zu\n", pc, node->value.command.arg);
                break;
            default:
                printf_error("Command is unsupported yet: '%c'", node->value.command.cmd);
        }
    } else if (node->type == NODE_LABEL) {
        // NOTHING
    } else if (node->type == NODE_CONDJMP) {
        char *op = node->value.condjmp.op;
        char new_pc = get_label_addr(node->value.condjmp.label);

        if (!strcmp(op, "==")) {
            if (flag_equal) pc = new_pc;
        } else if (!strcmp(op, "!")) {
            if (flag_noteq) pc = new_pc;
        } else if (!strcmp(op, ">")) {
            if (flag_bigger) pc = new_pc;
        } else if (!strcmp(op, "<")) {
            if (flag_less) pc = new_pc;
        } else if (!strcmp(op, ">=")) {
            if (flag_eq_bigger) pc = new_pc;
        } else if (!strcmp(op, "<=")) {
            if (flag_eq_less) pc = new_pc;
        }
    }
}

Status run_code(const char *code) {
    if (!code) printf_error("Failed to get the code while running");

    Status status;
    parse_labels(code, label_memory, &status);
    // scanlabels(100);

    for (; code[pc] != '\0' ;) {
        Node node = parse(code, &status);
        if (status != STATUS_SUCCESS) {
            if (status == STATUS_CONTINUE) continue;
            printf_error("%s at '%c' (code: %d, pc: %ld)", status_to_str(status), code[pc], code[pc], pc);
            return status;
        }
        
        run_node(&node);
    }

    return STATUS_SUCCESS;
}
