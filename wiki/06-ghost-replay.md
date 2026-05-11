# Ghost Replay

**Menu path:** Custom Menu → IL Settings → Ghost Settings

**Platform:** PS2 75k–90k, and Duckstation only.

Ghost replays let you record your IL runs and race against them in real time.  
You can also race against replays people upload online!

---

## How It Works

When **IL Mode** is active, and Ghosts are active, when you start an IL it starts recording your ghost automatically. When you complete a run, the recording is saved. On the next attempt, the ghost plays back alongside you so you can see exactly where you are gaining or losing time.

Ghosts are automatically enabled on startup if you loaded a ghost externally.

---

## Ghost Settings Menu

### Visual Mode

Controls how the ghost is drawn:

| Mode | Description |
|---|---|
| Full Spyro | Renders a transparent Spyro model at the ghost's position |
| Polygon | Renders a diamond. Good for laggy levels, to not lag the game any more. |

### Show Recording Length HUD

Displays the maximum available recording time on screen so you can see how much buffer is left.

### Reset All Ghosts

Clears saved ghost recording.

---

## Buffer Limits

You can only record for about 3:21s before running out of space. This should be fine for any level in any category, but a warning is shown on screen when you have **10 seconds remaining**.

---

## Saving & Loading Ghosts

Ghost files use the same format on PS2 and DuckStation, so files can be shared between both platforms.

### PS2

Use `ghost_loader.elf` from the Ghost Loader tool on a PS2 75k-90k.

**Saving a ghost from PS2:**

1. Record a completed ghost in IL Mode.
2. **Soft reset**, then launch `ghost_loader.elf` from a FAT32 USB drive in uLaunchELF.
3. Choose **Save Ghost**.
4. Review the level, and final time.
5. Press **X** to write the ghost to USB.

The tool writes files to the USB root as `ghost_<level>.bin`. If that filename already exists, it will add a number, such as `ghost_high_caves_1.bin`.

**Loading a ghost on PS2:**

1. Put the ghost `.bin` file on the root of a FAT32 USB drive. Filenames must start with `ghost_` and end with `.bin`.
2. Launch `ghost_loader.elf`.
3. Choose **Load Ghost**, then choose **USB**.
4. Select the ghost file from the list.
5. Review the ghost info, then press **X** to load it into RAM.
6. When the tool says the load is complete, insert the Spyro 1 Practice ROM and press **X**.

The loaded ghost is automatically enabled when the Practice ROM starts.

### DuckStation

Use the Emulator Ghost Tool on Windows. DuckStation must already be running with the Practice ROM open.

**Saving a ghost from DuckStation:**

1. Record a completed ghost in IL Mode.
2. Open `emulator_ghost_tool.exe`.
3. Click **Save Ghost File**.
4. Choose where to save the `.bin` file.

**Loading a ghost into DuckStation:**

1. Open DuckStation and boot the Practice ROM.
2. Open `emulator_ghost_tool.exe`.
3. Click **Load Ghost File**.
4. Select a ghost `.bin` file.

The tool writes the ghost directly into DuckStation's memory.

## Notes

- Ghost replay is **not available** on PS1 or PS2 30k–70k builds.
- Ghosts are stored in RAM and are **lost when the console is *hard* reset**. Upon a soft reset, the data stays in memory.
