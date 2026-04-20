#pragma once

#include "TodoState.h"
#include <set>

namespace TodoWorkspace {

class TodoManager : public ITodoManager {
public:
    TodoManager(ITodoHostServices* hostServices);

    const TodoState& GetState() const override { return m_state; }

    void AddItem(const std::string& description, ItemType type, Priority priority) override;
    void UpdateItem(const std::string& id, const std::string& description, ItemType type, Priority priority) override;
    void ToggleDone(const std::string& id) override;
    void DeleteItem(const std::string& id) override;
    void SetEditMode(bool enabled) override;
    std::string ExportChangelog() override;

    void AddListener(ITodoListener* listener) override;
    void RemoveListener(ITodoListener* listener) override;

private:
    void NotifyStateChanged();
    void SortItems();
    std::string GenerateId();
    std::string FormatItemForChangelog(const TodoItem& item);

    ITodoHostServices* m_hostServices;
    TodoState m_state;
    std::set<ITodoListener*> m_listeners;
};

} // namespace TodoWorkspace
