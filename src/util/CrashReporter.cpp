#include "CrashReporter.h"
using namespace urchin;

void CrashReporter::onLogContainFailure() const {
    onError();
}

void CrashReporter::onException(const std::exception& e) const {
    Logger::instance()->logError("Exception caught: " + std::string(e.what()));
    onError();
}

void CrashReporter::onSignalReceived(unsigned long signalId) {
    Logger::instance()->logError("Signal caught: " + std::to_string(signalId));
    onError();
}

void CrashReporter::onError() const {
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}
