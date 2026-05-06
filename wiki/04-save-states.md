# Save States

**Menu path:** Custom Menu → Savestate Settings

---

## Full Save States vs. Partial Save States

The type of save state available depends on your hardware. See [Platform Comparison](13-platform-comparison.md) for a full matrix.

### Full Save States (PS2 75k–90k / Deckard)

Full save states capture and restore essentially the entire level state:

- Spyro position, animation, and health
- Camera position and state
- All collectables (gems, dragons, eggs)
- Key state and key chest host gem
- Particle effects
- Respawn position and angle
- Electrical pad activation states
- Flight level timer and collected items
- Superflame timer state
- Level geometry and dynamic objects (moving platforms, etc.)
- Rat position (in Gnasty's World)

### Partial Save States (PS1 / PS2 30k–70k)

Partial save states only save and restore:

- Spyro position
- Camera position
- Rat position (in Gnasty's World)

Collectables and level state are **not** saved on these platforms.

---

## Savestate Slots

There are **3 independent savestate slots**. You can switch between them at any time.

- **Current Slot** — view or manually change the active slot from the menu
- **Switch Slot Hotkey** — quickly cycle slots without opening the menu

---

## Customizable Buttons

### Save Button

| Option | Input |
|---|---|
| L3 *(default)* | Press left stick |
| Start | Press Start button |
| L3 × 2 | Double-tap left stick |

### Load Button

| Option | Input |
|---|---|
| R3 *(default)* | Press right stick |
| Select | Press Select button |

### Switch Slot Button

| Option | Input |
|---|---|
| Right Stick Left / Right *(default)* | Tilt right stick left or right |
| L1 + R1 + DPad Left / Right | Hold L1+R1, press DPad |

---

## Notes

- On **PS2 30k–70k**, an additional **Respawn on Loadstate** toggle is available in the menu.
- If you use **Track Consistency**, the Load State button also increments your attempt count — factor this in when choosing your load button.
- **Rat position** is included in the partial save state for Gnasty's World, even on PS1 builds.
