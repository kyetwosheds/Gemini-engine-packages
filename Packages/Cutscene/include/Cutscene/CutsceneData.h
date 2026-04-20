#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace Cutscene {

/**
 * @brief Types of clips/cues supported by the system.
 */
enum class ClipType {
    Wait,
    Dialogue,
    CharacterMove,
    Animation,
    Camera,
    Audio,
    RoomChange,
    FullscreenImage,
    ScriptCommand
};

/**
 * @brief Base interface for a cutscene clip (an event on a timeline).
 */
class ICutsceneClip {
public:
    virtual ~ICutsceneClip() = default;

    virtual ClipType GetType() const = 0;
    
    float startTime = 0.0f;
    float duration = 0.0f;
    std::string id;

    bool IsInstant() const { return duration <= 0.0f; }
    float GetEndTime() const { return startTime + duration; }
};

/**
 * @brief A track contains a sequence of clips, often targeted at a specific object or system.
 */
class CutsceneTrack {
public:
    std::string name;
    std::string targetId; // e.g., "Player", "Character_01", "Camera", "Global"
    std::vector<std::shared_ptr<ICutsceneClip>> clips;

    // Helper to find clips active at a specific time
    void GetActiveClips(float time, std::vector<ICutsceneClip*>& outClips) const {
        for (auto& clip : clips) {
            if (time >= clip->startTime && time <= clip->GetEndTime()) {
                outClips.push_back(clip.get());
            }
        }
    }
};

/**
 * @brief The authored definition of a cutscene (the "asset").
 */
class CutsceneDefinition {
public:
    std::string id;
    std::string displayName;
    std::vector<CutsceneTrack> tracks;
    
    float GetTotalDuration() const {
        float maxTime = 0.0f;
        for (const auto& track : tracks) {
            for (const auto& clip : track.clips) {
                maxTime = (std::max)(maxTime, clip->GetEndTime());
            }
        }
        return maxTime;
    }
};

} // namespace Cutscene
