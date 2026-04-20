#pragma once

#include "WorkspaceState.h"
#include "IHostServices.h"
#include "Providers.h"
#include "Session.h"
#include <map>
#include <set>

namespace AgentWorkspace {

class WorkspaceManager : public IWorkspaceManager, public ISession::ISessionListener {
public:
    WorkspaceManager(IAgentHostServices* hostServices);

    // IWorkspaceManager implementation
    const WorkspaceState& GetState() const override { return m_state; }

    void SelectProvider(const std::string& providerId) override;
    void SetWorkingFolder(const std::string& folderPath) override;
    std::shared_ptr<ISession> CreateSession(const std::string& name) override;
    void SelectSession(const std::string& sessionId) override;
    void DeleteSession(const std::string& sessionId) override;

    void SendPrompt(const std::string& prompt) override;
    void CancelActiveOperation() override;

    void AddListener(IWorkspaceListener* listener) override;
    void RemoveListener(IWorkspaceListener* listener) override;

    // ISession::ISessionListener implementation
    void OnTranscriptUpdated(const TranscriptEntry& entry) override;
    void OnStatusChanged(AgentStatus newStatus) override;

private:
    void NotifyStateChanged();
    void RefreshSessionList();

    IAgentHostServices* m_hostServices;
    WorkspaceState m_state;
    std::map<std::string, std::shared_ptr<Session>> m_sessions;
    std::set<IWorkspaceListener*> m_listeners;
};

} // namespace AgentWorkspace
