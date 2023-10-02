

# Spyro 1 Practice Codes For PS2

This is a mod for the original Spyro the Dragon that adds practice features for speedrunners. This is for use primarily on PS2 with a burnt disc. 

These codes are relatively simple in comparison to [my Popstarter practice codes](https://discord.gg/rm4ZnjzeZR), that can also run on PS2. These codes lack the ability to do full save-states, however, they can run on the actual PS1 backwards compatibility layer of the PS2 with a burnt disc. Meaning: no Popstarter lag, no inaccuracies, resulting in completely accurate practice.

To play this using a burnt disc on PS2, will require you to have a softmodded ps2 with [Mechapwn](https://github.com/MechaResearch/MechaPwn) installed, or have a modchip.

This mod was created using the [psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain). 

## Features
1. Level Select in Inventory Menu (L1, L2, L3, R1, R2, R3)
2. Respawn Spyro (L1 + R1 + Circle)
3. Save/Load Position & Camera (L3/R3)
4. Re-Fly in to level quickly (L1 + R1 + Up + Triangle)
5. Moon-jump (L1 + R1 + L2 + R2 + X)
7. Collectables Always Respawn (Gems, Dragons, Eggs)
8. Automatically Skips Intro
9. Can Skip Nestor

## Downloads
#### Main Download
 -  **Download Patch**: Download the main [xdelta patch here](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/release2/spyro1_PracticeCodes.xdelta), then your personal Spyro 1 bin file can be [patched here](https://hack64.net/tools/patcher.php)
 -  **CUE File**: Once you've patched your bin,  you can create a cue file for it [here](https://www.duckstation.org/cue-maker/). This cue file can now be burned and played on a [mechapwn'd](https://github.com/MechaResearch/MechaPwn)/chipped PS2.


## Compiling & Building

1. Compiling this mod requires the [psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain). 
2. Once you have installed it and its pre-requisites, place the folder [from the source code download](https://github.com/C0mposer/Spyro-1-Practice-Codes/archive/refs/heads/master.zip), into the **/games/** folder of your psx-modding-toolchain directory.
4. Place your personal spyro 1 bin file in **/build/** and rename it to **spyro1.bin**
5. Browse to the **/mods/PracticeCodes/** directory, and open the MOD.bat file. From here you should be prompted with multiple options in a command line gui

#### Building BIN/CUE
	
	1. First type 1 then press enter to compile, then, type 3 then press enter again for the PS2_NTSC version.
	2. After it has compiled, type 4 then enter for "Build Iso", then, type 3 then press enter for the PS2_NTSC version.
	3. After it has built, your bin/cue should be in /build/

#### Generating xdelta patch
	
	1. First type 1 then press enter to compile, then, type 3 then press enter again for the PS2_NTSC version.
	6. After it has compiled, type 5 then press enter for "Generate xdetla Patch", then type 3 then press enter for the PS2_NTSC version.
	7. After it has built, your xdelta patch should be in /build/
	

## Tools Used In Creation

 - [psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain) for the automation of compiling C code to MIPS object code targeted for the PS1, and building a bin/cue with the mod files injected correctly.
 - [Bizhawk](https://github.com/TASEmulators/BizHawk) for searching/poking PS1 ram values easily
 - [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) for hot-reloading mods in for quick testing. Also for its debugging features.
 - [Ghidra](https://github.com/NationalSecurityAgency/ghidra) in combination with the [PSX Executable Loader Plugin](https://github.com/lab313ru/ghidra_psx_ldr) for reverse engineering functions in the game.

## Contact

If you need any help building this project, or are trying to get into spyro/game modding in general, feel free to reach out to me on discord: **Composer#7458**
