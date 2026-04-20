# CONSOLIDATED SOURCE CODE FOR REVIEW

This document contains the complete source code for three C++ packages:
1.  **AgentWorkspace**: Backend for AI Agent CLI integration.
2.  **TodoWorkspace**: Project action item management with Steam-ready changelog export.
3.  **Cutscene**: Initial framework for a timeline-based cutscene system.

---

## 1. AgentWorkspace Package

### `Packages/AgentWorkspace/README.md`
```markdown
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
```

### `Packages/AgentWorkspace/include/AgentWorkspace/IAgentProvider.h`
```cpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace AgentWorkspace {

enum class AgentStatus {
    Idle, Initializing, Running, Busy, Cancelling, Error, Disconnected
};

enum class AgentCapability {
    Interactive, OneShot, BackgroundSession, Streaming, SessionPersistence, FileManipulation
};

struct ProviderLaunchSpec {
    std::string executablePath;
    std::string workingDirectory;
    std::vector<std::string> arguments;
    std::string sessionId;
};

class IAgentProvider {
public:
    virtual ~IAgentProvider() = default;
    virtual std::string GetId() const = 0;
    virtual std::string GetDisplayName() const = 0;
    virtual bool IsAvailable() const = 0;
    virtual ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const = 0;
    virtual bool SupportsCapability(AgentCapability capability) const = 0;
};

}
```

### `Packages/AgentWorkspace/include/AgentWorkspace/ISession.h`
```cpp
#pragma once

#include "IAgentProvider.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace AgentWorkspace {

struct TranscriptEntry {
    std::string text;
    std::chrono::system_clock::time_point timestamp;
    bool isFromAgent;
};

class IBackgroundProcess {
public:
    virtual ~IBackgroundProcess() = default;
    virtual bool Start(const ProviderLaunchSpec& spec) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual void SendInput(const std::string& text) = 0;
    using OutputCallback = std::function<void(const std::string&, bool isError)>;
    virtual void SetOutputCallback(OutputCallback callback) = 0;
};

class ISession {
public:
    virtual ~ISession() = default;
    virtual std::string GetId() const = 0;
    virtual std::string GetDisplayName() const = 0;
    virtual std::shared_ptr<IAgentProvider> GetProvider() const = 0;
    virtual std::string GetWorkingDirectory() const = 0;
    virtual const std::vector<TranscriptEntry>& GetTranscript() const = 0;
    virtual void SendPrompt(const std::string& prompt) = 0;
    virtual void Cancel() = 0;
    virtual AgentStatus GetStatus() const = 0;

    struct ISessionListener {
        virtual ~ISessionListener() = default;
        virtual void OnTranscriptUpdated(const TranscriptEntry& entry) = 0;
        virtual void OnStatusChanged(AgentStatus newStatus) = 0;
    };

    virtual void AddListener(ISessionListener* listener) = 0;
    virtual void RemoveListener(ISessionListener* listener) = 0;
};

}
```

### `Packages/AgentWorkspace/include/AgentWorkspace/IHostServices.h`
```cpp
#pragma once

#include "ISession.h"
#include <string>
#include <vector>
#include <memory>

namespace AgentWorkspace {

class IBackgroundProcessHost {
public:
    virtual ~IBackgroundProcessHost() = default;
    virtual std::unique_ptr<IBackgroundProcess> CreateProcess() = 0;
};

class IFolderPickerHost {
public:
    virtual ~IFolderPickerHost() = default;
    virtual std::string PickFolder(const std::string& title, const std::string& defaultPath) = 0;
};

class IAgentHostServices {
public:
    virtual ~IAgentHostServices() = default;
    virtual IBackgroundProcessHost* GetProcessHost() = 0;
    virtual IFolderPickerHost* GetFolderPicker() = 0;
    virtual void LogMessage(const std::string& message, bool isError = false) = 0;
};

}
```

