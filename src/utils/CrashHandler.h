#ifndef URCHINENGINETEST_CRASHHANDLER_H
#define URCHINENGINETEST_CRASHHANDLER_H

#include <stdexcept>

#include "UrchinCommon.h"

class CrashHandler : public urchin::SignalReceptor {
    public:
        void onLogContainFailure();
        void onException(std::exception e);
        void onSignalReceived(int) override;
};

#endif
