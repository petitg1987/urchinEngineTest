#include "CrashHandler.h"

void CrashHandler::onLogContainFailure() {
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}

void CrashHandler::onException(const std::exception& e) {
    urchin::Logger::instance()->logError("Exception caught: " + std::string(e.what()));
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}

void CrashHandler::onSignalReceived(unsigned long signalId) {
    urchin::Logger::instance()->logError("Signal caught: " + std::to_string(signalId));
    std::cerr << "Error detected during application execution. See logs for more details." << std::endl;
}
