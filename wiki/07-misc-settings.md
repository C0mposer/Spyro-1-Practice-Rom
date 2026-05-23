# Misc Settings

**Menu path:** Custom Menu → Misc Settings

---

## Manual Timer

An on-screen timer for manually timing movement, routes, and tricks. Essentially a built in stopwatch.

- Resets when you load a state or respawn Spyro
- Stops when you pause the game
- Integrates with Disable Portal. The timer stops the moment the camera touches the portal when Disable Portal is on. Good for timing homeworld sections.

### Manual Timer Display Mode

| Mode | Behavior |
|---|---|
| Display on Stopped | Timer only appears on screen after you stop it |
| Display Always | Timer is always visible |

---

## Sparx Mode

Controls Sparx's health.

| Mode | Behavior |
|---|---|
| Sparx Normal | Default behavior |
| Perma Sparx | Sparx stays gold at all times. *(only when IL mode is OFF)* |
| Sparxless On | Sparx is permanently dead. (For sparxless practice) |

---

## Show Dragon Touch

Displays the number of **frames lost to a bad dragon touch** (at 30fps).

---

## Disable Portal

When enabled, the game automatically **loads your current save state the moment the camera touches a portal**. This is useful for timing portal entries.

- Stops the Manual Timer when Spyro & the camera touches the portal, allowing you to easily time portal entries accurately.

---

## Track Consistency

Helps you track your success rate on tricks or movement sequences.

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
| Reset both counters *(Alternative)* | Hold L2 + R2, then Circle × 3 |

Best way to use it: load your state to attempt the trick (this increments total automatically), then use Right Stick Up if you landed it.

---

## Memory Watch

*(PS2 75k-90k / Duckstation only)*

View the values of up to **10 arbitrary RAM addresses** simultaneously. Displayed on screen during gameplay.

### Configuration

- **Address** — 8-digit memory address to watch
- **Data type** — U8, U16, U32, S8, S16, S32, or Fixed 12.20 (fixed-point)
- **Display format** — Hex or Decimal
- **Label** — Custom name up to 10 characters
