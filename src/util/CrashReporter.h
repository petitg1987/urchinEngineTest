#pragma once

#include <UrchinCommon.h>

class CrashReporter final : public urchin::SignalReceptor {
    public:
        ~CrashReporter() override = default;

        void onLogContainFailure() const;
        void onException(const std::exception& e) const;
        void onSignalReceived(unsigned long) override;

    private:
        void onError() const;
};
