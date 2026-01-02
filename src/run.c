#include <stdio.h>
#include <string.h>

#include "run.h"
#include "debug.h"
#include "parse.h"
#include "mem.h"

#ifdef _WIN32
    #include <conio.h>
    #define inst_getchar() _getch()
#else
    #include <termios.h>
    #include <unistd.h>

    static int inited = 0;
    
    static inline int inst_getchar() {
        if (!inited) {
            struct termios t;
            tcgetattr(STDIN_FILENO, &t);
            t.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &t);
            inited = 1;
        } return getchar();
    }

#endif

static inline void putuint32(uint32_t num) {
    uint8_t bytes[4] = {
        (uint8_t)((num >> 16) & 0xFF),
        (uint8_t)((num >> 8) & 0xFF),
        (uint8_t)(num & 0xFF)
    };

    fwrite(bytes, 1, 4, stdout);
}

static void print_flags(ShortVM *vm) {
    printf("Equal: %d\n", vm->fl_equal);
    printf("Not equal: %d\n", vm->fl_not_equal);
    printf("Bigger: %d\n", vm->fl_bigger);
    printf("Less: %d\n", vm->fl_less);
    printf("Equals/Bigger: %d\n", vm->fl_eq_bigger);
    printf("Equals/Less: %d\n", vm->fl_less);
}

/*
    STATUS_SUCCESS,

    STATUS_CONTINUE,

    STATUS_EXPECTED_NUM,
    STATUS_EXPECTED_OP,
    STATUS_EXPECTED_VAL,
    STATUS_EXPECTED_LIT_CONT,
    STATUS_EXPECTED_QUOTE,
    STATUS_EXPECTED_QUOTE_CLOSE,
    STATUS_EXPECTED_ENDLINE,
    STATUS_EXPECTED_LABEL,
    STATUS_EXPECTED_CLOSING_BRACKET,

    STATUS_UNEXPECTED_SYMBOL,

    STATUS_UNKNOWN_LIT,

    STATUS_DIVISION_BY_ZERO
*/


static char* status_to_str(Status status) {
    switch (status) {
        case STATUS_SUCCESS: return "";
        case STATUS_EXPECTED_NUM: return "Expected number";
        case STATUS_EXPECTED_OP: return "Expected op";
        case STATUS_EXPECTED_VAL: return "Expected value";
        case STATUS_EXPECTED_VAR: return "Expected variable";
        case STATUS_EXPECTED_LIT_CONT: return "Expected literal to continue";
        case STATUS_EXPECTED_QUOTE: return "Expected a quote";
        case STATUS_EXPECTED_QUOTE_CLOSE: return "Expected a quote to close";
        case STATUS_EXPECTED_ENDLINE: return "Expected the end of line";
        case STATUS_EXPECTED_LABEL: return "Expected label";
        case STATUS_EXPECTED_CLOSING_BRACKET: return "Expected closing bracket";
        case STATUS_EXPECTED_COMMENT_END: return "Expected comment end";

        case STATUS_UNEXPECTED_SYMBOL: return "Unexpected symbol";

        case STATUS_UNKNOWN_LIT: return "Unknown literal";

        case STATUS_INVALID_SYMBOL: return "Invalid symbol";
        
        case STATUS_DIVISION_BY_ZERO: return "Division by zero";
        default: return "[Unknown error]";
    }
}

