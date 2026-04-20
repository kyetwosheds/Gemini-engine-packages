#pragma once
#include "../HostInterfaces.h"
#include "CalculatorManager.h"

namespace CalculatorWorkspace {

class CalculatorEditor : public Engine::IEditorWorkspace {
public:
    CalculatorEditor(ICalculatorManager& manager) : m_manager(manager) {}
    std::string GetWorkspaceId() const override { return "calculator"; }
    void Tick(float dt) override {}
    void Render() override { /* UI logic to display m_manager.GetState().display */ }
private:
    ICalculatorManager& m_manager;
};

class CalculatorModule : public Engine::INativePackageModule {
public:
    const Engine::PackageManifest& GetManifest() const override { return m_manifest; }
    bool Initialize(Engine::IPackageHost& host) override {
        m_manager = std::make_unique<CalculatorManager>();
        return true;
    }
    void Shutdown() override {}
    void RegisterEditorContributions(Engine::IEditorPackageRegistrar& editor) override {
        editor.RegisterWorkspace("calculator", "Calculator", [this]() {
            return std::make_unique<CalculatorEditor>(*m_manager);
        });
    }
    void RegisterRuntimeContributions(Engine::IRuntimePackageRegistrar& runtime) override {}

private:
    Engine::PackageManifest m_manifest = { "builtin.calculator", "Calculator", "1.0.0", true, {}, {}, {"calculator"}, false, true, "builtin.calculator" };
    std::unique_ptr<CalculatorManager> m_manager;
};

}
