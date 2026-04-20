#include "CutscenePlayer.h"
#include "CutsceneCues.h"
#include <algorithm>

namespace Cutscene {

CutsceneInstance::CutsceneInstance(std::shared_ptr<CutsceneDefinition> def)
    : definition(def), status(PlaybackStatus::Stopped) {}

CutscenePlayer::CutscenePlayer(ICutsceneHost* host) : m_host(host) {}

CutscenePlayer::~CutscenePlayer() {}

void CutscenePlayer::Play(std::shared_ptr<CutsceneDefinition> definition) {
    if (!definition) return;
    
    Stop();
    m_activeInstance = std::make_unique<CutsceneInstance>(definition);
    m_activeInstance->status = PlaybackStatus::Playing;
    
    if (m_host) {
        m_host->OnCutsceneStarted(definition->id);
        m_host->SetInputLocked(true);
    }
}

void CutscenePlayer::Pause() {
    if (m_activeInstance) m_activeInstance->status = PlaybackStatus::Paused;
}

void CutscenePlayer::Resume() {
    if (m_activeInstance) m_activeInstance->status = PlaybackStatus::Playing;
}

void CutscenePlayer::Stop() {
    if (m_activeInstance) {
        // End all active clips
        for (auto* clip : m_activeInstance->activeClips) {
            TriggerClipEnd(clip);
        }
        
        if (m_host) {
            m_host->OnCutsceneEnded(m_activeInstance->definition->id);
            m_host->SetInputLocked(false);
        }
        m_activeInstance = nullptr;
    }
}

void CutscenePlayer::Skip() {
    if (!m_activeInstance) return;
    
    // In a real implementation, skip logic is complex.
    // For now, we just jump to end.
    float duration = m_activeInstance->definition->GetTotalDuration();
    Update(duration - m_activeInstance->currentTime + 0.1f);
}

void CutscenePlayer::Update(float deltaTime) {
    if (!m_activeInstance || m_activeInstance->status != PlaybackStatus::Playing) return;

    float oldTime = m_activeInstance->currentTime;
    m_activeInstance->currentTime += deltaTime;
    float newTime = m_activeInstance->currentTime;

    ProcessClips(oldTime, newTime);

    if (newTime >= m_activeInstance->definition->GetTotalDuration()) {
        if (m_activeInstance->isLooping) {
            m_activeInstance->currentTime = 0.0f;
            m_activeInstance->triggeredClips.clear();
            m_activeInstance->activeClips.clear();
        } else {
            Stop();
        }
    }
}

bool CutscenePlayer::IsPlaying() const {
    return m_activeInstance != nullptr && m_activeInstance->status == PlaybackStatus::Playing;
}

PlaybackStatus CutscenePlayer::GetStatus() const {
    return m_activeInstance ? m_activeInstance->status : PlaybackStatus::Stopped;
}

float CutscenePlayer::GetCurrentTime() const {
    return m_activeInstance ? m_activeInstance->currentTime : 0.0f;
}

void CutscenePlayer::ProcessClips(float oldTime, float newTime) {
    if (!m_activeInstance) return;

    for (const auto& track : m_activeInstance->definition->tracks) {
        for (const auto& clip : track.clips) {
            ICutsceneClip* clipPtr = clip.get();

            // Check for Start
            if (newTime >= clipPtr->startTime && oldTime < clipPtr->startTime) {
                if (m_activeInstance->triggeredClips.find(clipPtr) == m_activeInstance->triggeredClips.end()) {
                    TriggerClipStart(clipPtr);
                    m_activeInstance->triggeredClips.insert(clipPtr);
                    if (!clipPtr->IsInstant()) {
                        m_activeInstance->activeClips.insert(clipPtr);
                    }
                }
            }

            // Check for End
            if (!clipPtr->IsInstant()) {
                float endTime = clipPtr->GetEndTime();
                if (newTime >= endTime && oldTime < endTime) {
                    if (m_activeInstance->activeClips.find(clipPtr) != m_activeInstance->activeClips.end()) {
                        TriggerClipEnd(clipPtr);
                        m_activeInstance->activeClips.erase(clipPtr);
                    }
                }
            }
        }
    }
}

void CutscenePlayer::TriggerClipStart(ICutsceneClip* clip) {
    if (!m_host) return;

    switch (clip->GetType()) {
        case ClipType::Dialogue: {
            auto d = static_cast<DialogueClip*>(clip);
            m_host->ShowDialogue(d->characterId, d->text, d->showDuration);
            break;
        }
        case ClipType::CharacterMove: {
            auto m = static_cast<CharacterMoveClip*>(clip);
            m_host->MoveCharacter(m->characterId, m->targetX, m->targetY, m->isRelative);
            break;
        }
        case ClipType::Animation: {
            auto a = static_cast<AnimationClip*>(clip);
            m_host->PlayAnimation(a->characterId, a->animName, a->loop);
            break;
        }
        case ClipType::Camera: {
            auto c = static_cast<CameraClip*>(clip);
            m_host->SetCamera(c->camX, c->camY, c->zoom, c->smooth);
            break;
        }
        case ClipType::Audio: {
            auto au = static_cast<AudioClip*>(clip);
            m_host->PlayAudio(au->assetPath, au->volume, au->isMusic);
            break;
        }
        case ClipType::RoomChange: {
            auto r = static_cast<RoomChangeClip*>(clip);
            m_host->ChangeRoom(r->targetRoomId, r->entranceMarker);
            break;
        }
        case ClipType::FullscreenImage: {
            auto i = static_cast<FullscreenImageClip*>(clip);
            m_host->ShowFullscreenImage(i->imageAsset, i->fadeIn);
            break;
        }
        case ClipType::ScriptCommand: {
            auto s = static_cast<ScriptCommandClip*>(clip);
            m_host->ExecuteScript(s->command, s->parameters);
            break;
        }
        default: break;
    }
}

void CutscenePlayer::TriggerClipEnd(ICutsceneClip* clip) {
    if (!m_host) return;

    switch (clip->GetType()) {
        case ClipType::Dialogue: {
            auto d = static_cast<DialogueClip*>(clip);
            m_host->HideDialogue(d->characterId);
            break;
        }
        case ClipType::FullscreenImage: {
            auto i = static_cast<FullscreenImageClip*>(clip);
            m_host->HideFullscreenImage(i->fadeOut);
            break;
        }
        default: break;
    }
}

} // namespace Cutscene
