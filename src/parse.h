#ifndef PARSE_H

#define PARSE_H

#include <stdint.h>

typedef enum {
    STATUS_SUCCESS,

    STATUS_CONTINUE,

    STATUS_EXPECTED_NUM,
    STATUS_EXPECTED_OP,
    STATUS_EXPECTED_VAL,
    STATUS_EXPECTED_LIT_CONT,
    STATUS_EXPECTED_QUOTE,
    STATUS_EXPECTED_ENDLINE,
    STATUS_EXPECTED_LABEL,

    STATUS_UNEXPECTED_SYMBOL,

    STATUS_UNKNOWN_LIT,

    STATUS_DIVISION_BY_ZERO
} Status;

typedef enum {
    NODE_ASSIGN_VAR,
    NODE_ASSIGN_BYTE,
    NODE_COMMAND,
    NODE_LABEL
} NodeType;

typedef struct {
    uint64_t var;
    uint64_t val;
    char op;
} AssignVarNode;

typedef struct {
    uint64_t var;
    uint8_t val;
    char op;
} AssignByteNode;

typedef struct {
    uint64_t op;
    uint64_t num;
} CondJumpNode;

typedef struct {
    char cmd;
    uint64_t arg;
} CommandNode;

typedef struct {
    uint64_t addr;
    uint64_t num;
} LabelNode;

typedef struct {
    NodeType type;
    union {
        AssignVarNode assign_var;
        AssignByteNode assign_byte;
        CommandNode command;
        LabelNode label;
        CondJumpNode cond_jump;
    } value;
} Node;

void parse_labels(const char *code, uint64_t *label_memory, Status *status);
Node parse(const char *code, Status *status);

#endif
