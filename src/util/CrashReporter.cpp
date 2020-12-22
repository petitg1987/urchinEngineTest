#include "CrashReporter.h"

void CrashReporter::onLogContainFailure() {
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}

void CrashReporter::onException(const std::exception& e) {
    urchin::Logger::instance()->logError("Exception caught: " + std::string(e.what()));
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}

void CrashReporter::onSignalReceived(unsigned long signalId) {
    urchin::Logger::instance()->logError("Signal caught: " + std::to_string(signalId));
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}
