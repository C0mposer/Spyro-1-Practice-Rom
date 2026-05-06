# Ghost Replay

**Menu path:** Custom Menu → IL Settings → Ghost Settings

**Platform:** PS2 Deckard (75k–90k) and Emulators only.

Ghost replay lets you record your IL runs and race against them in real time.

---

## How It Works

When **IL Mode** is active, the ghost system records your movement automatically. When you complete a run, the recording is saved. On the next attempt, the ghost plays back alongside you so you can see exactly where you are gaining or losing time.

Ghosts are automatically enabled on startup if a recording already exists for the current level.

---

## Recording Format

Ghosts use the **V2 format** (magic header `GHD2`). Each frame stores:

- Position deltas (compressed movement)
- Animation state
- Head animation state
- Visual angle

Dragon pause events (up to 16 per recording) are tracked separately so loadless comparisons remain accurate.

---

## Buffer Limits

The maximum recordable ghost length depends on your platform:

| Platform | Max ghost length (approx.) |
|---|---|
| PS2 Deckard (75k–90k) | ~3:22 at 30fps |
| Emulator (PCSX-Redux / DuckStation) | ~8:17 at 30fps |

A warning is shown on screen when you have **10 seconds of buffer remaining**.

---

## Ghost Settings Menu

### Visual Mode

Controls how the ghost is drawn:

| Mode | Description |
|---|---|
| Full Spyro | Renders a full Spyro model at the ghost's position |
| Wireframe | Ghost is shown as a wireframe outline |
| Polygon Only | Ghost geometry without texture |

### Show Recording Length HUD

Displays the maximum available recording time on screen so you can see how much buffer is left.

### Reset All Ghosts

Clears all saved ghost recordings. A confirmation dialog is shown before deleting.

---

## Notes

- Ghost replay is **not available** on PS1 or PS2 30k–70k (IOP) builds.
- The emulator ghost buffer is significantly larger than the Deckard buffer — if you need to record long levels, emulator is recommended.
- Ghosts are stored in RAM and are **lost when the console or emulator is reset**.
