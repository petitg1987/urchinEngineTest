#include "Urchin3dEngine.h"

#include "UnderWaterEvent.h"

UnderWaterEvent::UnderWaterEvent(urchin::SoundManager *soundManager) :
        bIsUnderWater(false) {
    auto *underWaterSound = new urchin::AmbientSound("sound/underWater.wav");
    underWaterSoundTrigger = new urchin::ManualTrigger(urchin::SoundBehavior(urchin::SoundBehavior::PLAY_LOOP, urchin::SoundBehavior::SMOOTH_STOP, 1.0));
    soundManager->addSound(underWaterSound, underWaterSoundTrigger);
}

void UnderWaterEvent::notify(urchin::Observable *observable, int notificationType) {
    if (dynamic_cast<urchin::Water *>(observable)) {
        switch (notificationType) {
            case urchin::Water::MOVE_UNDER_WATER:
                bIsUnderWater = true;

                underWaterSoundTrigger->play();
                break;
            case urchin::Water::MOVE_ABOVE_WATER:
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