#include "mem.h"
#include "vm.h"

ShortVM init_vm(const char *code) {
    ShortVM vm;

    vm.code = code;
    vm.pc = 0;
    
    init_mem(&vm);

    // Reset flags
    vm.fl_equal = 0;
    vm.fl_not_equal = 0;
    vm.fl_bigger = 0;
    vm.fl_less = 0;
    vm.fl_eq_bigger = 0;
    vm.fl_eq_less = 0;

    return vm;
}
