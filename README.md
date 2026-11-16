# RYNO Ammo Drops — OpenRAC RAC1

**Experimental v0.1.2 — test builds on `dev`; not yet verified in game.**

Adds RYNO ammunition to the random ammo crate selection after you own the RYNO.
RYNO joins the low-ammo priority pool with equal weight to other eligible weapons.
It is not selected by the mod when full. Fixed ammo crates, defeated enemies,
and the number of pickups remain unchanged. No game assets or save changes.

Supports both PAL and NTSC-U.

## Download automatic compiled builds

1. Open [Build and test](https://github.com/glawks/RYNO-Ammo-Drops/actions/workflows/build.yml).
2. Choose the newest **successful** workflow run on `dev`.
3. Under **Artifacts**, download **RYNO-Ammo-Drops-Compiled**.
4. Extract the GitHub artifact ZIP, then install the inner
   `ryno-ammo-drops-0.1.2.zip` with OpenRAC.

Individual Windows, Linux, and macOS compiled archives are also available as
separate artifacts. Every push to `dev` starts a new build. Public releases
are created only when a version tag is pushed after testing.

## v0.1.2 fix and debugging

Fixes a wrong inventory ID in the previous versions. Earlier builds read item 8
(the Wrench) rather than item 23 (RYNO). This incorrectly reported RYNO
as not owned and showed 0/0 ammunition, preventing even forced test drops.


If RYNO ammo is not appearing, enable **Detailed log** in the mod's settings
and break a few ammo crates. The OpenRAC log now reports whether they are random
or assigned, the RYNO ownership flag, current/maximum RYNO ammo, how many
other weapons are eligible, and whether the random roll selected RYNO.

To eliminate unlucky drop weighting during testing, enable
**Force RYNO drops (testing)** as well. This applies only to random ammo crates
when you own RYNO and its ammunition is not full. It is disabled by default.
Disable it again after you confirm RYNO pickups work. These settings do not
alter your save.

## Test in game

1. Before buying the RYNO, break random ammo crates: no RYNO drops.
2. Buy the RYNO and use some ammunition. Break random ammo crates:
   RYNO ammo should appear among ordinary weapon ammo.
3. Leave other weapons below full: they should still receive ammo.
4. Refill RYNO completely: random crates should stop selecting its ammo.
5. Confirm fixed/assigned ammo crates and enemy drops remain unchanged.
6. Try reloading a save, switching planets, and running alongside other mods.

## Building from source

Requires Python 3, CMake 3.20+, Ninja, and a C compiler:

```sh
python tools/fetch_sdk.py
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The SDK packaging helper packages `build/package/ryno_ammo_drops` for OpenRAC.

## Implementation

The normal `ChooseDropItem` uses the current level's Gadgetron vendor
stock. The RYNO is absent there. This mod wraps `rac1.ammo.choose_drop`
and applies a mod-local RNG to random ammo crates, allowing RYNO to compete
when owned and underfull without permanently altering vendor stock.
RYNO is inventory/ammo item 23 (0x17), not the separate GadgetId 8. Ammo crate class is 511, and RYNO ammo's pickup
class is 1438.

MIT licensed; copyright (c) 2026 glawks.
