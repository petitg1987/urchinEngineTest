#ifndef URCHINENGINETEST_UNDERWATEREVENT_H
#define URCHINENGINETEST_UNDERWATEREVENT_H

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

class UnderWaterEvent : public urchin::Observer {
    public:
        explicit UnderWaterEvent(urchin::SoundManager *);

        void notify(urchin::Observable*, int) override;

        bool isUnderWater() const;

    private:
        bool bIsUnderWater;
        urchin::ManualTrigger *underWaterSoundTrigger;
};

#endif
