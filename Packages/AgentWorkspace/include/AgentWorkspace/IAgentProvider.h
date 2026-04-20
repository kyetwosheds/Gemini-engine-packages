#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace AgentWorkspace {

/**
 * @brief Represents the current state of an agent provider or session.
 */
enum class AgentStatus {
    Idle,
    Initializing,
    Running,
    Busy,
    Cancelling,
    Error,
    Disconnected
};

/**
 * @brief Capabilities that an agent provider might support.
 */
enum class AgentCapability {
    Interactive,
    OneShot,
    BackgroundSession,
    Streaming,
    SessionPersistence,
    FileManipulation
};

/**
 * @brief Configuration for launching an agent provider.
 */
struct ProviderLaunchSpec {
    std::string executablePath;
    std::string workingDirectory;
    std::vector<std::string> arguments;
    std::string sessionId;
};

/**
 * @brief Interface for an agent provider (Gemini CLI, Codex CLI, Claude CLI, etc.)
 */
class IAgentProvider {
public:
    virtual ~IAgentProvider() = default;

    /**
     * @brief Unique identifier for this provider (e.g., "gemini-cli").
     */
    virtual std::string GetId() const = 0;

    /**
     * @brief User-facing display name.
     */
    virtual std::string GetDisplayName() const = 0;

    /**
     * @brief Returns true if the provider's executable is found on the system.
     */
    virtual bool IsAvailable() const = 0;

    /**
     * @brief Creates a launch specification for starting a process.
     */
    virtual ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const = 0;

    /**
     * @brief Checks if the provider supports a specific capability.
     */
    virtual bool SupportsCapability(AgentCapability capability) const = 0;
};

} // namespace AgentWorkspace
