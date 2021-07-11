#include <Urchin3dEngine.h>

#include <game/UnderWaterEvent.h>
using namespace urchin;

UnderWaterEvent::UnderWaterEvent(SoundManager* soundManager) :
        bIsUnderWater(false) {
    auto underWaterSound = std::make_shared<GlobalSound>("sound/underWater.ogg", Sound::SoundCategory::EFFECTS);
    underWaterSoundTrigger = std::make_shared<ManualTrigger>(SoundTrigger::PLAY_LOOP);
    soundManager->addSound(underWaterSound, underWaterSoundTrigger);
}

void UnderWaterEvent::notify(Observable* observable, int notificationType) {
    if (dynamic_cast<Water *>(observable)) {
        switch (notificationType) {
            case Water::MOVE_UNDER_WATER:
                bIsUnderWater = true;
                underWaterSoundTrigger->play();
                break;
            case Water::MOVE_ABOVE_WATER:
                bIsUnderWater = false;
                underWaterSoundTrigger->stop();
                break;
            default:
                ;
        }
    }
}

bool UnderWaterEvent::isUnderWater() const {
    return bIsUnderWater;
}