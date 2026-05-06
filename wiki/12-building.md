# Building the Mod

Instructions for compiling the mod from source and generating distributable patches.

---

## Prerequisites

1. **[psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain)** — handles compilation of C code to MIPS object code and builds the final bin/cue.
2. Install the toolchain and all of its own prerequisites as described in its README.
3. Place the contents of this repository into the **/games/** folder of your `psx-modding-toolchain` directory.
4. Place your personal Spyro 1 (NTSC) `.bin` file in **/build/** and rename it to **spyro1.bin**.

---

## Opening the Build Menu

Navigate to **/mods/PracticeCodes/** and run **MOD.bat**. A command-line menu will appear with the available build options.

---

## Build Targets

There are two build targets. Make sure to choose the right one for your intended platform:

| Target | Use for |
|---|---|
| **NTSC** | PS1, PS2 30k–70k, and emulators |
| **PS2_NTSC** | PS2 75k–90k (Deckard) only |

**PS2_NTSC will not work on PS1 or emulators, and vice versa.**

---

## Building a BIN/CUE

1. Type **1** and press Enter to compile. Select your target: **1** for NTSC or **3** for PS2_NTSC.
2. Type **4** and press Enter for "Build ISO". Select the same target as above.
3. Your finished `bin/cue` will appear in **/build/**.

---

## Generating an xdelta Patch

1. Type **1** and press Enter to compile. Select your target.
2. Type **5** and press Enter for "Generate xdelta Patch". Select the same target.
3. Your `.xdelta` patch file will appear in **/build/**.

An xdelta patch can be applied to a vanilla Spyro 1 bin using an xdelta patcher. This is the distribution format used for releases.

---

## Custom Skin Patcher

To patch a custom Spyro skin (BMP file) into your built ROM, use the separate tool:

[Spyro 1 Practice ROM Skin Patcher](https://github.com/C0mposer/Spyro1PracticeRomSkinPatcher)

---

## Tools Used in Development

| Tool | Purpose |
|---|---|
| [psx-modding-toolchain](https://github.com/mateusfavarin/psx-modding-toolchain) | C → MIPS compilation and bin/cue injection |
| [BizHawk](https://github.com/TASEmulators/BizHawk) | RAM searching and poking during development |
| [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) | Hot-reload testing and debugging |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) + [PSX Loader Plugin](https://github.com/lab313ru/ghidra_psx_ldr) | Reverse engineering game functions |
