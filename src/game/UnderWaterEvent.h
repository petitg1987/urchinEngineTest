#pragma once

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

class UnderWaterEvent : public urchin::Observer {
    public:
        explicit UnderWaterEvent(urchin::SoundEnvironment&);

        void notify(urchin::Observable*, int) override;

        bool isUnderWater() const;

    private:
        bool bIsUnderWater;
        std::unique_ptr<urchin::ManualSound> underWaterSound;
};