static void run_node(Node *node, ShortVM *vm) {
    if (node->type == NODE_ASSIGN_VAR) {
        uint64_t var = node->value.assign_var.var;
        uint64_t var_val = getvar_mem(node->value.assign_var.var, vm);
        uint64_t val = node->value.assign_var.val;
        switch (node->value.assign_var.op) {
            case '=': setvar_mem(var, val, vm); break;
            case '+': addvar_mem(var, val, vm); break;
            case '-': subvar_mem(var, val, vm); break;
            case '*': mulvar_mem(var, val, vm); break;
            case '/': divvar_mem(var, val, vm); break;
            case '%': remvar_mem(var, val, vm); break;
            case '^': xorvar_mem(var, val, vm); break;
            case '&': andvar_mem(var, val, vm); break;
            case '|': orvar_mem(var, val, vm); break;
            case '>': rightvar_mem(var, val, vm); break;
            case '<': leftvar_mem(var, val, vm); break;
            case '?':
                vm->fl_equal = var_val == val;
                vm->fl_not_equal= !vm->fl_equal;
                vm->fl_bigger = var_val > val;
                vm->fl_less = var_val < val;
                vm->fl_eq_bigger = var_val >= val;
                vm->fl_eq_less = var_val <= val;
                break;
        }
    } else if (node->type == NODE_ASSIGN_BYTE) {
        uint8_t var = node->value.assign_byte.var;
        uint8_t var_val = getbyte_mem(node->value.assign_byte.var, vm);
        uint8_t val = node->value.assign_byte.val;
        switch (node->value.assign_byte.op) {
            case '=': setbyte_mem(var, val, vm); break;
            case '+': addbyte_mem(var, val, vm); break;
            case '-': subbyte_mem(var, val, vm); break;
            case '*': mulbyte_mem(var, val, vm); break;
            case '/': divbyte_mem(var, val, vm); break;
            case '%': rembyte_mem(var, val, vm); break;
            case '^': xorbyte_mem(var, val, vm); break;
            case '&': andbyte_mem(var, val, vm); break;
            case '|': orbyte_mem(var, val, vm); break;
            case '>': rightbyte_mem(var, val, vm); break;
            case '<': leftbyte_mem(var, val, vm); break;
            case '?':
                vm->fl_equal = var_val == val;
                vm->fl_not_equal = !vm->fl_equal;
                vm->fl_bigger = var_val > val;
                vm->fl_less = var_val < val;
                vm->fl_eq_bigger = var_val >= val;
                vm->fl_eq_less = var_val <= val;
                break;
        }
    } else if (node->type == NODE_COMMAND) {
        char c;
        switch (node->value.command.cmd) {
            case 'w':
                putuint32(node->value.command.arg);
                break;
            case 'r':
                c = inst_getchar();
                setbyte_mem(node->value.command.arg, c, vm);
                break;
            case 'j':
                vm->pc = get_label_addr(node->value.command.arg, vm);
                // printf("Jumping to %zu from l%zu\n", pc, node->value.command.arg);
                break;
            default:
                printf_error_vm(vm, "Command is unsupported yet: '%c'", node->value.command.cmd);
        }
    } else if (node->type == NODE_LABEL) {
        // NOTHING
    } else if (node->type == NODE_CONDJMP) {
        char *op = node->value.condjmp.op;
        uint64_t new_pc = get_label_addr(node->value.condjmp.label, vm);

        // print_flags(vm);

        if (!strcmp(op, "==")) {
            if (vm->fl_equal) vm->pc = new_pc;
        } else if (!strcmp(op, "!")) {
            if (vm->fl_not_equal) vm->pc = new_pc;
        } else if (!strcmp(op, ">")) {
            if (vm->fl_bigger) vm->pc = new_pc;
        } else if (!strcmp(op, "<")) {
            if (vm->fl_less) vm->pc = new_pc;
        } else if (!strcmp(op, ">=")) {
            if (vm->fl_eq_bigger) vm->pc = new_pc;
        } else if (!strcmp(op, "<=")) {
            if (vm->fl_eq_less) vm->pc = new_pc;
        }
    }
}

void run_code(ShortVM *vm) {
    if (!vm->code) printf_error_vm(vm, "Failed to get the code while running");

    Status status;
    parse_labels(vm, &status);
    scanlabels(100, vm);

    for (; vm->code[vm->pc] != '\0' ;) {
        Node node = parse(vm, &status);
        if (status != STATUS_SUCCESS) {
            if (status == STATUS_CONTINUE) continue;
            if (vm->code[vm->pc] == '\0') {
                printf_error_vm(vm, "%s at the end of file (PC: %ld)", status_to_str(status), vm->pc);
            } else {
                printf_error_vm(vm, "%s at '%c' (ASCII: %d, PC: %ld)", status_to_str(status), vm->code[vm->pc], vm->code[vm->pc], vm->pc);
            }
            return;
        }
        
        run_node(&node, vm);
    }
}
