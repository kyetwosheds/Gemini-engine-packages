#pragma once

#include "../../HostInterfaces.h"
#include "../include/TodoWorkspace/TodoItem.h"
#include <vector>

namespace TodoWorkspace {

class TodoSaveParticipant : public Engine::IPackageSaveParticipant {
public:
    TodoSaveParticipant(std::vector<TodoItem>& items, std::function<void()> onLoaded) 
        : m_items(items), m_onLoaded(onLoaded) {}

    std::string GetPackageId() const override { return "builtin.todo"; }

    bool SavePackageState(Engine::IPackageStateWriter& writer) override {
        writer.WriteInt("count", static_cast<int>(m_items.size()));
        for (size_t i = 0; i < m_items.size(); ++i) {
            std::string prefix = "item_" + std::to_string(i) + "_";
            writer.WriteString(prefix + "id", m_items[i].id);
            writer.WriteString(prefix + "desc", m_items[i].description);
            writer.WriteInt(prefix + "type", static_cast<int>(m_items[i].type));
            writer.WriteInt(prefix + "prio", static_cast<int>(m_items[i].priority));
            writer.WriteBool(prefix + "done", m_items[i].isDone);
            writer.WriteBool(prefix + "exported", m_items[i].isExported);
        }
        return true;
    }

    bool LoadPackageState(Engine::IPackageStateReader& reader) override {
        int count = 0;
        // If "count" is missing, we treat it as a valid empty state (default)
        if (!reader.ReadInt("count", count)) {
            m_items.clear();
            if (m_onLoaded) m_onLoaded();
            return true; 
        }

        m_items.clear();
        for (int i = 0; i < count; ++i) {
            std::string prefix = "item_" + std::to_string(i) + "_";
            TodoItem item;
            reader.ReadString(prefix + "id", item.id);
            reader.ReadString(prefix + "desc", item.description);
            
            int type, prio;
            reader.ReadInt(prefix + "type", type);
            reader.ReadInt(prefix + "prio", prio);
            item.type = static_cast<ItemType>(type);
            item.priority = static_cast<Priority>(prio);
            
            reader.ReadBool(prefix + "done", item.isDone);
            reader.ReadBool(prefix + "exported", item.isExported);
            
            // Note: Timestamps are not persisted in this simple version, 
            // but could be added if needed.
            item.createdAt = std::chrono::system_clock::now();
            
            m_items.push_back(item);
        }

        if (m_onLoaded) m_onLoaded();
        return true;
    }

private:
    std::vector<TodoItem>& m_items;
    std::function<void()> m_onLoaded;
};

} // namespace TodoWorkspace
