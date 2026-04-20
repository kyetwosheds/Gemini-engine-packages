#pragma once

#include "../HostInterfaces.h"
#include "TodoManager.h"
#include "TodoSaveParticipant.h"

namespace TodoWorkspace {

class TodoModule : public Engine::INativePackageModule {
public:
    const Engine::PackageManifest& GetManifest() const override { return m_manifest; }

    bool Initialize(Engine::IPackageHost& host) override {
        m_host = &host;
        m_manager = std::make_unique<TodoManager>(nullptr); // Host services bridge needed
        
        m_saveParticipant = std::make_shared<TodoSaveParticipant>(
            const_cast<std::vector<TodoItem>&>(m_manager->GetState().items),
            [this]() { /* OnLoaded */ }
        );
        
        return true;
    }

    void Shutdown() override {
        m_manager.reset();
    }

    void RegisterEditorContributions(Engine::IEditorPackageRegistrar& editor) override {
        editor.RegisterWorkspace("todo", "Project Todo List", [this]() {
            // return std::make_unique<TodoEditorWorkspace>(*m_manager);
            return nullptr; // Placeholder for UI
        });
    }

    void RegisterRuntimeContributions(Engine::IRuntimePackageRegistrar& runtime) override {
        runtime.RegisterSaveParticipant("builtin.todo", m_saveParticipant);
    }

private:
    Engine::PackageManifest m_manifest = {
        "builtin.todo",
        "Todo Workspace",
        "1.0.0",
        true, // isBuiltIn
        {},   // dependencies
        {},   // contentRoots
        {"todo"}, // workspaceIds
        false, // hasRuntime
        true,  // hasEditor
        "builtin.todo" // nativeEntry
    };

    Engine::IPackageHost* m_host = nullptr;
    std::unique_ptr<TodoManager> m_manager;
    std::shared_ptr<TodoSaveParticipant> m_saveParticipant;
};

} // namespace TodoWorkspace
