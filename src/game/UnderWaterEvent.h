#pragma once

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

class UnderWaterEvent : public urchin::Observer {
    public:
        explicit UnderWaterEvent(urchin::SoundManager *);

        void notify(urchin::Observable*, int) override;

        bool isUnderWater() const;

    private:
        bool bIsUnderWater;
        std::shared_ptr<urchin::ManualTrigger> underWaterSoundTrigger;
};
