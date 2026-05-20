# Ghost Replay

**Menu path:** Custom Menu → IL Settings → Ghost Settings

**Platform:** PS2 75k–90k, and Duckstation only.

Ghost replays let you record your IL runs and race against them in real time.  
  
You can also race against replays other runners upload online! Download ghosts [**here**](https://docs.google.com/spreadsheets/d/1FRsIFruvudBQzKBPcCEm27SErnol9FkRNUDLPE_SKMI/edit?gid=0#gid=0)

---

## How It Works

With ghosts enabled, when you start an IL with L1 + R1 + Triangle, it starts recording your ghost automatically. When you complete a run, the recording is saved. On the next attempt, the ghost plays back alongside you so you can see exactly where you are gaining or losing time.

## Ghost Settings Menu

### Ghost Visual Options

Controls how the ghost is drawn:

| Mode | Description |
|---|---|
| Spyro | Renders a transparent Spyro model at the ghost's position |
| Polygon | Renders a diamond where the ghost would be. Good for laggier levels, as drawing another spyro can add to the lag in bigger levels. |

### Ghost Color Options

Changes what color the transparent ghost is

### Show Recording Length

Displays the maximum available recording time on screen so you can see how much buffer is left.

### Reset All Ghosts

Clears saved ghost recording.

---

## Recording Length Limit

You can only record for about 3:21s before running out of space. A warning is shown on screen when you have **10 seconds remaining**.

---

## Saving & Loading Ghosts

You can save & load ghosts to a USB drive on PS2, and to/from your computer on Duckstation.

### PS2

Download [ghost_tool.elf](https://github.com/C0mposer/Ghost-Tool/releases/download/1.0/ghost_tool.elf) and place it on the root of a USB drive.

**Saving a ghost from PS2:**

1. Record a completed ghost in IL Mode.
2. **Soft reset** (tap power button), then launch `ghost_loader.elf` from a USB drive in uLaunchELF.
3. Choose **Save Ghost**.
4. Review the level, and final time.
5. Press **X** to write the ghost to USB.

The tool writes files to the USB root as `ghost_<level>.bin`. If that filename already exists, it will add a number, like `ghost_high_caves_1.bin`.

**Loading a ghost on PS2:**

1. Put the ghost `.bin` file on the root of a USB drive. Filenames must start with `ghost_` and end with `.bin`.
2. Launch `ghost_loader.elf`.
3. Choose **Load Ghost**, then choose **USB**.
4. Select the ghost file from the list.
5. Review the ghost info, then press **X** to load it into RAM.
6. When the tool says the load is complete, insert the Spyro 1 Practice ROM and press **X**.

The loaded ghost is automatically enabled when the Practice ROM starts.

### Duckstation

Use the Emulator Ghost Tool on Windows.** Duckstation must already be running with the Practice ROM open.**

**Saving a ghost from Duckstation:**

1. Record a completed ghost in IL Mode.
2. Open `emulator_ghost_tool.exe`.
3. Click **Save Ghost File**.
4. Choose where to save the `.bin` file.

**Loading a ghost into Duckstation:**

1. Open `emulator_ghost_tool.exe`.
2. Click **Load Ghost File**.
3. Select a ghost `.bin` file.

## Notes

- Ghost replay is **not available** on PS1 or PS2 30k–70k builds. To use on emulator, you must use Duckstation.
- Ghosts are stored in RAM and are **lost when the console is *hard* reset**. Upon a soft reset, the data stays in memory.
