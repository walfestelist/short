#include "src/run.h"
#include "src/mem.h"

extern uint64_t pc;

int main(void) {
    init_mem();

    char *code =
        "w 'F'\n"
        "w 'E'\n"
        ;

    run_code(code);
}
