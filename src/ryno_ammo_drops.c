/* SPDX-License-Identifier: MIT
 * RYNO Ammo Drops for OpenRAC RAC1, experimental v0.1.0.
 * No assets or game binaries are included.
 *
 * Original game's random ammo crates only choose from the current level's
 * Gadgetron vendor stock. This hook calls the original selector first, then
 * lets owned RYNO (item 8) compete for RANDOM AMMO CRATE drops under the same
 * low-ammo priority; assigned/fixed crates and enemy drops are unchanged.
 */
#include <stdint.h>
#include <string.h>
#include "openrac/openrac_mod.h"
#include "openrac/games/rac1.h"
#include "priority.h"

OPENRAC_MOD_DEFINE("ryno-ammo-drops", "0.1.0")

/* Some C SDKs include this hook only in the C++ header; its stable name and
 * signature are listed in sdk/bindings/rac1.json on both PAL and NTSC-U. */
#ifndef OPENRAC_RAC1_HOOK_Ammo_ChooseDrop
#define OPENRAC_RAC1_HOOK_Ammo_ChooseDrop "Ammo_ChooseDrop"
#define OPENRAC_RAC1_SIG_Ammo_ChooseDrop "i32(u32,u32)"
typedef int32_t (*openrac_rac1_Ammo_ChooseDrop)(openrac_gaddr moby, openrac_gaddr out_item);
#endif

#define RYNO_ITEM 8u
#define AMMO_CRATE_CLASS 0x01FFu
#define MOBY_CLASS_OFFSET 0xA6u
#define VENDOR_STOCK_SIZE 12u
#define ITEM_INFO_SIZE 0x18u
#define ITEM_MAX_AMMO_OFFSET 0xEu
#define ITEM_COUNT 37u

/* PAL v2.00 and NTSC-U from rac1-decomp and OpenRAC mod examples.
 * All global addresses are game addresses: relocate_data for the executable,
 * relocate_level_data(0) for overlay 0's item info table. */
typedef struct disc_addresses {
    const char* version;
    uint32_t owned;
    uint32_t ammo;
    uint32_t vendor_stock;
    uint32_t item_info;
} disc_addresses;
static const disc_addresses k_discs[] = {
    {"pal",  0x0013D5C8u, 0x0013D530u, 0x0015EED0u, 0x001C43B0u},
    {"ntsc", 0x0013D4C0u, 0x0013D428u, 0x0015EDD0u, 0x001C40B0u}
};

static const openrac_api* s_api;
static const disc_addresses* s_disc;
static openrac_rac1_Ammo_ChooseDrop s_original;
static uint32_t s_random_state = 0xC3C32D27u;

static uint8_t* global_at(uint32_t address) {
    return s_api->memory + s_api->relocate_data(address);
}
static uint8_t* item_table(void) {
    return s_api->memory + s_api->relocate_level_data(0, s_disc->item_info);
}
static int32_t load_s32(const uint8_t* p) {
    int32_t x;
    memcpy(&x, p, sizeof x);
    return x;
}
static uint16_t load_u16(const uint8_t* p) {
    uint16_t x;
    memcpy(&x, p, sizeof x);
    return x;
}
static void save_s32(uint8_t* p, int32_t x) {
    memcpy(p, &x, sizeof x);
}
static uint16_t max_ammo(unsigned item) {
    return load_u16(item_table() + item * ITEM_INFO_SIZE + ITEM_MAX_AMMO_OFFSET);
}
static int32_t current_ammo(unsigned item) {
    return load_s32(global_at(s_disc->ammo) + 4u * item);
}

/* This RNG is mod-local: don't disturb the game's random sequence, so other
 * weapon drops and mods don't become dependent on this mod's random draw. */
static uint32_t next_random(void) {
    uint32_t x = s_random_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_random_state = x;
    return x;
}

static int32_t choose_drop(openrac_gaddr moby, openrac_gaddr out_item) {
    /* Record the caller's choice before allowing the game and earlier mods
     * in the hook chain to handle it; positive values are assigned drops. */
    const int32_t initial = out_item ? load_s32(s_api->memory + out_item) : 0;
    const int32_t drops = s_original(moby, out_item);

    if (!moby || !out_item || initial > 0 || drops <= 0) {
        return drops;
    }
    if (load_u16(s_api->memory + moby + MOBY_CLASS_OFFSET) != AMMO_CRATE_CLASS) {
        return drops; /* Never change defeated enemies or other mobys. */
    }

    const uint8_t* owned = global_at(s_disc->owned);
    if (!owned[RYNO_ITEM] || max_ammo(RYNO_ITEM) == 0 ||
        current_ammo(RYNO_ITEM) >= max_ammo(RYNO_ITEM)) {
        return drops; /* Not bought, or already full: normal selection. */
    }

    /* Count the underfull normal ammo weapons the retail chooser could pick.
     * The retail code ignores stock entries after 0xFF and only examines 12.
     * Only items already owned count; empty/non-ammo items don't. */
    const uint8_t* stock = global_at(s_disc->vendor_stock);
    unsigned others = 0;
    for (unsigned i = 0; i < VENDOR_STOCK_SIZE && stock[i] != 0xFFu; ++i) {
        const unsigned item = stock[i] & 0x3Fu;
        if (item == RYNO_ITEM) {
            return drops; /* Another mod already has RYNO in the selector. */
        }
        if (item < ITEM_COUNT && owned[item] && max_ammo(item) != 0 &&
            current_ammo(item) < max_ammo(item)) {
            ++others;
        }
    }

    /* Allow each underfull weapon, RYNO included, one equal chance. Keep the
     * original chosen item on every other roll, and keep the original count. */
    if (ryno_should_replace(1, others, next_random())) {
        save_s32(s_api->memory + out_item, (int32_t)RYNO_ITEM);
    }
    return drops;
}

OPENRAC_MOD_EXPORT int openrac_mod_load(const openrac_api* api, openrac_mod* mod) {
    s_api = api;
    s_disc = NULL;
    s_original = NULL;
    if (strcmp(api->game, "rac1") != 0) {
        api->fail(mod, "RYNO Ammo Drops is for Ratchet & Clank 1 only");
        return 1;
    }
    for (unsigned i = 0; i < sizeof k_discs / sizeof k_discs[0]; ++i) {
        if (strcmp(api->version, k_discs[i].version) == 0) {
            s_disc = &k_discs[i];
            break;
        }
    }
    if (!s_disc) {
        api->fail(mod, "RYNO Ammo Drops only supports RAC1 PAL and NTSC-U");
        return 1;
    }
    const int result = api->hook(mod, OPENRAC_RAC1_HOOK_Ammo_ChooseDrop,
                                OPENRAC_RAC1_SIG_Ammo_ChooseDrop, (void*)choose_drop,
                                (void**)&s_original);
    if (result != OPENRAC_HOOK_OK || !s_original) {
        api->fail(mod, "OpenRAC did not provide the RAC1 ammo selection hook");
        return 1;
    }
    api->log(mod, OPENRAC_LOG_INFO, "RYNO ammo is now eligible in random ammo crates after purchase");
    return 0;
}
