# Package Development Context & Status

## Overview
This file tracks the status and integration requirements for the `AgentWorkspace`, `TodoWorkspace`, and `Cutscene` packages. These packages are architected to be "clean," manifest-ready, and explicitly dependent on the engine's `HostInterfaces.h` API.

---

## 1. Package Status

### AgentWorkspace (`builtin.agent_workspace`)
- **Status**: Stable.
- **Key Files**: `Packages/AgentWorkspace/src/WorkspaceManager.cpp`, `Packages/AgentWorkspace/src/Session.cpp`.
- **Integration Requirements (Codex)**:
    - Implement `IExternalProcessService` for cross-platform process spawning.
    - Wire `ISettingsStore` for `recent_folders` and `last_provider` persistence.
- **Notes**: Transcript access is thread-safe (`GetTranscriptCopy`).

### TodoWorkspace (`builtin.todo`)
- **Status**: Stable.
- **Key Files**: `Packages/TodoWorkspace/src/TodoManager.cpp`, `Packages/TodoWorkspace/src/TodoSaveParticipant.h`.
- **Integration Requirements (Codex)**:
    - Register `TodoSaveParticipant` with the `IRuntimePackageRegistrar`.
    - Implement `IProjectContent` to support `WriteTextFile` for changelog exports.
- **Notes**: Missing save state is handled as a default empty list.

### Cutscene (`builtin.cutscene`)
- **Status**: Foundation ready.
- **Key Files**: `Packages/Cutscene/src/CutscenePlayer.cpp`, `Packages/Cutscene/include/Cutscene/ICutsceneHostBridge.h`.
- **Integration Requirements (Codex)**:
    - Implement `ICutsceneHostBridge` to execute engine-specific commands (move characters, play sounds, camera).
    - Wire `IRuntimeSystem` to receive engine `Tick` calls.
- **Notes**: Renamed interface to `ICutsceneHostBridge` to explicitly mark it as an engine-supplied dependency.

---

## 2. Global Integration Guidelines
- **Zero Engine Dependencies**: Only `HostInterfaces.h` is allowed for host communication.
- **Stable IDs**: Use workspace/package IDs as defined in the `Module` classes.
- **Persistence**: All package data MUST be handled via `IPackageSaveParticipant` or `ISettingsStore`.
- **Content**: All files MUST be accessed through `IProjectContent` using package-relative paths.

---

## 3. Pending/Future Tasks
- [ ] Bind UI for `AgentWorkspace` (Editor Workspace ID: `"agent_workspace"`).
- [ ] Bind UI for `TodoWorkspace` (Editor Workspace ID: `"todo"`).
- [ ] Bind UI for `Cutscene` (Editor Workspace ID: `"cutscene"`).
- [ ] Implement interpolation/curves for `Cutscene` playback.
- [ ] Add serialization for `TodoItem` timestamps.
