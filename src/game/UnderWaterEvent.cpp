#include <Urchin3dEngine.h>

#include <game/UnderWaterEvent.h>
using namespace urchin;

UnderWaterEvent::UnderWaterEvent(SoundEnvironment& soundEnvironment) :
        soundEnvironment(soundEnvironment),
        bIsUnderWater(false) {
    underWaterSound = soundEnvironment.getSoundBuilder().newManualTriggerEffect("sounds/underWater.ogg", PlayBehavior::PLAY_LOOP);
}

UnderWaterEvent::~UnderWaterEvent() {
    soundEnvironment.removeSoundComponent(*underWaterSound);
}

void UnderWaterEvent::notify(Observable* observable, int notificationType) {
    if (dynamic_cast<Water *>(observable)) {
        switch (notificationType) {
            case Water::MOVE_UNDER_WATER:
                bIsUnderWater = true;
                underWaterSound->getManualTrigger().playNew();
                break;
            case Water::MOVE_ABOVE_WATER:
                bIsUnderWater = false;
                underWaterSound->getManualTrigger().stopAll();
                break;
            default:
                ;
        }
    }
}

bool UnderWaterEvent::isUnderWater() const {
    return bIsUnderWater;
}