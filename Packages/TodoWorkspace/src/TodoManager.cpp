#include "TodoManager.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace TodoWorkspace {

static std::string TypeToString(ItemType type) {
    switch (type) {
        case ItemType::Feature: return "[FEATURE]";
        case ItemType::Bug: return "[BUG FIX]";
        case ItemType::Polish: return "[POLISH]";
        default: return "[UNKNOWN]";
    }
}

TodoManager::TodoManager(ITodoHostServices* hostServices)
    : m_hostServices(hostServices) {
    m_state.isEditMode = true;
}

void TodoManager::AddItem(const std::string& description, ItemType type, Priority priority) {
    TodoItem item;
    item.id = GenerateId();
    item.description = description;
    item.type = type;
    item.priority = priority;
    item.isDone = false;
    item.isExported = false;
    item.createdAt = std::chrono::system_clock::now();
    
    m_state.items.push_back(item);
    SortItems();
    NotifyStateChanged();
}

void TodoManager::UpdateItem(const std::string& id, const std::string& description, ItemType type, Priority priority) {
    auto it = std::find_if(m_state.items.begin(), m_state.items.end(), [&](const auto& i) { return i.id == id; });
    if (it != m_state.items.end()) {
        it->description = description;
        it->type = type;
        it->priority = priority;
        SortItems();
        NotifyStateChanged();
    }
}

void TodoManager::ToggleDone(const std::string& id) {
    auto it = std::find_if(m_state.items.begin(), m_state.items.end(), [&](const auto& i) { return i.id == id; });
    if (it != m_state.items.end()) {
        it->isDone = !it->isDone;
        if (it->isDone) {
            it->completedAt = std::chrono::system_clock::now();
        }
        NotifyStateChanged();
    }
}

void TodoManager::DeleteItem(const std::string& id) {
    auto it = std::find_if(m_state.items.begin(), m_state.items.end(), [&](const auto& i) { return i.id == id; });
    if (it != m_state.items.end()) {
        m_state.items.erase(it);
        NotifyStateChanged();
    }
}

void TodoManager::SetEditMode(bool enabled) {
    m_state.isEditMode = enabled;
    NotifyStateChanged();
}

std::string TodoManager::ExportChangelog() {
    std::stringstream ss;
    ss << "=== CHANGELOG - " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << " ===\n\n";

    bool hasAny = false;
    for (auto& item : m_state.items) {
        if (item.isDone && !item.isExported) {
            ss << FormatItemForChangelog(item) << "\n";
            item.isExported = true;
            hasAny = true;
        }
    }

    if (!hasAny) return "";

    std::string content = ss.str();
    
    // Generate filename based on date
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream filename;
    filename << "changelog_" << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S") << ".txt";
    
    std::string path = m_hostServices->GetProjectRoot() + "/" + filename.str();
    m_hostServices->WriteFile(path, content);
    
    m_state.currentChangelogPath = path;
    NotifyStateChanged();
    
    return path;
}

void TodoManager::AddListener(ITodoListener* listener) {
    m_listeners.insert(listener);
}

void TodoManager::RemoveListener(ITodoListener* listener) {
    m_listeners.erase(listener);
}

void TodoManager::NotifyStateChanged() {
    for (auto* l : m_listeners) l->OnStateChanged(m_state);
}

void TodoManager::SortItems() {
    // Sort by priority (high to low), then by creation date (new to old)
    std::sort(m_state.items.begin(), m_state.items.end(), [](const TodoItem& a, const TodoItem& b) {
        if (a.priority != b.priority) {
            return static_cast<int>(a.priority) > static_cast<int>(b.priority);
        }
        return a.createdAt > b.createdAt;
    });
}

std::string TodoManager::GenerateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    for (int i = 0; i < 8; ++i) ss << std::hex << dis(gen);
    return ss.str();
}

std::string TodoManager::FormatItemForChangelog(const TodoItem& item) {
    std::stringstream ss;
    ss << TypeToString(item.type) << " " << item.description;
    return ss.str();
}

} // namespace TodoWorkspace
