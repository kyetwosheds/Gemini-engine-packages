#pragma once

#include "IAgentProvider.h"
#include <string>

namespace AgentWorkspace {

/**
 * @brief Base class for CLI-based providers.
 */
class BaseCliProvider : public IAgentProvider {
public:
    BaseCliProvider(const std::string& id, const std::string& displayName)
        : m_id(id), m_displayName(displayName) {}

    std::string GetId() const override { return m_id; }
    std::string GetDisplayName() const override { return m_displayName; }

    /**
     * @brief Simple executable discovery - can be overridden for more complex logic.
     */
    virtual bool IsAvailable() const override {
        // In a real implementation, this would check the system PATH.
        return true; 
    }

protected:
    std::string m_id;
    std::string m_displayName;
};

/**
 * @brief Provider for Gemini CLI.
 */
class GeminiCliProvider : public BaseCliProvider {
public:
    GeminiCliProvider() : BaseCliProvider("gemini-cli", "Gemini CLI") {}

    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        ProviderLaunchSpec spec;
        spec.executablePath = "gemini"; // Assumes in PATH
        spec.workingDirectory = workingDir;
        spec.arguments = { "--session", sessionId };
        return spec;
    }

    bool SupportsCapability(AgentCapability capability) const override {
        switch (capability) {
            case AgentCapability::Interactive:
            case AgentCapability::Streaming:
            case AgentCapability::SessionPersistence:
                return true;
            default:
                return false;
        }
    }
};

/**
 * @brief Provider for Codex CLI.
 */
class CodexCliProvider : public BaseCliProvider {
public:
    CodexCliProvider() : BaseCliProvider("codex-cli", "Codex CLI") {}

    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        ProviderLaunchSpec spec;
        spec.executablePath = "codex";
        spec.workingDirectory = workingDir;
        spec.arguments = { "--cwd", workingDir };
        if (!sessionId.empty()) spec.arguments.push_back("--session=" + sessionId);
        return spec;
    }

    bool SupportsCapability(AgentCapability capability) const override {
        return true; // Codex supports everything
    }
};

/**
 * @brief Provider for Claude CLI.
 */
class ClaudeCliProvider : public BaseCliProvider {
public:
    ClaudeCliProvider() : BaseCliProvider("claude-cli", "Claude CLI") {}

    ProviderLaunchSpec CreateLaunchSpec(const std::string& workingDir, const std::string& sessionId = "") const override {
        ProviderLaunchSpec spec;
        spec.executablePath = "claude";
        spec.workingDirectory = workingDir;
        spec.arguments = { "chat" };
        return spec;
    }

    bool SupportsCapability(AgentCapability capability) const override {
        switch (capability) {
            case AgentCapability::Interactive:
            case AgentCapability::Streaming:
                return true;
            default:
                return false;
        }
    }
};

} // namespace AgentWorkspace
