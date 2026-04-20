#pragma once

#include "CutsceneData.h"
#include "ICutsceneHostBridge.h"
#include <memory>
#include <set>

namespace Cutscene {

/**
 * @brief Current playback status of a cutscene.
 */
enum class PlaybackStatus {
    Stopped,
    Playing,
    Paused
};

/**
 * @brief A runtime instance of a cutscene being played.
 */
class CutsceneInstance {
public:
    CutsceneInstance(std::shared_ptr<CutsceneDefinition> definition);

    std::shared_ptr<CutsceneDefinition> definition;
    float currentTime = 0.0f;
    PlaybackStatus status = PlaybackStatus::Stopped;
    bool isLooping = false;

    // Track which clips have already been triggered to avoid re-triggering
    std::set<ICutsceneClip*> triggeredClips;
    std::set<ICutsceneClip*> activeClips;
};

/**
 * @brief The controller that drives cutscene playback.
 */
class CutscenePlayer {
public:
    CutscenePlayer(ICutsceneHostBridge* host);
    ~CutscenePlayer();

    /**
     * @brief Starts playing a cutscene definition.
     */
    void Play(std::shared_ptr<CutsceneDefinition> definition);

    /**
     * @brief Pauses the current playback.
     */
    void Pause();

    /**
     * @brief Resumes the current playback.
     */
    void Resume();

    /**
     * @brief Stops and resets the current playback.
     */
    void Stop();

    /**
     * @brief Skips the current cutscene to the end.
     */
    void Skip();

    /**
     * @brief Advances playback time.
     */
    void Update(float deltaTime);

    /**
     * @brief Returns true if a cutscene is currently playing.
     */
    bool IsPlaying() const;

    /**
     * @brief Returns the current playback status.
     */
    PlaybackStatus GetStatus() const;

    /**
     * @brief Returns the current playback time.
     */
    float GetCurrentTime() const;

private:
    void ProcessClips(float oldTime, float newTime);
    void TriggerClipStart(ICutsceneClip* clip);
    void TriggerClipEnd(ICutsceneClip* clip);

    ICutsceneHostBridge* m_host;
    std::unique_ptr<CutsceneInstance> m_activeInstance;
};

} // namespace Cutscene
