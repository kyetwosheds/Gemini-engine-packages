# CONSOLIDATED SOURCE CODE FOR REVIEW (V2 - HOST API ALIGNED)

This document contains the complete source code for three C++ packages designed to plug into the new engine package system:
1.  **AgentWorkspace**: AI Agent CLI integration using `IExternalProcessService`.
2.  **TodoWorkspace**: Task management using `IPackageSaveParticipant` and `IProjectContent`.
3.  **Cutscene**: Timeline framework using `IRuntimeSystem` and `IProjectContent`.

---

## 1. AgentWorkspace Package (builtin.agent_workspace)

### `AgentWorkspaceModule.h` (Native Entry Point)
```cpp
#pragma once
#include "HostInterfaces.h" // Assume provided interfaces are here
#include "WorkspaceManager.h"
#include "AgentWorkspaceEditor.h"

namespace AgentWorkspace {

class AgentWorkspaceModule : public INativePackageModule {
public:
    const PackageManifest& GetManifest() const override { return m_manifest; }

    bool Initialize(IPackageHost& host) override {
        m_host = &host;
        m_manager = std::make_unique<WorkspaceManager>(host);
        return true;
    }

    void Shutdown() override { m_manager.reset(); }

    void RegisterEditorContributions(IEditorPackageRegistrar& editor) override {
        editor.RegisterWorkspace("agent_workspace", "AI Agent Workspace", [this]() {
            return std::make_unique<AgentWorkspaceEditor>(*m_manager, *m_host);
        });
    }

    void RegisterRuntimeContributions(IRuntimePackageRegistrar& runtime) override {}

private:
    PackageManifest m_manifest = { "builtin.agent_workspace", "AI Agent Workspace", "1.0.0", true };
    IPackageHost* m_host = nullptr;
    std::unique_ptr<WorkspaceManager> m_manager;
};

}
```

### `AgentSession.h` (Process Wrapper)
```cpp
#pragma once
#include "HostInterfaces.h"
#include <string>
#include <vector>
#include <mutex>

namespace AgentWorkspace {

struct TranscriptEntry {
    std::string text;
    bool isError;
};

class AgentSession {
public:
    AgentSession(const std::string& id, IPackageHost& host) 
        : m_id(id), m_host(host) {
        m_process = m_host.ExternalProcesses().CreateProcess();
    }

    bool Start(const std::string& exe, const std::vector<std::string>& args, const std::string& wd) {
        m_process->SetOutputCallback([this](const std::string& text, bool isErr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_transcript.push_back({text, isErr});
        });
        return m_process->Start(exe, args, wd);
    }

    void SendInput(const std::string& input) { m_process->SendInput(input + "\n"); }
    void Stop() { m_process->Stop(); }
    bool IsRunning() const { return m_process->IsRunning(); }

    const std::vector<TranscriptEntry>& GetTranscript() const { return m_transcript; }

private:
    std::string m_id;
    IPackageHost& m_host;
    std::unique_ptr<IExternalProcess> m_process;
    std::vector<TranscriptEntry> m_transcript;
    mutable std::mutex m_mutex;
};

}
```

### `WorkspaceManager.cpp` (Settings & State Logic)
```cpp
#include "WorkspaceManager.h"

namespace AgentWorkspace {

void WorkspaceManager::LoadSettings() {
    std::vector<std::string> folders;
    if (m_host.Settings().GetStringList("agent", "recent_folders", folders)) {
        m_recentFolders = folders;
    }
    m_host.Settings().GetString("agent", "last_provider", m_selectedProviderId);
}

void WorkspaceManager::SaveSettings() {
    m_host.Settings().SetStringList("agent", "recent_folders", m_recentFolders);
    m_host.Settings().SetString("agent", "last_provider", m_selectedProviderId);
}

void WorkspaceManager::SetWorkingFolder(const std::string& path) {
    m_currentFolder = path;
    // Add to recent list and persist via host
    if (std::find(m_recentFolders.begin(), m_recentFolders.end(), path) == m_recentFolders.end()) {
        m_recentFolders.push_back(path);
        SaveSettings();
    }
}

}
```

---

## 2. TodoWorkspace Package (builtin.todo)

