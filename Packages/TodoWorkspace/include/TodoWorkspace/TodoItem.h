#pragma once

#include <string>
#include <chrono>

namespace TodoWorkspace {

/**
 * @brief Categorization for action items.
 */
enum class ItemType {
    Feature,
    Bug,
    Polish
};

/**
 * @brief Priority level (1 to 5 stars).
 */
enum class Priority {
    OneStar = 1,
    TwoStar = 2,
    ThreeStar = 3,
    FourStar = 4,
    FiveStar = 5
};

/**
 * @brief Represents a single action item in the to-do list.
 */
struct TodoItem {
    std::string id;
    std::string description;
    ItemType type;
    Priority priority;
    bool isDone;
    bool isExported; // Track if this has been included in a previous changelog
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point completedAt;
};

} // namespace TodoWorkspace
