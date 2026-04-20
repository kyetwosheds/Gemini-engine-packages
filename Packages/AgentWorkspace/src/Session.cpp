#include "Session.h"
#include <chrono>

namespace AgentWorkspace {

Session::Session(const std::string& id, 
                 const std::string& displayName,
                 std::shared_ptr<IAgentProvider> provider,
                 const std::string& workingDir,
                 IAgentHostServices* hostServices)
    : m_id(id), 
      m_displayName(displayName), 
      m_provider(provider), 
      m_workingDir(workingDir), 
      m_hostServices(hostServices) {}

Session::~Session() {
    if (m_process) {
        m_process->Stop();
    }
}

bool Session::Initialize() {
    auto processHost = m_hostServices->GetProcessHost();
    if (!processHost) return false;

    m_process = processHost->CreateProcess();
    if (!m_process) return false;

    SetStatus(AgentStatus::Initializing);

    m_process->SetOutputCallback([this](const std::string& output, bool isError) {
        this->OnProcessOutput(output, isError);
    });

    auto spec = m_provider->CreateLaunchSpec(m_workingDir, m_id);
    if (!m_process->Start(spec)) {
        SetStatus(AgentStatus::Error);
        AppendTranscript("Failed to start agent process.", true);
        return false;
    }

    SetStatus(AgentStatus::Idle);
    return true;
}

void Session::SendPrompt(const std::string& prompt) {
    if (!m_process || !m_process->IsRunning()) {
        AppendTranscript("Error: Agent process is not running.", true);
        return;
    }

    AppendTranscript(prompt, false);
    SetStatus(AgentStatus::Busy);
    m_process->SendInput(prompt + "\n");
}

void Session::Cancel() {
    if (m_status == AgentStatus::Busy) {
        SetStatus(AgentStatus::Cancelling);
        // In a real CLI provider, this might send Ctrl+C or a specific kill signal.
        // For now, we rely on the host-provided process implementation.
        m_process->Stop();
        
        // Restart it if the provider supports it, or mark it as stopped.
        SetStatus(AgentStatus::Idle);
    }
}

void Session::AddListener(ISessionListener* listener) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.insert(listener);
}

void Session::RemoveListener(ISessionListener* listener) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.erase(listener);
}

void Session::SetStatus(AgentStatus newStatus) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_status = newStatus;
    }
    for (auto* l : m_listeners) l->OnStatusChanged(newStatus);
}

void Session::AppendTranscript(const std::string& text, bool isFromAgent) {
    TranscriptEntry entry{ text, std::chrono::system_clock::now(), isFromAgent };
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_transcript.push_back(entry);
    }
    for (auto* l : m_listeners) l->OnTranscriptUpdated(entry);
}

void Session::OnProcessOutput(const std::string& output, bool isError) {
    // If it's the end of a response, we might set status back to Idle.
    // This depends on the specific CLI protocol. 
    // For this generic foundation, we just append to transcript.
    AppendTranscript(output, true);
    
    // Simple heuristic: if we were busy and we got output, we might be idle now
    // (though real streaming needs better detection of "done").
    if (m_status == AgentStatus::Busy) {
        SetStatus(AgentStatus::Idle);
    }
}

} // namespace AgentWorkspace