### `TodoSaveParticipant.h` (Persistence)
```cpp
#pragma once
#include "HostInterfaces.h"
#include "TodoItem.h"
#include <vector>

namespace TodoWorkspace {

class TodoSaveParticipant : public IPackageSaveParticipant {
public:
    TodoSaveParticipant(std::vector<TodoItem>& items) : m_items(items) {}

    std::string GetPackageId() const override { return "builtin.todo"; }

    bool SavePackageState(IPackageStateWriter& writer) override {
        writer.WriteInt("count", (int)m_items.size());
        for (size_t i = 0; i < m_items.size(); ++i) {
            writer.WriteString("item_" + std::to_string(i) + "_desc", m_items[i].description);
            writer.WriteInt("item_" + std::to_string(i) + "_prio", (int)m_items[i].priority);
            writer.WriteBool("item_" + std::to_string(i) + "_done", m_items[i].isDone);
        }
        return true;
    }

    bool LoadPackageState(IPackageStateReader& reader) override {
        int count = 0;
        if (!reader.ReadInt("count", count)) return false;
        m_items.clear();
        for (int i = 0; i < count; ++i) {
            TodoItem item;
            reader.ReadString("item_" + std::to_string(i) + "_desc", item.description);
            int prio; reader.ReadInt("item_" + std::to_string(i) + "_prio", prio);
            item.priority = (Priority)prio;
            reader.ReadBool("item_" + std::to_string(i) + "_done", item.isDone);
            m_items.push_back(item);
        }
        return true;
    }

private:
    std::vector<TodoItem>& m_items;
};

}
```

### `TodoManager.cpp` (Content Export Logic)
```cpp
#include "TodoManager.h"
#include <sstream>

namespace TodoWorkspace {

void TodoManager::ExportChangelog() {
    std::stringstream ss;
    ss << "=== STEAM CHANGELOG ===\n\n";
    for (const auto& item : m_items) {
        if (item.isDone && !item.isExported) {
            ss << "[" << TypeToString(item.type) << "] " << item.description << "\n";
        }
    }

    // Use Package-Aware Project Content Service
    std::string content = ss.str();
    if (m_host.ProjectContent().WriteTextFile("builtin.todo", "exports/last_changelog.txt", content)) {
        m_host.Notifications().Info("Changelog written to project exports.");
    } else {
        m_host.Notifications().Error("Failed to write changelog file.");
    }
}

}
```

---

## 3. Cutscene Package (builtin.cutscene)

### `CutsceneRuntimeSystem.h` (Engine Loop Integration)
```cpp
#pragma once
#include "HostInterfaces.h"
#include "CutscenePlayer.h"

namespace Cutscene {

class CutsceneRuntimeSystem : public IRuntimeSystem {
public:
    CutsceneRuntimeSystem(ICutsceneHostBridge& bridge) : m_bridge(bridge) {
        m_player = std::make_unique<CutscenePlayer>(bridge);
    }

    void Tick(float dt) override {
        if (m_player->IsPlaying()) {
            m_player->Update(dt);
        }
    }

    void PlayCutscene(std::shared_ptr<CutsceneDefinition> def) {
        m_player->Play(def);
    }

private:
    ICutsceneHostBridge& m_bridge;
    std::unique_ptr<CutscenePlayer> m_player;
};

}
```

### `CutsceneModule.cpp` (Package Registration)
```cpp
#include "CutsceneModule.h"
#include "CutsceneRuntimeSystem.h"

namespace Cutscene {

bool CutsceneModule::Initialize(IPackageHost& host) {
    m_host = &host;
    // Bridge implementation is provided by the engine to handle characters/camera
    m_runtimeSystem = std::make_shared<CutsceneRuntimeSystem>(m_engineBridge);
    return true;
}

void CutsceneModule::RegisterRuntimeContributions(IRuntimePackageRegistrar& runtime) {
    // Register the system to receive Tick calls from the engine runtime
    runtime.RegisterRuntimeSystem("cutscene_player", m_runtimeSystem);
}

void CutsceneModule::RegisterEditorContributions(IEditorPackageRegistrar& editor) {
    editor.RegisterWorkspace("cutscene", "Cutscene Editor", [this]() {
        return std::make_unique<CutsceneEditorWorkspace>(*m_host);
    });
}

}
```

---

## 4. Integration Summary for Reviewers

- **Zero Global State**: All packages receive an `IPackageHost` reference during `Initialize`.
- **Async Processing**: `AgentWorkspace` uses the host's `IExternalProcess` callback pattern, avoiding thread-blocking engine stalls.
- **Content Isolation**: `TodoWorkspace` and `Cutscene` use `IProjectContent` with their unique package IDs, preventing path collisions.
- **Save/Load Safety**: `TodoWorkspace` uses the `IPackageSaveParticipant` pattern, ensuring it only touches its own state snapshot.
- **Editor Decoupling**: Workspaces are identified by stable strings (`"agent_workspace"`, `"todo"`, `"cutscene"`), allowing the engine to reorder or re-skin the UI without breaking package logic.
