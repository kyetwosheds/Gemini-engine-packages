#pragma once

#include <string>

namespace Cutscene {

/**
 * @brief Host interface for the engine.
 * The CutscenePlayer uses this to execute the actual engine commands.
 */
class ICutsceneHostBridge {
public:
    virtual ~ICutsceneHostBridge() = default;

    // Execution commands
    virtual void ShowDialogue(const std::string& characterId, const std::string& text, float duration) = 0;
    virtual void HideDialogue(const std::string& characterId) = 0;

    virtual void MoveCharacter(const std::string& characterId, float x, float y, bool relative) = 0;
    virtual void PlayAnimation(const std::string& characterId, const std::string& animName, bool loop) = 0;

    virtual void SetCamera(float x, float y, float zoom, bool smooth) = 0;
    
    virtual void PlayAudio(const std::string& assetPath, float volume, bool isMusic) = 0;
    virtual void StopAudio(const std::string& assetPath) = 0;

    virtual void ChangeRoom(const std::string& roomId, const std::string& entrance) = 0;

    virtual void ShowFullscreenImage(const std::string& assetPath, bool fadeIn) = 0;
    virtual void HideFullscreenImage(bool fadeOut) = 0;

    virtual void ExecuteScript(const std::string& command, const std::string& params) = 0;

    // Engine control
    virtual void SetInputLocked(bool locked) = 0;
    virtual void SetEngineTimeScale(float scale) = 0;

    // Reporting
    virtual void OnCutsceneStarted(const std::string& id) = 0;
    virtual void OnCutsceneEnded(const std::string& id) = 0;
};

} // namespace Cutscene
