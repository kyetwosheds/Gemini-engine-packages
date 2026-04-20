#include "WorkspaceManager.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

namespace AgentWorkspace {

static std::string GenerateId() {
    static std::random_device rd;
    static std::mt19_pair_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << dis(gen);
    return ss.str();
}

WorkspaceManager::WorkspaceManager(IAgentHostServices* hostServices)
    : m_hostServices(hostServices) {
    
    // Register default providers
    m_state.availableProviders.push_back(std::make_shared<GeminiCliProvider>());
    m_state.availableProviders.push_back(std::make_shared<CodexCliProvider>());
    m_state.availableProviders.push_back(std::make_shared<ClaudeCliProvider>());
    
    if (!m_state.availableProviders.empty()) {
        m_state.selectedProviderId = m_state.availableProviders[0]->GetId();
    }

    m_state.hasActiveBackgroundTasks = false;
    m_state.globalStatusMessage = "Ready";
}

void WorkspaceManager::SelectProvider(const std::string& providerId) {
    m_state.selectedProviderId = providerId;
    NotifyStateChanged();
}

void WorkspaceManager::SetWorkingFolder(const std::string& folderPath) {
    m_state.currentWorkingFolder = folderPath;
    
    // Add to recent if not present
    auto it = std::find(m_state.recentWorkingFolders.begin(), m_state.recentWorkingFolders.end(), folderPath);
    if (it == m_state.recentWorkingFolders.end()) {
        m_state.recentWorkingFolders.insert(m_state.recentWorkingFolders.begin(), folderPath);
        if (m_state.recentWorkingFolders.size() > 10) {
            m_state.recentWorkingFolders.pop_back();
        }
    }
    
    NotifyStateChanged();
}

std::shared_ptr<ISession> WorkspaceManager::CreateSession(const std::string& name) {
    auto providerIt = std::find_if(m_state.availableProviders.begin(), m_state.availableProviders.end(),
        [this](const auto& p) { return p->GetId() == m_state.selectedProviderId; });
    
    if (providerIt == m_state.availableProviders.end()) return nullptr;

    std::string id = GenerateId();
    auto session = std::make_shared<Session>(id, name, *providerIt, m_state.currentWorkingFolder, m_hostServices);
    
    if (session->Initialize()) {
        session->AddListener(this);
        m_sessions[id] = session;
        SelectSession(id);
        RefreshSessionList();
        return session;
    }
    
    return nullptr;
}

void WorkspaceManager::SelectSession(const std::string& sessionId) {
    auto it = m_sessions.find(sessionId);
    if (it != m_sessions.end()) {
        m_state.selectedSessionId = sessionId;
        m_state.activeSession = it->second;
        NotifyStateChanged();
    }
}

void WorkspaceManager::DeleteSession(const std::string& sessionId) {
    auto it = m_sessions.find(sessionId);
    if (it != m_sessions.end()) {
        it->second->RemoveListener(this);
        m_sessions.erase(it);
        
        if (m_state.selectedSessionId == sessionId) {
            m_state.selectedSessionId = "";
            m_state.activeSession = nullptr;
        }
        
        RefreshSessionList();
    }
}

void WorkspaceManager::SendPrompt(const std::string& prompt) {
    if (m_state.activeSession) {
        m_state.activeSession->SendPrompt(prompt);
    }
}

void WorkspaceManager::CancelActiveOperation() {
    if (m_state.activeSession) {
        m_state.activeSession->Cancel();
    }
}

void WorkspaceManager::AddListener(IWorkspaceListener* listener) {
    m_listeners.insert(listener);
}

void WorkspaceManager::RemoveListener(IWorkspaceListener* listener) {
    m_listeners.erase(listener);
}

void WorkspaceManager::OnTranscriptUpdated(const TranscriptEntry& entry) {
    NotifyStateChanged();
}

void WorkspaceManager::OnStatusChanged(AgentStatus newStatus) {
    // Update global background task state
    bool anyBusy = false;
    for (const auto& pair : m_sessions) {
        if (pair.second->GetStatus() == AgentStatus::Busy) {
            anyBusy = true;
            break;
        }
    }
    m_state.hasActiveBackgroundTasks = anyBusy;
    
    RefreshSessionList(); // Status is shown in list
}

void WorkspaceManager::NotifyStateChanged() {
    for (auto* l : m_listeners) l->OnStateChanged(m_state);
}

void WorkspaceManager::RefreshSessionList() {
    m_state.sessions.clear();
    for (const auto& pair : m_sessions) {
        WorkspaceState::SessionListItem item;
        item.id = pair.first;
        item.name = pair.second->GetDisplayName();
        item.providerId = pair.second->GetProvider()->GetId();
        item.status = pair.second->GetStatus();
        item.lastInteraction = std::chrono::system_clock::now(); // Should track real time
        m_state.sessions.push_back(item);
    }
    NotifyStateChanged();
}

} // namespace AgentWorkspace
