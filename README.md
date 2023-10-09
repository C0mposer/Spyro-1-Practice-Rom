

# Spyro 1 Practice Codes For PS2

This mod for the original Spyro the Dragon adds practice features for speedrunners including but not limited to: *partial save-states*, *timing functionality*, and many other quality of life features.

This can be played on PS1, PS2, PS3, and most emulators. 

To play this on PS2 with a burnt disk will require you to specifically download the **PS2 version**. It will also require you to have a softmodded ps2 with [Mechapwn](https://github.com/MechaResearch/MechaPwn) installed, or have a modchip.

## Hotkeys:
1. Open Custom Menu (**L2 + R2 + Triangle**)
2. Open Level Select (**Select**)
3. Quick re-fly in to current level (**L1 + R1 + Triangle**)
4. Respawn Spyro (**L1 + R1 + Circle**)
5. Save/Load Spyro's Data (**L3** to save, **R3** to load)
6. Moon-jump (**L1 + R1 + L2 + R2 + X**)

## Menu Features:

### Timer: 
This setting displays an on screen timer that resets upon reloading spyro, and stops upon pausing. Can be used for timing movement/routes in game!

	 - "TIMER ONLY STOPPED" mode displays the timer on screen only when you pause to stop the timer. This is to reduce minor lag that could be caused from the extra rendering of the text mobys.
	 - "TIMER ALWAYS" mode displays the timer on screen at all times.
	 
### IL Mode:
This setting lets you practice & time individual level runs for most categories! (SPREADSHEET FOR VORTEX & 120 COMING SOON!)

	 - Once you enter/re-fly in to a level, a timer will keep track of its time starting the first frame the HUD starts coming down from the fly in.
	 - Once you leave through a vortex/through the pause menu, it will then stop the first frame the HUD for the "RETURNING HOME" text starts coming down in the loading screen.
	 - The loading screen will then display your individual level time in big text, and underneath display the loadless version of the same time to account for dragons in slightly smaller text.
	 - It will also reload you back into the same level to let you practice on loop.
	 - This timer should be accurate within 1 frame (~0.03) in most situations.
	 
### Reset Collectables:
This setting resets all collectables anytime you reload the level through flying in/re-flying in, respawning spyro, etc. (ON by default)

	 - Wont reset collectables if you die normally in game even when ON, so you can still death abuse during practice.
	 - Could be turned off to practice homeworlds/longer segments while maintaining what you've collected.
	 
### Sparx Mode:
	 - "PERMA SPARX ON" mode keeps sparx gold at all times, and your health will never deplete.
	 - "SPARXLESS ON" mode keeps sparx dead at all times, allowing for sparxless practice.
	 
### Quick Goop:
This setting kills spyro in goop the moment you can not input anymore. Small quality of life feature to reduce the amount of time you have to wait to die in goop.
	 
### BG Color:
You can select the pause/inventory menu color from a range of pre-selected options!
	 

## Other Features:

### Automatically Skips Intro:
Skips straight to "The Adventure Continues..."
	
### Nestor Skip:
Nestor is always skippable like any other dragon. Allows for faster nestor skip practice.

## Downloads
#### PS2
 -  **Download Patch**: Download the main [xdelta patch here](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/release2/spyro1_PracticeCodes.xdelta), then your personal Spyro 1 bin file can be [patched here](https://hack64.net/tools/patcher.php)
 -  **CUE File**: Once you've patched your bin,  you can create a cue file for it [here](https://www.duckstation.org/cue-maker/). This cue file can now be burned and played on a [mechapwn'd](https://github.com/MechaResearch/MechaPwn)/chipped PS2.
 #### All Other Platforms (PS1, PS3, Emulator, etc)
 -  **Download Patch**: Download the main [xdelta patch here](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/release2/spyro1_PracticeCodes.xdelta), then your personal Spyro 1 bin file can be [patched here](https://hack64.net/tools/patcher.php)
 -  **CUE File**: Once you've patched your bin,  you can create a cue file for it [here](https://www.duckstation.org/cue-maker/). This cue file can now be burned and played on a [mechapwn'd](https://github.com/MechaResearch/MechaPwn)/chipped PS2.


## Compiling & Building

1. Compiling this mod requires the [psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain). 
2. Once you have installed it and its pre-requisites, place the folder [from the source code download](https://github.com/C0mposer/Spyro-1-Practice-Codes/archive/refs/heads/master.zip), into the **/games/** folder of your psx-modding-toolchain directory.
4. Place your personal spyro 1 bin file in **/build/** and rename it to **spyro1.bin**
5. Browse to the **/mods/PracticeCodes/** directory, and open the MOD.bat file. From here you should be prompted with multiple options in a command line gui

#### Building BIN/CUE
	
	1. First type 1 then press enter to compile, then, either type 3 PS2_NTSC version, or type 1 for the NTSC version which will work for all other platforms.
	2. After it has compiled, type 4 then enter for "Build Iso", then, type 3 PS2_NTSC version, or type 1 for the NTSC version which will work for all other platforms.
	3. After it has built, your bin/cue should be in /build/
	4. Be sure you built for the correct version. PS2_NTSC will not work on PS1/EMU, and visa-versa.

#### Generating xdelta patch
	
	1. First type 1 then press enter to compile, then, type 3 for the PS2_NTSC version, or type 1 for the NTSC version which will work for all other platforms.
	2. After it has compiled, type 5 then press enter for "Generate xdetla Patch", then type 3 for the PS2_NTSC version, or type 1 for the NTSC version which will work for all other platforms.
	3. After it has built, your xdelta patch should be in /build/
	4. Be sure you built for the correct version. PS2_NTSC will not work on PS1/EMU, and visa-versa.
	

## Tools Used In Creation

 - [psx-modding-toolchain by Redhot](https://github.com/mateusfavarin/psx-modding-toolchain) for the automation of compiling C code to MIPS object code targeted for the PS1, and building a bin/cue with the mod files injected correctly.
 - [Bizhawk](https://github.com/TASEmulators/BizHawk) for searching/poking PS1 ram values easily
 - [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) for hot-reloading mods in for quick testing. Also for its debugging features.
 - [Ghidra](https://github.com/NationalSecurityAgency/ghidra) in combination with the [PSX Executable Loader Plugin](https://github.com/lab313ru/ghidra_psx_ldr) for reverse engineering functions in the game.

## Contact

If you need any help building this project, or are just trying to get into spyro/game modding in general, feel free to reach out to us on discord: **Composer** & **OddKara** :)