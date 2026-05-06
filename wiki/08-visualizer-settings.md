# Visualizer Settings

**Menu path:** Custom Menu → Visualizer Settings

---

## Hitbox Viewer

Renders active collision shapes in the current level in real time. Useful for understanding enemy hitboxes, gem hitboxes, and trigger volumes.

Supported shapes:

| Shape type | Color / display |
|---|---|
| Sphere | Cyan rings |
| Sphere-Z | Vertical sphere rings |
| Pill (capsule) | Capsule outline |
| XForm | Transformed geometry |

- Draw distance: **10,000 units**
- Up to **48 hitboxes** rendered per frame
- Budget: 8,192 primitives

---

## Collision Wireframe

*(PS2 Deckard / Emulator only)*

Renders the level's collision geometry as a wireframe overlay. Lets you see exactly where collision boundaries are, including invisible walls and floors.

---

## Free Camera

Detaches the camera from Spyro, giving you full 3D control. Spyro stops updating while free cam is active — he will not fall or move.

Useful for:

- Scouting geometry and collision from any angle
- Setting up theatre mode keyframes
- Capturing screenshots or footage without Spyro in frame

---

## Theatre Mode

A full camera keyframing system for creating cinematic shots or replays.

### Overview

Set up to **16 keyframes**, each storing a camera position and angle. The camera interpolates smoothly between them on playback.

### Adding Keyframes

Move the camera to the desired position, then add a keyframe from the Theatre Mode menu. Each keyframe has:

- **Transition time** — frames to travel from the previous keyframe (default: 60 frames)
- **Wait time** — frames to hold at this keyframe before moving to the next

### Interpolation Modes

| Mode | Description |
|---|---|
| Ease In/Out *(default)* | Smooth acceleration and deceleration |
| Linear | Constant speed between keyframes |
| Catmull-Rom | Smooth spline through all keyframes |
| Ease In | Accelerates into the next keyframe |
| Ease Out | Decelerates into the next keyframe |

### Playback Options

| Option | Description |
|---|---|
| Auto-play Forward | Automatically plays through keyframes in order |
| Auto-play Backward | Plays keyframes in reverse order |
| Auto-play Off | Manual or scripted control only |
| Loop | Restarts playback when it reaches the last keyframe |
| Green Screen | Replaces the background with a solid color for compositing |
| Hide HUD | Hides all on-screen HUD elements during playback |

### Camera Controls (during Theatre Mode)

| Action | Input |
|---|---|
| Rotate camera | Arrow / DPad |
| Move forward / back | R2 / L2 |
| Move up / down | R1 / L1 |

---

## Sparx Range

Also accessible from Misc Settings — see [Misc Settings → Show Sparx Range](07-misc-settings.md#show-sparx-range).
