#pragma once

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

class UnderWaterEvent final : public urchin::Observer {
    public:
        explicit UnderWaterEvent(urchin::SoundEnvironment&);
        ~UnderWaterEvent() override;

        void notify(urchin::Observable*, int) override;

        bool isUnderWater() const;

    private:
        urchin::SoundEnvironment& soundEnvironment;

        bool bIsUnderWater;
        std::shared_ptr<urchin::SoundComponent> underWaterSound;
};
