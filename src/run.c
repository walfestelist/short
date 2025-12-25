#include <stdio.h>
#include "run.h"
#include "debug.h"
#include "parse.h"
#include "mem.h"

uint64_t pc = 0;
extern uint64_t *label_memory;

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
        switch (node->value.assign_var.op) {
            case '=': setvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '+': addvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '-': subvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '*': mulvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '/': divvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '%': remvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '^': xorvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '&': andvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '|': orvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '>': rightvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
            case '<': leftvar_mem(node->value.assign_var.var, node->value.assign_var.val); break;
        }
    } else if (node->type == NODE_ASSIGN_BYTE) {
        switch (node->value.assign_byte.op) {
            case '=': setbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '+': addbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '-': subbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '*': mulbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '/': divbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '%': rembyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '^': xorbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '&': andbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '|': orbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '>': rightbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
            case '<': leftbyte_mem(node->value.assign_byte.var, node->value.assign_byte.val); break;
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
    }
}

Status run_code(const char *code) {
    if (!code) printf_error("Failed to get the code while running");

    Status status;
    parse_labels(code, label_memory, &status);

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
