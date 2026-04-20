#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Engine {

// --- Manifest & Package Model ---
struct PackageManifest {
    std::string id;
    std::string displayName;
    std::string version;
    bool isBuiltIn = false;
    std::vector<std::string> dependencies;
    std::vector<std::string> contentRoots;
    std::vector<std::string> workspaceIds;
    bool hasRuntime = false;
    bool hasEditor = false;
    std::string nativeEntry;
};

// --- Host Services ---
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void Info(const std::string& msg) = 0;
    virtual void Error(const std::string& msg) = 0;
};

class IProjectContent {
public:
    virtual ~IProjectContent() = default;
    virtual std::string ResolveReadPath(const std::string& pkgId, const std::string& path) const = 0;
    virtual bool ReadTextFile(const std::string& pkgId, const std::string& path, std::string& outText) const = 0;
    virtual bool WriteTextFile(const std::string& pkgId, const std::string& path, const std::string& text) = 0;
};

class ISettingsStore {
public:
    virtual ~ISettingsStore() = default;
    virtual bool GetString(const std::string& scope, const std::string& key, std::string& out) const = 0;
    virtual void SetString(const std::string& scope, const std::string& key, const std::string& val) = 0;
    virtual bool GetStringList(const std::string& scope, const std::string& key, std::vector<std::string>& out) const = 0;
    virtual void SetStringList(const std::string& scope, const std::string& key, const std::vector<std::string>& val) = 0;
};

class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void Info(const std::string& message) = 0;
    virtual void Warning(const std::string& message) = 0;
    virtual void Error(const std::string& message) = 0;
};

class IExternalProcess {
public:
    using OutputCallback = std::function<void(const std::string& text, bool isStdErr)>;
    virtual ~IExternalProcess() = default;
    virtual bool Start(const std::string& exe, const std::vector<std::string>& args, const std::string& wd) = 0;
    virtual void SendInput(const std::string& text) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual void SetOutputCallback(OutputCallback cb) = 0;
};

class IExternalProcessService {
public:
    virtual ~IExternalProcessService() = default;
    virtual std::unique_ptr<IExternalProcess> CreateProcess() = 0;
};

class IPackageStateWriter {
public:
    virtual ~IPackageStateWriter() = default;
    virtual void WriteString(const std::string& key, const std::string& val) = 0;
    virtual void WriteBool(const std::string& key, bool val) = 0;
    virtual void WriteInt(const std::string& key, int val) = 0;
};

class IPackageStateReader {
public:
    virtual ~IPackageStateReader() = default;
    virtual bool ReadString(const std::string& key, std::string& val) const = 0;
    virtual bool ReadBool(const std::string& key, bool& val) const = 0;
    virtual bool ReadInt(const std::string& key, int& val) const = 0;
};

class IPackageSaveParticipant {
public:
    virtual ~IPackageSaveParticipant() = default;
    virtual std::string GetPackageId() const = 0;
    virtual bool SavePackageState(IPackageStateWriter& writer) = 0;
    virtual bool LoadPackageState(IPackageStateReader& reader) = 0;
};

class IRuntimeSystem {
public:
    virtual ~IRuntimeSystem() = default;
    virtual void Tick(float dt) = 0;
};

class IRuntimePackageRegistrar {
public:
    virtual ~IRuntimePackageRegistrar() = default;
    virtual void RegisterRuntimeSystem(const std::string& id, std::shared_ptr<IRuntimeSystem> system) = 0;
    virtual void RegisterSaveParticipant(const std::string& id, std::shared_ptr<IPackageSaveParticipant> participant) = 0;
};

class IEditorWorkspace {
public:
    virtual ~IEditorWorkspace() = default;
    virtual std::string GetWorkspaceId() const = 0;
    virtual void Tick(float dt) = 0;
    virtual void Render() = 0;
};

class IEditorPackageRegistrar {
public:
    virtual ~IEditorPackageRegistrar() = default;
    virtual void RegisterWorkspace(const std::string& id, const std::string& name, std::function<std::unique_ptr<IEditorWorkspace>()> factory) = 0;
};

class IPackageHost {
public:
    virtual ~IPackageHost() = default;
    virtual ILogger& Log() = 0;
    virtual ISettingsStore& Settings() = 0;
    virtual IProjectContent& ProjectContent() = 0;
    virtual INotificationService& Notifications() = 0;
    virtual IExternalProcessService& ExternalProcesses() = 0;
};

class INativePackageModule {
public:
    virtual ~INativePackageModule() = default;
    virtual const PackageManifest& GetManifest() const = 0;
    virtual bool Initialize(IPackageHost& host) = 0;
    virtual void Shutdown() = 0;
    virtual void RegisterEditorContributions(IEditorPackageRegistrar& editor) = 0;
    virtual void RegisterRuntimeContributions(IRuntimePackageRegistrar& runtime) = 0;
};

} // namespace Engine
