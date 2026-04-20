#include "CutscenePlayer.h"
#include "CutsceneCues.h"

namespace Cutscene {

/**
 * @brief Demonstration of how to author a cutscene in code (for testing).
 */
void CreateSampleCutscene() {
    auto def = std::make_shared<CutsceneDefinition>();
    def->id = "intro_scene";
    def->displayName = "Intro Cinematic";

    // --- Track 1: Player ---
    CutsceneTrack playerTrack;
    playerTrack.name = "Player Actions";
    playerTrack.targetId = "Player";

    auto move = std::make_shared<CharacterMoveClip>();
    move->startTime = 0.0f;
    move->duration = 2.0f;
    move->targetX = 100.0f;
    move->targetY = 200.0f;
    playerTrack.clips.push_back(move);

    auto anim = std::make_shared<AnimationClip>();
    anim->startTime = 2.0f;
    anim->duration = 1.0f;
    anim->animName = "wave";
    playerTrack.clips.push_back(anim);

    def->tracks.push_back(playerTrack);

    // --- Track 2: Narrator ---
    CutsceneTrack narratorTrack;
    narratorTrack.name = "Dialogue";

    auto dial1 = std::make_shared<DialogueClip>();
    dial1->startTime = 3.0f;
    dial1->duration = 3.0f;
    dial1->text = "Welcome to the adventure!";
    dial1->characterId = "Narrator";
    narratorTrack.clips.push_back(dial1);

    def->tracks.push_back(narratorTrack);

    // --- Track 3: Camera ---
    CutsceneTrack camTrack;
    camTrack.name = "Camera System";

    auto cam1 = std::make_shared<CameraClip>();
    cam1->startTime = 0.0f;
    cam1->camX = 50.0f;
    cam1->camY = 50.0f;
    cam1->zoom = 1.2f;
    camTrack.clips.push_back(cam1);

    def->tracks.push_back(camTrack);
}

} // namespace Cutscene
