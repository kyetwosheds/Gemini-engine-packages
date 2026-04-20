#pragma once

#include "TodoItem.h"
#include <vector>
#include <memory>
#include <functional>

namespace TodoWorkspace {

/**
 * @brief View-model state for the Todo Workspace.
 */
struct TodoState {
    std::vector<TodoItem> items; // Should be kept sorted by priority
    bool isEditMode;
    std::string currentChangelogPath;
};

/**
 * @brief Interface for managing the Todo list.
 */
class ITodoManager {
public:
    virtual ~ITodoManager() = default;

    virtual const TodoState& GetState() const = 0;

    // Actions
    virtual void AddItem(const std::string& description, ItemType type, Priority priority) = 0;
    virtual void UpdateItem(const std::string& id, const std::string& description, ItemType type, Priority priority) = 0;
    virtual void ToggleDone(const std::string& id) = 0;
    virtual void DeleteItem(const std::string& id) = 0;
    virtual void SetEditMode(bool enabled) = 0;

    /**
     * @brief Exports all 'Done' but 'Not Exported' items to a changelog file.
     * @return Path to the generated file.
     */
    virtual std::string ExportChangelog() = 0;

    /**
     * @brief Listener for state changes.
     */
    struct ITodoListener {
        virtual ~ITodoListener() = default;
        virtual void OnStateChanged(const TodoState& newState) = 0;
    };
    virtual void AddListener(ITodoListener* listener) = 0;
    virtual void RemoveListener(ITodoListener* listener) = 0;
};

/**
 * @brief Host services needed by the TodoWorkspace.
 */
class ITodoHostServices {
public:
    virtual ~ITodoHostServices() = default;
    virtual void WriteFile(const std::string& path, const std::string& content) = 0;
    virtual std::string GetProjectRoot() = 0;
};

} // namespace TodoWorkspace
