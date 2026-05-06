# Visualizer Settings

**Menu path:** Custom Menu → Visualizer Settings

---

## Hitbox Viewer

Renders active object collision in real time.

---

## Free Camera

Detaches the camera from Spyro, giving you control to move around the camera with the analog sticks. Spyro stops updating while free cam is active.

| Action | Controls |
|---|---|
| Move Camera | Left Analog |
| Rotate Camera | Right Analog |
| Speed Up Movement | R2 |
| Slow Down Movement | L2 |
| Move Camera Upwards | R1 |
| Move Camera Downwards | L1 |
---

## Theatre Mode

A full camera keyframing system for creating cinematic shots.

### Overview

Set up to **16 keyframes**, each storing a camera position and angle. The camera interpolates smoothly between them on playback.

### Adding Keyframes

Move the camera to the desired position, then add some keyframes. Each keyframe has:

- **Transition time** — frames to travel to the next keyframe (default: 60 frames)
- **Wait time** — frames to hold at this keyframe before moving to the next

### Interpolation Modes

| Mode | Description |
|---|---|
| Ease In/Out *(default)* | Smooth acceleration and deceleration |
| Linear | Constant speed between keyframes |
| Spline | Smooth spline through all keyframes |
| Ease In | Accelerates into the next keyframe |
| Ease Out | Decelerates into the next keyframe |

### Playback Options

| Option | Description |
|---|---|
| Auto-play Forward | Automatically plays through keyframes in order |
| Auto-play Backward | Plays keyframes in reverse order |
| Auto-play Off | Manual or scripted control only |
| Loop | Loops back to the first keyframe, when it reaches the last keyframe |
| Green Screen | Draw's all objects over a solid green background for compositing |
| Hide HUD | Hides all on-screen HUD elements during playback |


## Sparx Range

Shows Sparx's gem pickup radius as an octagon.