### `Packages/AgentWorkspace/include/AgentWorkspace/WorkspaceState.h`
```cpp
#pragma once

#include "ISession.h"
#include <string>
#include <vector>
#include <memory>

namespace AgentWorkspace {

struct WorkspaceState {
    struct SessionListItem {
        std::string id;
        std::string name;
        std::string providerId;
        AgentStatus status;
        std::chrono::system_clock::time_point lastInteraction;
    };
    std::vector<SessionListItem> sessions;
    std::string selectedSessionId;
    std::shared_ptr<ISession> activeSession;
    std::vector<std::shared_ptr<IAgentProvider>> availableProviders;
    std::string selectedProviderId;
    std::string currentWorkingFolder;
    std::vector<std::string> recentWorkingFolders;
    std::string composerText;
    bool hasActiveBackgroundTasks;
    std::string globalStatusMessage;
};

class IWorkspaceManager {
public:
    virtual ~IWorkspaceManager() = default;
    virtual const WorkspaceState& GetState() const = 0;
    virtual void SelectProvider(const std::string& providerId) = 0;
    virtual void SetWorkingFolder(const std::string& folderPath) = 0;
    virtual std::shared_ptr<ISession> CreateSession(const std::string& name) = 0;
    virtual void SelectSession(const std::string& sessionId) = 0;
    virtual void DeleteSession(const std::string& sessionId) = 0;
    virtual void SendPrompt(const std::string& prompt) = 0;
    virtual void CancelActiveOperation() = 0;

    struct IWorkspaceListener {
        virtual ~IWorkspaceListener() = default;
        virtual void OnStateChanged(const WorkspaceState& newState) = 0;
    };
    virtual void AddListener(IWorkspaceListener* listener) = 0;
    virtual void RemoveListener(IWorkspaceListener* listener) = 0;
};

}
```

### `Packages/AgentWorkspace/include/AgentWorkspace/Providers.h`
```cpp
#pragma once

#include "IAgentProvider.h"

namespace AgentWorkspace {

class BaseCliProvider : public IAgentProvider {
public:
    BaseCliProvider(const std::string& id, const std::string& displayName) : m_id(id), m_displayName(displayName) {}
    std::string GetId() const override { return m_id; }
    std::string GetDisplayName() const override { return m_displayName; }
    bool IsAvailable() const override { return true; }
protected:
    std::string m_id, m_displayName;
};

class GeminiCliProvider : public BaseCliProvider {
public:
    GeminiCliProvider() : BaseCliProvider("gemini-cli", "Gemini CLI") {}
    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        return { "gemini", workingDir, { "--session", sessionId }, sessionId };
    }
    bool SupportsCapability(AgentCapability cap) const override { return true; }
};

class CodexCliProvider : public BaseCliProvider {
public:
    CodexCliProvider() : BaseCliProvider("codex-cli", "Codex CLI") {}
    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        return { "codex", workingDir, { "--cwd", workingDir }, sessionId };
    }
    bool SupportsCapability(AgentCapability cap) const override { return true; }
};

class ClaudeCliProvider : public BaseCliProvider {
public:
    ClaudeCliProvider() : BaseCliProvider("claude-cli", "Claude CLI") {}
    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        return { "claude", workingDir, { "chat" }, sessionId };
    }
    bool SupportsCapability(AgentCapability cap) const override { return true; }
};

}
```

### `Packages/AgentWorkspace/src/WorkspaceManager.cpp`
```cpp
#include "WorkspaceManager.h"
#include "Providers.h"
#include "Session.h"
#include <algorithm>

namespace AgentWorkspace {

WorkspaceManager::WorkspaceManager(IAgentHostServices* hostServices) : m_hostServices(hostServices) {
    m_state.availableProviders.push_back(std::make_shared<GeminiCliProvider>());
    m_state.availableProviders.push_back(std::make_shared<CodexCliProvider>());
    m_state.availableProviders.push_back(std::make_shared<ClaudeCliProvider>());
}

void WorkspaceManager::SelectProvider(const std::string& id) { m_state.selectedProviderId = id; NotifyStateChanged(); }

std::shared_ptr<ISession> WorkspaceManager::CreateSession(const std::string& name) {
    auto it = std::find_if(m_state.availableProviders.begin(), m_state.availableProviders.end(), [&](auto p) { return p->GetId() == m_state.selectedProviderId; });
    if (it == m_state.availableProviders.end()) return nullptr;
    auto session = std::make_shared<Session>("sess_" + std::to_string(m_sessions.size()), name, *it, m_state.currentWorkingFolder, m_hostServices);
    if (session->Initialize()) {
        session->AddListener(this);
        m_sessions[session->GetId()] = session;
        SelectSession(session->GetId());
        return session;
    }
    return nullptr;
}

void WorkspaceManager::NotifyStateChanged() { for (auto* l : m_listeners) l->OnStateChanged(m_state); }

}
```

