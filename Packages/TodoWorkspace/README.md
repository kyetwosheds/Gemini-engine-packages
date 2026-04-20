# TodoWorkspace Package

## Purpose
The `TodoWorkspace` provides a native backend for managing project action items (Features, Bugs, Polish) with a priority-based ordering system. It includes a specialized export feature to generate Steam-ready changelogs from completed tasks.

## Features
- **Categorization**: Mark items as `Feature`, `Bug`, or `Polish`.
- **Priority System**: 5-star priority ranking. Items are automatically sorted by priority (High to Low).
- **Edit/Execution Modes**: 
  - **Edit Mode**: Add, modify, or delete items.
  - **Execution Mode**: Tick off items as they are completed.
- **Steam-Ready Export**: One-click generation of a `changelog_YYYYMMDD_HHMMSS.txt` file containing all items completed since the last export.

## Architecture
- **TodoItem**: Data model for tasks.
- **TodoState**: View-model that the future UI binds to.
- **ITodoManager**: Interface for all list manipulations and exports.
- **ITodoHostServices**: Interface for file system operations provided by the engine host.

## Integration Hooks
To fully integrate this package:
1. Implement `ITodoHostServices` to allow the manager to write changelog files to the project root.
2. Bind the `TodoState` to a UI panel (e.g., a sidebar or dedicated editor tab).
3. Connect the "Export" button to `manager->ExportChangelog()`.

## Steam Changelog Format
The exported file uses the following format:
```
=== CHANGELOG - [DATE] ===

[FEATURE] Added new inventory system
[BUG FIX] Fixed crash on level reload
[POLISH] Improved shadow quality in forest
```
This is designed to be easily copied and pasted into Steam's update description field.
