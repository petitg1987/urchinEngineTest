#ifndef URCHINENGINETEST_CRASHREPORTER_H
#define URCHINENGINETEST_CRASHREPORTER_H

#include <stdexcept>

#include "UrchinCommon.h"

class CrashReporter : public urchin::SignalReceptor {
    public:
        void onLogContainFailure() const;
        void onException(const std::exception& e) const;
        void onSignalReceived(unsigned long) override;

    private:
        void onError() const;
};

#endif