---

## 2. TodoWorkspace Package

### `Packages/TodoWorkspace/README.md`
```markdown
# TodoWorkspace Package

## Purpose
The `TodoWorkspace` provides a native backend for managing project action items (Features, Bugs, Polish) with a priority-based ordering system. It includes a specialized export feature to generate Steam-ready changelogs from completed tasks.
```

### `Packages/TodoWorkspace/include/TodoWorkspace/TodoItem.h`
```cpp
#pragma once
#include <string>
#include <chrono>

namespace TodoWorkspace {
enum class ItemType { Feature, Bug, Polish };
enum class Priority { OneStar = 1, TwoStar = 2, ThreeStar = 3, FourStar = 4, FiveStar = 5 };
struct TodoItem {
    std::string id, description;
    ItemType type; Priority priority;
    bool isDone, isExported;
    std::chrono::system_clock::time_point createdAt, completedAt;
};
}
```

### `Packages/TodoWorkspace/src/TodoManager.cpp`
```cpp
#include "TodoManager.h"
#include <algorithm>
#include <sstream>

namespace TodoWorkspace {
void TodoManager::AddItem(const std::string& desc, ItemType type, Priority prio) {
    TodoItem item; item.id = "todo_" + std::to_string(m_state.items.size());
    item.description = desc; item.type = type; item.priority = prio;
    item.isDone = false; item.isExported = false;
    item.createdAt = std::chrono::system_clock::now();
    m_state.items.push_back(item);
    SortItems(); NotifyStateChanged();
}

std::string TodoManager::ExportChangelog() {
    std::stringstream ss; ss << "=== CHANGELOG ===\n\n";
    for (auto& item : m_state.items) {
        if (item.isDone && !item.isExported) {
            ss << "[" << (int)item.type << "] " << item.description << "\n";
            item.isExported = true;
        }
    }
    std::string path = m_hostServices->GetProjectRoot() + "/changelog.txt";
    m_hostServices->WriteFile(path, ss.str());
    return path;
}
}
```

---

## 3. Cutscene Package

### `Packages/Cutscene/README.md`
```markdown
# Cutscene Package

## Purpose
The `Cutscene` package provides a timeline-based cutscene framework for the engine. It separates authored cutscene data from the runtime playback logic and uses a host-interface pattern to execute engine commands without hardwiring dependencies.
```

### `Packages/Cutscene/include/Cutscene/CutsceneData.h`
```cpp
#pragma once
#include <string>
#include <vector>
#include <memory>

namespace Cutscene {
enum class ClipType { Wait, Dialogue, CharacterMove, Animation, Camera, Audio, RoomChange, FullscreenImage, ScriptCommand };
class ICutsceneClip {
public:
    virtual ~ICutsceneClip() = default;
    virtual ClipType GetType() const = 0;
    float startTime = 0.0f, duration = 0.0f;
};
class CutsceneTrack {
public:
    std::string name, targetId;
    std::vector<std::shared_ptr<ICutsceneClip>> clips;
};
class CutsceneDefinition {
public:
    std::string id, displayName;
    std::vector<CutsceneTrack> tracks;
};
}
```

### `Packages/Cutscene/src/CutscenePlayer.cpp`
```cpp
#include "CutscenePlayer.h"

namespace Cutscene {
void CutscenePlayer::Play(std::shared_ptr<CutsceneDefinition> def) {
    m_activeInstance = std::make_unique<CutsceneInstance>(def);
    m_activeInstance->status = PlaybackStatus::Playing;
    m_host->OnCutsceneStarted(def->id);
    m_host->SetInputLocked(true);
}

void CutscenePlayer::Update(float dt) {
    if (!m_activeInstance || m_activeInstance->status != PlaybackStatus::Playing) return;
    m_activeInstance->currentTime += dt;
    // ... logic for triggering clips based on currentTime ...
}
}
```
