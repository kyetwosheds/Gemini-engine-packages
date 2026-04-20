# Cutscene Package

## Purpose
The `Cutscene` package provides a timeline-based cutscene framework for the engine. It separates authored cutscene data from the runtime playback logic and uses a host-interface pattern to execute engine commands without hardwiring dependencies.

## Architecture
- **CutsceneDefinition**: The authored data asset containing multiple tracks and clips.
- **ICutsceneClip**: Base for all events (Dialogue, Move, etc.) with a start time and duration.
- **CutscenePlayer**: The central controller that advances time and triggers events.
- **ICutsceneHost**: Interface that the engine must implement to perform the actual character movements, audio playback, etc.

## Key Components
- **Tracks**: Can be targeted at specific characters or global systems.
- **Clips**: Overlapping clips are supported across different tracks.
- **Edge Triggering**: The player detects when the playback cursor crosses clip boundaries to trigger "Start" and "End" events.

## Integration
1. Implement `ICutsceneHost` in the engine core or a dedicated integration module.
2. Initialize `CutscenePlayer` with your host implementation.
3. Call `player.Update(dt)` in your engine's main loop.
4. Load a `CutsceneDefinition` (authored via future editor tools or JSON) and call `player.Play(definition)`.

## Supported Cues (Framework)
- **Wait**: Empty timing clip.
- **Dialogue**: Character speech with duration or click-to-continue.
- **CharacterMove**: Coordinate-based character walking.
- **Animation**: Trigger specific character animation states.
- **Camera**: Set camera position, zoom, and smoothing.
- **Audio**: Play music or sound effects.
- **RoomChange**: Transition to a new room.
- **FullscreenImage**: Cinematic panel cutaways with faders.
- **ScriptCommand**: Direct execution of engine-specific script commands.
