# Misc Settings

**Menu path:** Custom Menu → Misc Settings

---

## Manual Timer

An on-screen stopwatch for manually timing movement, routes, and tricks.

- Resets when you load a state or respawn Spyro
- Stops when you pause the game
- Integrates with [Disable Portal](#disable-portal) — the timer stops the moment the camera touches the portal

### Manual Timer Display Mode

| Mode | Behavior |
|---|---|
| Display on Stopped | Timer only appears on screen after you pause to stop it |
| Display Always | Timer is always visible |

---

## Sparx Mode

Controls Spyro's health companion Sparx.

| Mode | Behavior |
|---|---|
| Sparx Normal | Default game behavior |
| Perma Sparx On | Sparx stays gold at all times — health never depletes *(only when IL mode is OFF)* |
| Sparxless On | Sparx is permanently dead — for sparxless category practice |

---

## Show Dragon Touch

Displays the number of **frames lost to a dragon touch** (at 30fps). Useful for measuring the cost of accidental dragon conversations mid-run.

---

## Disable Portal

When enabled, the game automatically **loads your current save state the moment the camera touches a portal**. This removes the need to manually reload after failed portal entries.

- Stops the Manual Timer at portal contact, allowing you to time portal entries
- Works per-level — only triggers for portals in the level where the state was saved

---

## Track Consistency

Tracks your success rate on tricks or movement sequences without leaving the game.

Two numbers are displayed in the **bottom-left corner**: a numerator (successes) and a denominator (total attempts).

### Controls

| Action | Input |
|---|---|
| Increment total attempts | Load State button |
| Increment successful attempts | Right Stick Up |
| Decrement successful attempts | Right Stick Down |
| Increment total attempts manually | L2 + R2 + Right Stick Up |
| Decrement total attempts manually | L2 + R2 + Right Stick Down |
| Show tracker for 2 seconds | L2 + R2 |
| Reset both counters | Load State × 3 |
| Reset both counters *(PS1 / 70k alt)* | Hold L2 + R2, then Circle × 3 |

The typical workflow: load your state to attempt the trick (this increments total automatically), then use Right Stick Up if you landed it.

---

## Show Sparx Range

Draws a **cyan octagon** around Spyro showing the radius at which Sparx will collect nearby gems. Useful for optimizing gem vacuum routing.

---

## Super Mode

Lets you use **supercharge** and **superfly** in any level, regardless of whether the level normally supports them.

| Move | How to activate |
|---|---|
| Superfly | Jump, then glide |
| Supercharge | Jump, then press Square × 2 while in the air |

While supercharging:

| Action | Input |
|---|---|
| Increase supercharge speed | R1 |
| Decrease supercharge speed | L1 |

*Super Mode only activates when enabled in the menu.*

---

## Memory Watch

*(PS2 Deckard / Emulator only)*

Watch up to **10 arbitrary RAM addresses** simultaneously, displayed on screen in real time. Useful for debugging, route research, or verifying game state.

### Configuration

- **Address** — 8-digit hex address to watch
- **Data type** — U8, U16, U32, S8, S16, S32, or Fixed 12.20 (fixed-point)
- **Display format** — Hex or Decimal
- **Label** — Custom name up to 10 characters
