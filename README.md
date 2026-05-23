

# Spyro 1 Practice Rom
![GitHub Release](https://img.shields.io/github/v/release/c0mposer/spyro-1-practice-rom?style=flat-square&color=blue&link=https%3A%2F%2Fgithub.com%2FC0mposer%2FC-Game-Modding-Utility%2Freleases%2Ftag%2Flatest)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/C0mposer/Spyro-1-Practice-Rom/total?style=flat-square&color=purple&link=https%3A%2F%2Fgithub.com%2FC0mposer%2FSpyro-1-Practice-Rom%2Freleases%2Flatest) 



This mod for the original Spyro the Dragon adds practice features for speedrunners, including:

| Feature | Description |
|---|---|
| *Full Save-States* | Save and Load states to practice sections. |
| *Ghost Replays* | Compare against top level ghost replays like a racing game. |
| *Level Select* | Teleport to any level instantly. |
| *Timing Tools* | Time movements, tricks, and more. |
| *Custom Skins* | Change Spyro's appearance while practicing. |
| *Visualizations* | Visualize hitboxes, Sparx's range, and more.|
| *Much More!* | [View Wiki for all features](https://github.com/C0mposer/Spyro-1-Practice-Rom/tree/main/wiki#spyro-1-practice-rom-wiki) |

## Downloads

This mod supports multiple platforms. **Download the correct version for what you're going to play on**. The PS2 build will not work on PS1 or emulator, and vice versa.

| Platform | Save States | Ghost Replay | Notes |
|---|---|---|---|
|[PS2 75k–90k](https://github.com/C0mposer/Spyro-1-Practice-Rom/releases/download/fullrelease5.0/Spyro.1.Practice.Rom.PS2.Deckard.zip) | Full | Yes | Recommended console for full feature set |
| [Duckstation](https://github.com/C0mposer/Spyro-1-Practice-Rom/releases/download/fullrelease5.0/Spyro.1.Practice.Rom.Duckstation.zip) | Full | Yes | Recommended emulator for full feature set |
| [PS2 30k–70k](https://github.com/C0mposer/Spyro-1-Practice-Rom/releases/download/fullrelease5.0/Spyro.1.Practice.Rom.PS2.IOP.zip) | Partial | No | Spyro/camera position save-states only |
| [PS1 & Other Emulators](https://github.com/C0mposer/Spyro-1-Practice-Rom/releases/download/fullrelease5.0/Spyro.1.Practice.Rom.PS1.zip) | Partial | No | Spyro/camera position save-states only |

See [Platform Comparison](13-platform-comparison.md) for a complete breakdown.

---

## Patching Your Own ISO

If you already own a Spyro 1 `.bin` file and want to patch it yourself:

[Online Patcher](https://c0mposer.github.io/Spyro-1-Practice-Rom/)

---

## Emulator Setup Notes

- **Duckstation** is the recommended emulator.
- You **MUST** enable extra 8MB ram if you use the Duckstation version
![Duckstation Settings](image.png)

---

## Tools Used In Creation

 - [C/C++ Game Modding Utility](https://github.com/C0mposer/C-Game-Modding-Utility) for the automation of compiling C code to MIPS object code targeted for the PS1, and building a bin/cue with the mod files injected correctly.
 - [Bizhawk](https://github.com/TASEmulators/BizHawk) for searching/poking PS1 ram values easily
 - [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) for hot-reloading mods in for quick testing. Also for its debugging features.
 - [Ghidra](https://github.com/NationalSecurityAgency/ghidra) in combination with the [PSX Executable Loader Plugin](https://github.com/lab313ru/ghidra_psx_ldr) for reverse engineering functions in the game.

## Contact

If you need any help building this project, or are just trying to get into spyro/game modding in general, feel free to reach out to us on discord: **Composer** & **OddKara** :)
