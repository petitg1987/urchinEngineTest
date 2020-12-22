#ifndef URCHINENGINETEST_CRASHREPORTER_H
#define URCHINENGINETEST_CRASHREPORTER_H

#include <stdexcept>

#include "UrchinCommon.h"

class CrashReporter : public urchin::SignalReceptor {
    public:
        void onLogContainFailure();
        void onException(const std::exception& e);
        void onSignalReceived(unsigned long) override;
};

#endif
