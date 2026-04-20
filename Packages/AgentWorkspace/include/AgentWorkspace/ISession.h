#pragma once

#include "IAgentProvider.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace AgentWorkspace {

/**
 * @brief Represents a single log or transcript entry in a session.
 */
struct TranscriptEntry {
    std::string text;
    std::chrono::system_clock::time_point timestamp;
    bool isFromAgent;
};

/**
 * @brief Interface for a background process that runs an agent.
 */
class IBackgroundProcess {
public:
    virtual ~IBackgroundProcess() = default;

    /**
     * @brief Starts the background process using the provided spec.
     */
    virtual bool Start(const ProviderLaunchSpec& spec) = 0;

    /**
     * @brief Stops/Kills the background process.
     */
    virtual void Stop() = 0;

    /**
     * @brief Returns true if the process is currently running.
     */
    virtual bool IsRunning() const = 0;

    /**
     * @brief Sends input to the process (stdin).
     */
    virtual void SendInput(const std::string& text) = 0;

    /**
     * @brief Callback for receiving output from the process (stdout/stderr).
     */
    using OutputCallback = std::function<void(const std::string&, bool isError)>;
    virtual void SetOutputCallback(OutputCallback callback) = 0;
};

/**
 * @brief Represents a session with an agent.
 */
class ISession {
public:
    virtual ~ISession() = default;

    /**
     * @brief Unique session identifier.
     */
    virtual std::string GetId() const = 0;

    /**
     * @brief Human-readable name (e.g., "Main Workspace").
     */
    virtual std::string GetDisplayName() const = 0;

    /**
     * @brief Returns the provider used in this session.
     */
    virtual std::shared_ptr<IAgentProvider> GetProvider() const = 0;

    /**
     * @brief Returns the current working directory of the session.
     */
    virtual std::string GetWorkingDirectory() const = 0;

    /**
     * @brief Returns the full transcript of the session.
     */
    virtual const std::vector<TranscriptEntry>& GetTranscript() const = 0;

    /**
     * @brief Sends a prompt/command to the agent in this session.
     */
    virtual void SendPrompt(const std::string& prompt) = 0;

    /**
     * @brief Cancels any active operation in this session.
     */
    virtual void Cancel() = 0;

    /**
     * @brief Returns the current session status.
     */
    virtual AgentStatus GetStatus() const = 0;

    /**
     * @brief Event listener for session state changes.
     */
    struct ISessionListener {
        virtual ~ISessionListener() = default;
        virtual void OnTranscriptUpdated(const TranscriptEntry& entry) = 0;
        virtual void OnStatusChanged(AgentStatus newStatus) = 0;
    };

    virtual void AddListener(ISessionListener* listener) = 0;
    virtual void RemoveListener(ISessionListener* listener) = 0;
};

} // namespace AgentWorkspace
