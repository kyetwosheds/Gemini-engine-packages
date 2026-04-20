# AgentWorkspace Package

## Purpose
The `AgentWorkspace` package provides a native backend foundation for integrating AI Agent CLIs (Gemini, Codex, Claude) into the engine. It handles provider abstraction, session management, background process coordination, and provides a view-model state layer for future UI binding.

## Architecture
The package follows a clean, interface-driven architecture to ensure isolation from the engine core and extensibility for future providers.

### Core Components
- **IAgentProvider**: Abstraction for CLI-based agents. Handles launch specification and capability discovery.
- **ISession**: Represents an active or historical conversation with an agent. Manages transcripts and state.
- **IWorkspaceManager**: The central coordinator that manages the `WorkspaceState` (view-model).
- **WorkspaceState**: A plain-old-data structure representing the entire state of the workspace, suitable for UI binding.

### Provider Implementations
- **GeminiCliProvider**: Adapter for `gemini-cli`.
- **CodexCliProvider**: Adapter for `codex-cli`.
- **ClaudeCliProvider**: Adapter for `claude-cli`.

### Host Integration (Contracts)
The package defines several interfaces that must be implemented by the host (Codex/Engine) to provide platform-specific services:
- **IBackgroundProcessHost**: Responsible for spawning and managing the actual OS processes.
- **IFolderPickerHost**: Provides native folder selection dialogs.
- **IAgentHostServices**: Aggregates host services and provides logging.

## Implementation Details
- **Async Output**: Process output is handled via callbacks and streamed into session transcripts.
- **State Management**: The `WorkspaceManager` maintains a consistent view of all sessions and notifies listeners of any state changes.
- **Persistence Ready**: Models are designed to be easily serialized into JSON or other formats for session persistence.

## Future Integration Hooks
When the new package API is ready, the following steps are needed:
1. Implement `IBackgroundProcess` using the engine's process management system.
2. Implement `IBackgroundProcessHost` and `IFolderPickerHost`.
3. Register the `WorkspaceManager` as a package-local singleton.
4. Bind the future Codex-like UI (sidebar, transcript panel, composer) to `WorkspaceState`.

## Integration Points for Codex
Codex must expose the following for final hookup:
- A way to register the package's view-model with the editor's UI framework.
- A standardized `IBackgroundProcess` implementation that handles stdout/stderr redirection and signal handling (Ctrl+C).
- A hook for persistent settings storage (for recent folders and provider selection).
