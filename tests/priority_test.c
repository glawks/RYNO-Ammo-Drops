#include <assert.h>
#include <stdio.h>
#include "priority.h"
int main(void) {
    for (unsigned n = 0; n < 13; ++n) {
        unsigned hits = 0;
        for (unsigned r = 0; r < (n + 1) * 100; ++r) {
            hits += ryno_should_replace(1, n, r);
            assert(!ryno_should_replace(0, n, r));
        }
        assert(hits == 100);
    }
    puts("RYNO selection priority tests passed");
    return 0;
}
