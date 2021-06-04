#pragma once

#include <stdexcept>

#include <UrchinCommon.h>

class CrashReporter : public urchin::SignalReceptor {
    public:
        virtual ~CrashReporter() = default;

        void onLogContainFailure() const;
        void onException(const std::exception& e) const;
        void onSignalReceived(unsigned long) override;

    private:
        void onError() const;
};
