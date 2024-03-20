

# Spyro 1 Practice Codes

This mod for the original Spyro the Dragon adds practice features for speedrunners including but not limited to: *partial save-states*, *a level select*, *timing functionality*, *automatic collectable resetting*, and many other quality of life features.

This can be played on PS1, PS2, PS3, and most emulators. 

To play this on PS2 with a burnt CD will require you to specifically download the **PS2 version**. It will also require you to have a softmodded PS2 with [MechaPwn](https://github.com/MechaResearch/MechaPwn) installed, or have a modchip.

## Hotkeys:
1. Open Custom Menu (**L2 + R2 + Triangle**)
2. Open Level Select (**Select**)
3. Quick Fly In To Current Level (**L1 + R1 + Triangle**)
4. Respawn Spyro (**L1 + R1 + Circle**)
5. Save/Load Spyro's Data (**L3** to save, **R3** to load)
6. Moon-jump (**L1 + R1 + L2 + R2 + X**)
7. Give Key (**Triangle x3**)

# Downloads
**Be sure to download the correct version for the platform you are going to play on**
## BIN/CUE Downloads
To simply download the patched rom and get practicing, download the cue/bin here:
   
[PS2 bin/cue](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/fullrelease1.4/spyro1_PracticeCodes_PS2.zip)
    
[PS1/PS3/EMU bin/cue](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/fullrelease1.4/spyro1_PracticeCodes_PS1.zip)

*Note: To play this rom on **PS2** requires either a [MechaPwn'd](https://github.com/MechaResearch/MechaPwn) or modchipped PS2 to read the burned disc. Must be a CD not a DVD.*

## Xdelta Downloads
If you wish to patch your own personal spyro 1 bin/cue file instead, download the patch here:
   
[PS2 Xdelta Patch](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/fullrelease1.4/spyro1_PracticeCodes_PS2.xdelta)
   
[PS1/PS3/EMU Xdelta Patch](https://github.com/C0mposer/Spyro-1-Practice-Codes/releases/download/fullrelease1.4/spyro1_PracticeCodes_PS1.xdelta)

## Menu Features:

### Timer: 
This setting displays an on screen timer that resets upon reloading spyro, and stops upon pausing. Can be used for timing movement/routes in game!

- **TIMER ONLY STOPPED** mode displays the timer on screen only when you pause to stop the timer. This is to reduce minor lag that could be caused from the extra rendering of the text mobys.
- **TIMER ALWAYS** mode displays the timer on screen at all times.
	 
### IL Mode:
This setting lets you practice & time individual level runs for most categories! [Click here to view the IL spreadsheet](https://www.docs.google.com/spreadsheets/d/1FLkc8-dGTO4jJhAi5nDrhm0h6qlVflPlAIlXAYEylsg/edit#gid=1140931951)

- Once you enter/re-fly in to a level, a timer will keep track of its time starting the first frame the HUD starts coming down from the fly in.
- Once you leave through a vortex/through the pause menu, it will then stop the first frame the HUD for the "RETURNING HOME" text starts coming down in the loading screen.
- The loading screen will then display your individual level time in big text, and underneath display the loadless version of the same time to account for dragons in slightly smaller text.
- It will also reload you back into the same level to let you practice on loop.
- This timer should be accurate within 1 frame (~0.03) in most situations.
	 
### Reset All With R3:
- This setting will reset the level every time before loading a sudo-savestate.   

- This is essentially a macro for resetting the level manually with *L1 + R1 + Circle* and then pressing R3 after.
	 
### Sparx Mode:
- **PERMA SPARX ON** mode keeps sparx gold at all times, and your health will never deplete.
- **SPARXLESS ON** mode keeps sparx dead at all times, allowing for sparxless practice.
	 
### Quick Goop:
This setting kills spyro in goop the moment you can not input anymore. Small quality of life feature to reduce the amount of time you have to wait to die in goop.
	 
### BG Color:
You can select the pause/inventory menu color from a range of pre-selected options!
	 

## Other Features:

### Automatically Skips Intro:
Skips straight to "The Adventure Continues..."
	
### Nestor Skip:
Nestor is always skippable like any other dragon. Allows for faster nestor skip practice.

### Rat Practice:
Freeze/Unfreeze rat - *Triangle 2 times*  
  
Allows for a longer time to set up rat-surf.  
The rat is saved as a part of the sudo-savestate (L3/R3) as well

### Open/Close heads in Gnasty's World
Open/Close Twilight Harbor - *L2 3 times*   
Open/Close Gnasty Gnorc - *R2 3 times*   
Open/Close Gnasty's Loot - *R1 3 times*   

### Credits Skip Practice
Warp to end of Gnasty Gnorc fight - *L1 + R1 + X*   

### Loot Plane Practice
Warp before plane 2 with key - *L1 + R1 + X*   
Warp before plane 1 with key - *L1 + R1 + Square*   


## Compiling & Building
If you wish to compile the mod yourself, you'll need a few prerequisites
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
