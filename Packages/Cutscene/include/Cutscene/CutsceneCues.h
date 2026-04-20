#pragma once

#include "CutsceneData.h"
#include <string>

namespace Cutscene {

// --- Timing ---
class WaitClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::Wait; }
};

// --- Dialogue ---
class DialogueClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::Dialogue; }
    std::string text;
    std::string characterId;
    float showDuration = 3.0f; // if 0, waits for click
};

// --- Character Movement ---
class CharacterMoveClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::CharacterMove; }
    std::string characterId;
    float targetX = 0.0f;
    float targetY = 0.0f;
    bool isRelative = false;
};

// --- Animation ---
class AnimationClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::Animation; }
    std::string characterId;
    std::string animName;
    bool loop = false;
};

// --- Camera ---
class CameraClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::Camera; }
    float camX = 0.0f;
    float camY = 0.0f;
    float zoom = 1.0f;
    bool smooth = true;
};

// --- Audio ---
class AudioClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::Audio; }
    std::string assetPath;
    float volume = 1.0f;
    bool isMusic = false;
};

// --- Room Transitions ---
class RoomChangeClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::RoomChange; }
    std::string targetRoomId;
    std::string entranceMarker;
};

// --- Cinematic Panels ---
class FullscreenImageClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::FullscreenImage; }
    std::string imageAsset;
    bool fadeIn = true;
    bool fadeOut = true;
};

// --- Scripting ---
class ScriptCommandClip : public ICutsceneClip {
public:
    ClipType GetType() const override { return ClipType::ScriptCommand; }
    std::string command;
    std::string parameters;
};

} // namespace Cutscene
