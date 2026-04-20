#pragma once

#include "ISession.h"
#include "IHostServices.h"
#include <vector>
#include <string>
#include <mutex>
#include <set>

namespace AgentWorkspace {

class Session : public ISession, public std::enable_shared_from_this<Session> {
public:
    Session(const std::string& id, 
            const std::string& displayName,
            std::shared_ptr<IAgentProvider> provider,
            const std::string& workingDir,
            IAgentHostServices* hostServices);

    ~Session();

    // ISession implementation
    std::string GetId() const override { return m_id; }
    std::string GetDisplayName() const override { return m_displayName; }
    std::shared_ptr<IAgentProvider> GetProvider() const override { return m_provider; }
    std::string GetWorkingDirectory() const override { return m_workingDir; }
    const std::vector<TranscriptEntry>& GetTranscript() const override { return m_transcript; }
    std::vector<TranscriptEntry> GetTranscriptCopy() const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_transcript;
    }
    void SendPrompt(const std::string& prompt) override;
    void Cancel() override;
    AgentStatus GetStatus() const override { return m_status; }
    void AddListener(ISessionListener* listener) override;
    void RemoveListener(ISessionListener* listener) override;

    /**
     * @brief Initializes the session and starts the background process.
     */
    bool Initialize();

private:
    void SetStatus(AgentStatus newStatus);
    void AppendTranscript(const std::string& text, bool isFromAgent);
    void OnProcessOutput(const std::string& output, bool isError);

    std::string m_id;
    std::string m_displayName;
    std::shared_ptr<IAgentProvider> m_provider;
    std::string m_workingDir;
    IAgentHostServices* m_hostServices;

    AgentStatus m_status = AgentStatus::Idle;
    std::vector<TranscriptEntry> m_transcript;
    std::unique_ptr<IBackgroundProcess> m_process;
    
    std::set<ISessionListener*> m_listeners;
    mutable std::mutex m_mutex;
};

} // namespace AgentWorkspace
