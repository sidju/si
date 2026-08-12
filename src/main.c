#include <stdio.h>

#include "si/runtime.h"

int main(void) {
    const char *script = "1 2 add print\n";
    int rc = si_execute_script(script, stdout, stderr);
    return rc == 0 ? 0 : 1;
}
