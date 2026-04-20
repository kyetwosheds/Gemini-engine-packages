#pragma once

#include "ISession.h"
#include <string>
#include <vector>
#include <memory>
#include <future>

namespace AgentWorkspace {

/**
 * @brief Host-provided service for launching processes.
 * This is implemented by the host (Codex/Engine) and consumed by the package.
 */
class IBackgroundProcessHost {
public:
    virtual ~IBackgroundProcessHost() = default;

    /**
     * @brief Creates a new background process instance managed by the host.
     */
    virtual std::unique_ptr<IBackgroundProcess> CreateProcess() = 0;
};

/**
 * @brief Host-provided service for file/folder operations.
 */
class IFolderPickerHost {
public:
    virtual ~IFolderPickerHost() = default;

    /**
     * @brief Shows a native folder selection dialog.
     */
    virtual std::string PickFolder(const std::string& title, const std::string& defaultPath) = 0;
};

/**
 * @brief Top-level host interface for integration.
 */
class IAgentHostServices {
public:
    virtual ~IAgentHostServices() = default;

    virtual IBackgroundProcessHost* GetProcessHost() = 0;
    virtual IFolderPickerHost* GetFolderPicker() = 0;

    /**
     * @brief Logs message to the host's global output/log system.
     */
    virtual void LogMessage(const std::string& message, bool isError = false) = 0;
};

/**
 * @brief Bridge interface that allows the host to drive the workspace.
 */
class IWorkspaceHostBridge {
public:
    virtual ~IWorkspaceHostBridge() = default;

    /**
     * @brief Notifies the workspace that it should refresh its view/state.
     */
    virtual void Refresh() = 0;

    /**
     * @brief Requests the workspace to focus on a specific session.
     */
    virtual void FocusSession(const std::string& sessionId) = 0;
};

} // namespace AgentWorkspace
