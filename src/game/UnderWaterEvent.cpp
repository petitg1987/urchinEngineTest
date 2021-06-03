#include <Urchin3dEngine.h>

#include <game/UnderWaterEvent.h>
using namespace urchin;

UnderWaterEvent::UnderWaterEvent(SoundManager* soundManager) :
        bIsUnderWater(false) {
    auto* underWaterSound = new AmbientSound("sound/underWater.flac");
    underWaterSoundTrigger = new ManualTrigger(SoundBehavior(SoundBehavior::PLAY_LOOP, SoundBehavior::SMOOTH_STOP, 1.0));
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