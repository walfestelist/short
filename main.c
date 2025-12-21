#include "src/run.h"
#include "src/mem.h"

extern uint64_t pc;

int main(void) {
    init_mem();

    char *code =
        "v1 = 42\n"
        "v1 >> 3\n"
        "w v1\n"
        ;

    run_code(code);

    scanvars_mem(10);
}
