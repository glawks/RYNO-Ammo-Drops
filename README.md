# RYNO Ammo Drops

An OpenRAC mod for **Ratchet & Clank (2002)** that lets random ammo crates drop **R.Y.N.O. ammunition** once you've acquired the weapon.

The R.Y.N.O. becomes part of the game's normal low-ammo selection, so you can refill it from crates alongside your other weapons instead of relying solely on the vendor.

## Features

- **R.Y.N.O. ammo in random crates:** available only after you own the R.Y.N.O. and have room for more ammunition.
- **Balanced with other weapons:** R.Y.N.O. competes with other eligible weapons that need ammo; it doesn't take over every crate.
- **Original drops preserved:** when the R.Y.N.O. is full, the game's ordinary ammo selection is left unchanged.
- **No additional pickups:** the mod changes the ammo type selected for a random crate; it does not spawn a bonus pickup.
- **Minimal changes:** fixed/assigned ammo crates and enemy drops are left to the original game logic, with no permanent changes to vendor stock or the save format.

## Installation

1. Open the **OpenRAC Launcher** and go to **Mods**.
2. Add a mod using this repository URL:
   `https://github.com/glawks/RYNO-Ammo-Drops`
3. Install and enable **RYNO Ammo Drops**, then start Ratchet & Clank.

The launcher downloads the appropriate build from the [latest GitHub Release](https://github.com/glawks/RYNO-Ammo-Drops/releases/latest).

For manual installation, download your platform's `ryno-ammo-drops-<version>-<platform>.zip` from [Releases](https://github.com/glawks/RYNO-Ammo-Drops/releases). A combined multi-platform archive is also provided.

## How it works

The original game chooses random ammunition drops using the current level's Gadgetron vendor stock, which excludes the R.Y.N.O. This mod adds the owned R.Y.N.O. as an eligible choice when it needs ammunition, while preserving the existing selection for other weapons.

For example, if the R.Y.N.O. and one other eligible weapon both need ammo, each has approximately a **50% chance** of being selected by the mod's weighting. If the R.Y.N.O. is already full, it will not be selected.

Only *random* ammo-crate selections are affected. Crates with fixed drops and other kinds of pickups are not changed.

## Settings

Both settings are **off by default** and are optional.

| Setting | Description |
| --- | --- |
| **Detailed log** | Writes ammo-crate decisions, eligible weapons and R.Y.N.O. ammo levels to the OpenRAC log for troubleshooting. |
| **Force RYNO drops (testing)** | Makes eligible random crates choose R.Y.N.O. ammo whenever you own it and it is not full. Intended for testing; disable for normal gameplay. |

## Compatibility

| Game / platform | Status |
| --- | --- |
| Ratchet & Clank 1 — NTSC-U | Tested in-game |
| Ratchet & Clank 1 — PAL | Compiled; in-game testing still welcome |
| Windows x86-64 | Compiled |
| Linux x86-64 | Compiled |
| macOS Apple Silicon / Intel | Compiled |

The mod uses OpenRAC's `rac1.ammo.choose_drop` hook. It is not intended for Ratchet & Clank 2 or 3.

## Building from source

Requires Python 3, CMake 3.20 or newer, Ninja, a C compiler and the OpenRAC mod SDK (downloaded by the first command).

```sh
python tools/fetch_sdk.py
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

GitHub Actions builds Windows, Linux and macOS packages automatically on `main` and `dev`. The `main` workflow also publishes a new GitHub Release when the manifest version has not already been released. Development artifacts can be found under [Actions](https://github.com/glawks/RYNO-Ammo-Drops/actions/workflows/build.yml).

## Feedback and bug reports

If you find an issue, please [open an issue](https://github.com/glawks/RYNO-Ammo-Drops/issues) with your game region, OpenRAC version, mod version, steps to reproduce it and, if possible, the relevant OpenRAC log. PAL feedback is especially welcome.

## License

[MIT](LICENSE) — Copyright © 2026 glawks.
