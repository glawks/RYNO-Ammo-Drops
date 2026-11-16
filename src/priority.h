/* SPDX-License-Identifier: MIT
 * Decision helper for a RYNO pickup competing with the normal low-ammo pool. */
#ifndef RYNO_AMMO_PRIORITY_H
#define RYNO_AMMO_PRIORITY_H

/* RAC1's inventory/ammo-table index (not GadgetId, where RYNO is 8). */
#define RYNO_ITEM_ID 0x17u
#include <stdint.h>

/* On a random ammo-crate drop, if RYNO is underfull, give it one equal-weight
 * position alongside each other underfull eligible weapon. */
static inline int ryno_should_replace(int ryno_underfull, unsigned others_underfull,
                                      uint32_t random_word) {
    return ryno_underfull && (random_word % (others_underfull + 1u) == 0u);
}
#endif
