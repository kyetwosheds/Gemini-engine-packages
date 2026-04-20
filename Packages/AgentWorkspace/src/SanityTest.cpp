#include "WorkspaceManager.h"
#include <iostream>

/**
 * This is a compile-only sanity check to ensure the backend foundation
 * is correctly architected and follows the intended patterns.
 */

namespace AgentWorkspace {

// Mock implementation of host services for testing
class MockBackgroundProcess : public IBackgroundProcess {
public:
    bool Start(const ProviderLaunchSpec& spec) override { return true; }
    void Stop() override {}
    bool IsRunning() const override { return true; }
    void SendInput(const std::string& text) override {}
    void SetOutputCallback(OutputCallback callback) override {}
};

class MockProcessHost : public IBackgroundProcessHost {
public:
    std::unique_ptr<IBackgroundProcess> CreateProcess() override {
        return std::make_unique<MockBackgroundProcess>();
    }
};

class MockFolderPicker : public IFolderPickerHost {
public:
    std::string PickFolder(const std::string& title, const std::string& defaultPath) override {
        return "C:/Mock/Path";
    }
};

class MockHostServices : public IAgentHostServices {
public:
    IBackgroundProcessHost* GetProcessHost() override { return &m_processHost; }
    IFolderPickerHost* GetFolderPicker() override { return &m_folderPicker; }
    void LogMessage(const std::string& message, bool isError) override {
        std::cout << (isError ? "[ERROR] " : "[INFO] ") << message << std::endl;
    }

private:
    MockProcessHost m_processHost;
    MockFolderPicker m_folderPicker;
};

void RunSanityTest() {
    MockHostServices hostServices;
    WorkspaceManager manager(&hostServices);

    // Test provider selection
    manager.SelectProvider("gemini-cli");

    // Test working folder
    manager.SetWorkingFolder("C:/Projects/MyGame");

    // Test session creation
    auto session = manager.CreateSession("New Task");
    if (session) {
        manager.SendPrompt("Implement a simple inventory system.");
        manager.CancelActiveOperation();
    }

    // Inspect state
    const auto& state = manager.GetState();
    (void)state; // suppress unused warning
}

} // namespace AgentWorkspace
