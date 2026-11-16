#include <assert.h>
#include <stdio.h>
#include "priority.h"
int main(void) {
    /* Protect against confusing GadgetId 8 with GiveItem/ammo-table id 0x17. */
    assert(RYNO_ITEM_ID == 23u);
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
